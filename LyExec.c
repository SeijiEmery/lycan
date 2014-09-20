//
//  LyExec.c
//  Lycan
//
//  Created by Seiji Emery on 2/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "LyCore.h"
#include "LyQueue.h"
#include "LyInstr.h"
#include "LyString.h"
#include "LyTable.h"
#include "LyNumber.h"
#include "LyLog.h"

#include "LyExec.h"
// ====================
// Local helper macros:
// ====================
// raise (error) - encapsulates call to LyLogError (so I can change the implementation w/out changing all error handling code)
#define raise(args...) LyLogError(args)
// Get a variable from table (eg. if current buffer is a LyName (eg. "x"), this will set the buffer to the value stored in table
// that corresponds to x):
#define getVar(table, name) LyTypeOf(name) == LY_T_NAME ? LyTable_getItem(table, name) : name
// Execute a code block
#define execBlock(block) LyExec(ns, LyClone(block), LYI_ENDQUEUE)

void * LyRun (LyTable *ns, LyQueue *queue) {
    return LyExec(ns, queue, LYI_ENDQUEUE);
}
void * _LyExec (LyTable *ns, LyQueue *queue, LyInstrType endInstr);
/*
 LyExec is a wrapper for _LyExec:
 if the return value of _LyExec is a name (eg. the input is ">> x\n"),
 then LyExec should return the value assigned to the name, not the name itself.
 Hence the wrapper.
*/

void * LyExec (LyTable *ns, LyQueue *queue, LyInstrType endInstr) {
    LyLog(LYL_EXEC_DEBUG, "Beginning exec:");
    LyLog_indent();
    LyDebug(queue);
    void * retVal = _LyExec(ns, queue, endInstr);
    LyLog(LYL_EXEC_DEBUG, "returning:");
    LyLog_indent();
    LyDebug(retVal);
    LyLog_dedent();
    LyLog_dedent();
    LyLog(LYL_EXEC_DEBUG, "Ending exec.");
    if (retVal == NULL)
        return NULL;
    
    // If the return value is a LyName, lookup the value associated with LyName
    if (LyTypeOf(retVal) == LY_T_NAME) {
        void *val = getVar(ns, retVal);
        if (val == NULL) {
            // An entry for the name does not exist in ns.
            // That is a problem.
            LyString *name = retVal;
            raise(LYL_EXEC_ERROR, "Name %s is not defined.", name->string);
            return NULL;
        }
        retVal = val;
            
        if (LyTypeOf(retVal) == LY_T_BLOCK) {
            LyLog(LYL_EXEC_DEBUG, "executing block.");
            LyLog_indent();
            return execBlock(retVal);
            LyLog_dedent();
        }
    }
    if (LyTypeOf(retVal) == LY_T_GROUP) {
        LyLog(LYL_EXEC_DEBUG, "executing group.");
        LyLog_indent();
        return execBlock(retVal);
        LyLog_dedent();
    }
    return retVal;
}

#define lyExecErrorCode bool
#define lyExecError   1
#define lyExecNoError 0

typedef struct LyExecPacket {
    void *buffer;
    lyExecErrorCode err;
}LyExecPacket;

LyExecPacket _LyExec_set (void *buffer, LyTable *ns, LyQueue *queue);
LyExecPacket _LyExec_add (void *buffer, LyTable *ns, LyQueue *queue);
LyExecPacket _LyExec_sub (void *buffer, LyTable *ns, LyQueue *queue);
LyExecPacket _LyExec_mul (void *buffer, LyTable *ns, LyQueue *queue);
LyExecPacket _LyExec_div (void *buffer, LyTable *ns, LyQueue *queue);


// ===============
// _LyExec macros:
// ===============

// handleCase: takes the return value of a _LyExec helper function, checks it for errors, and procedes
// as neccessary.
// eg: case LYI_SET: handleCase(_LyExec_set(buffer, ns, queue))
#define handleCase(stuff) { LyExecPacket ret = stuff; if (ret.err) return NULL; buffer = ret.buffer; } break

// Error handling: function (long name) constructs an error packet
// the macro maps a short name (simple & obvious) to the constructor function,
// and is undefined later to prevent name collision.
// usage: return execError; or return execPacket(buffer);
    //LyExecPacket LyExec_errorPacket () { LyExecPacket p = {NULL, lyExecError}; return p; }
    //#define execError LyExec_errorPacket()
LyExecPacket LyExec_errorPacket = {NULL, lyExecError};
#define execError LyExec_errorPacket
LyExecPacket LyExec_returnPacket (void *buffer) { LyExecPacket p = {buffer, lyExecNoError}; return p; }
#define execPacket(buffer) LyExec_returnPacket(buffer)

void * _LyExec (LyTable *ns, LyQueue *queue, LyInstrType endInstr) {
    void *buffer = NULL;
    void *next;
    
    while (1) {
        next = LyQueue_pop(queue);
        LyLog(LYL_EXEC_DEBUG, "==exec cycle==");
        LyLog(LYL_EXEC_DEBUG, "Buffer:");
            LyLog_indent();
                LyDebug(buffer);
            LyLog_dedent();
        LyLog(LYL_EXEC_DEBUG, "Next:");
            LyLog_indent();
                LyDebug(next);
            LyLog_dedent();
        LyLog(LYL_EXEC_DEBUG, "Queue state:");
            LyLog_indent();
                LyDebug(queue);
            LyLog_dedent();
        if (next == NULL) return buffer;
        
        if (LyTypeOf(next) == LY_T_INSTR) {
            LyInstr *instr = next;
            if (instr->type == endInstr) {
                return buffer;
            }
            switch(instr->type) {
                case LYI_SET: 
                handleCase(_LyExec_set(buffer, ns, queue));
                case LYI_ADD:
                handleCase(_LyExec_add(buffer, ns, queue));
                case LYI_SUB:
                handleCase(_LyExec_sub(buffer, ns, queue));
                case LYI_MUL:
                handleCase(_LyExec_mul(buffer, ns, queue));
                case LYI_DIV:
                handleCase(_LyExec_div(buffer, ns, queue));
                case LYI_ENDLINE:
                return LyExec(ns, queue, endInstr);
                case LYI_ENDQUEUE:
                return buffer;
                default:
                raise(LYL_EXEC_ERROR, "Unimplemented instruction: %s", LyInstr_toString(instr));
                return NULL;
            }
        } else {
            if (buffer != NULL)
                LyLog(LYL_EXEC_WARN, "Overwriting value %s with %s", 
                      LyTypeToString(LyTypeOf(buffer)), 
                      LyTypeToString(LyTypeOf(next)));
            buffer = next;
        }
    }
}

//
// Error message macros:
// Expected usage:
//      raise(<errorMacro>(<args>));
//      return lyExecError;
// eg:
//      raise(typeError("LYI_ADD", buffer, value));
//      return lyExecError;

// assignementError (char *instrTypeStr, void *value)
#define assignmentError(instrTypeStr, value) LYL_SYNTAX_ERROR, \
    "AssignmentError: %s cannot assign to %s", instrTypeStr, LyTypeToString(LyTypeOf(value))
// unexpectedEolError (char *instrTypeStr)
#define unexpectedEolError(instrTypeStr) LYL_SYNTAX_ERROR, \
    "UnexpectedToken: %s expecting statement or value; found LYI_ENDLINE", instrTypeStr
// undefinedNameError (char *nameStr)
#define undefinedNameError(nameStr) LYL_EXEC_ERROR, \
    "NameError: %s is not defined.", nameStr
// typeError (char *instrString, void *val1, void *val2)
#define typeError(instrString, val1, val2) LYL_SYNTAX_ERROR, \
    "TypeError: %s does not support %s with %s", \
    LyTypeToString(LyTypeOf(val1)), instrString, LyTypeToString(LyTypeOf(val2)) 

LyExecPacket _LyExec_set (void *buffer, LyTable *ns, LyQueue *queue) 
{
    // $buffer: previous value  $ns: active namespace
    // $queue: remaining instruction queue 
    // (excludes all instructions before and including this operation (LYI_SET))
    // eg. "x = 12 + 1"
    // buffer = LyName(x); queue = LyQueue(LyNum(12), LyInstr(LYI_SET), LyNum(1))

    // Valid syntax: [LyName] = [LyName | LyNumber | LyString | LyBlock | LyTable | LyFunc]
    
    void *value;
    LyLog(LYL_EXEC_DEBUG, "LyExec op: LYI_SET");
    
    if (LyTypeOf(buffer) != LY_T_NAME) {
        //raise(LYL_SYNTAX_ERROR, "LYI_SET: cannot assign to %s", LyTypeToString(LyTypeOf(buffer)));
        raise(assignmentError("LYI_SET", buffer));
        return execError;
    }
    
        // Forward responsibility for parsing the rest of queue (to the next endline)
        // to LyExec(). Store this value in $value.
    value = LyExec(ns, queue, LYI_ENDLINE);
    LyLog(LYL_EXEC_DEBUG, "LYI_SET value:");
    LyLog_indent();
    LyDebug(value);
    LyLog_dedent();
    if (value == NULL) {
        //raise(LYL_SYNTAX_ERROR, "LYI_SET: expecting statement or value; found LYI_ENDLINE");
        raise(unexpectedEolError("LYI_SET"));
        return execError;
    }

        // If $value is a block, execute its queue, set $value to the result, and continue
    if (LyTypeOf(value) == LY_T_GROUP) {
        value = execBlock(value);
        if (value == NULL) {
            //raise (LYL_SYNTAX_ERROR, "LYI_SET: expecting statement or value; found LYI_ENDLINE");
            raise(unexpectedEolError("LYI_SET"));
            return execError;
        }
    }
    LyLog(LYL_EXEC_DEBUG, "Setting item");
        // Set $value (LyValue) into $ns (Lytable) at $buffer (LyName)
    LyTable_setItem(ns, buffer, value);
    return execPacket(NULL);
}

LyExecPacket _LyExec_add (void *buffer, LyTable *ns, LyQueue *queue) 
{
    // $buffer: previous value  $ns: active namespace
    // $queue: remaining instruction queue 
    // (excludes all instructions before and including this operation (LYI_SET))
    // eg. "x + 1 - y"
    // buffer = LyName(x); queue = LyQueue(LyNum(1), LyInstr(LYI_SUB), LyName("y"))
    void *value = LyExec(ns, queue, LYI_ENDLINE);
    LyQueue_prepend(queue, LyInstr_new(LYI_ENDLINE));
    
    if (value == NULL) {
        //raise(LYL_SYNTAX_ERROR, "LYI_ADD: expecting statement or value; found LYI_ENDLINE");
        raise(unexpectedEolError("LYI_ADD"));
        return execError;
    }
    
    if (buffer == NULL) {
        if (LyTypeOf(value) == LY_T_NUMBER)
            buffer = LyNumber_new(0);
        else if (LyTypeOf(value) == LY_T_STRING)
            buffer = LyString_new("");
    }
    
    if (LyTypeOf(buffer) == LY_T_NAME) {
        LyString *name = buffer;
        buffer = getVar(ns, buffer);
        if (buffer == NULL) {
                //raise(LYL_EXEC_ERROR, "Name %s is not defined.", name->string);
            raise(undefinedNameError(name->string));
            return execError;
        }
    }
    
    if (LyTypeOf(buffer) == LY_T_STRING) {
        if (LyTypeOf(value) == LY_T_STRING) { // Concat strings:
            // Make a clone of buffer: otherwise, "str1 + str2" will directly modify
            // str1 as well as return str1 + str2 (unexpected behavior)
            buffer = LyClone(buffer);
            LyString_append(buffer, value);
        } else if (LyTypeOf(value) == LY_T_NUMBER) { // Insert a number onto the end of a string:
            LyNumber *num = value;
            char numStr[128];
            sprintf(numStr, "%f", num->val);
            buffer = LyClone(buffer);
            LyString_append(buffer, LyString_new(numStr));
        } else {
            raise(typeError("LYI_ADD", buffer, value));
            return execError;
        }
    } else if (LyTypeOf(buffer) == LY_T_NUMBER) {
        if (LyTypeOf(value) == LY_T_STRING) {
            LyString *str = value;
            float n;
            sscanf(str->string, "%f", &n);
            value = LyNumber_new(n);
        }
        if (LyTypeOf(value) == LY_T_NUMBER) { // add two numbers together
            // Make a clone of buffer: the addition operator should return
            // the sum of two numbers, NOT directly modify one of them
            LyNumber *n1, *n2;
            buffer = LyClone(buffer);
            n1 = buffer; n2 = value;
            n1->val += n2->val;
        } else {
            raise(typeError("LYI_ADD", buffer, value));
            return execError;
        }
    } else {
        raise(typeError("LYI_ADD", buffer, value));
        return execError;
    }
    return execPacket(buffer);
}

LyExecPacket _LyExec_sub (void *buffer, LyTable *ns, LyQueue *queue) 
{
    void *value = LyExec(ns, queue, LYI_ENDLINE);
    LyQueue_prepend(queue, LyInstr_new(LYI_ENDLINE));
    
    if (value == NULL) {
        raise(unexpectedEolError("LYI_SUB"));
        return execError;
    }
    
    if (buffer == NULL) {
        if (LyTypeOf(value) == LY_T_NUMBER)
            buffer = LyNumber_new(0);
        else if (LyTypeOf(value) == LY_T_STRING) {
            buffer = LyString_new("");
        }
    }
    
    if (LyTypeOf(buffer) == LY_T_NAME) {
        LyString *name = buffer;
        buffer = getVar(ns, buffer);
        if (buffer == NULL) {
                //raise(LYL_EXEC_ERROR, "Name %s is not defined.", name->string);
            raise(undefinedNameError(name->string));
            return execError;
        }
    }
    
    if (LyTypeOf(buffer) == LY_T_NUMBER) {
        if (LyTypeOf(value) == LY_T_STRING) {
            LyString *str = value;
            float n;
            sscanf(str->string, "%f", &n);
            value = LyNumber_new(n);
        }
        if (LyTypeOf(value) == LY_T_NUMBER) {
          // Make a clone of buffer: the addition operator should return
          // the sum of two numbers, NOT directly modify one of them
            LyNumber *n1, *n2;
            buffer = LyClone(buffer);
            n1 = buffer; n2 = value;
            n1->val -= n2->val;
        } else {
            raise(typeError("LYI_SUB", buffer, value));
            return execError;
        }
    } else if (LyTypeOf(buffer) == LY_T_STRING) { // Append a string in reverse :P
        if (LyTypeOf(value) == LY_T_NUMBER) {
            LyNumber *num = value;
            char numStr[128];
            sprintf(numStr, "%f", num->val);
            value = LyString_new(numStr);
        }
        if (LyTypeOf(value) == LY_T_STRING) {
            buffer = LyClone(buffer);
            LyString *str = value;
            LyString *reversedStr = LyString_new(str->string);
            for (int i = 0; i < str->len; i++) {
                reversedStr->string[i] = str->string[str->len - i - 1];
            }
            LyString_append(buffer, reversedStr);
            LyMem_release(reversedStr);
        }
    }else {
        raise(typeError("LYI_SUB", buffer, value));
        return execError;
    }
    return execPacket(buffer);
}

LyExecPacket _LyExec_mul (void *buffer, LyTable *ns, LyQueue *queue) 
{
    void *value = LyExec(ns, queue, LYI_ENDLINE);
    LyQueue_prepend(queue, LYI_ENDLINE);
    
    if (value == NULL) {
        raise(unexpectedEolError("LYI_MUL"));
        return execError;
    }
    
    if (buffer == NULL) {
        raise(LYL_SYNTAX_ERROR, "SyntaxError: expecting left hand value");
        return execError;
    }
    
    if (LyTypeOf(buffer) == LY_T_NAME) {
        LyString *name = buffer;
        buffer = getVar(ns, buffer);
        if (buffer == NULL) {
                //raise(LYL_EXEC_ERROR, "Name %s is not defined.", name->string);
            raise(undefinedNameError(name->string));
            return execError;
        }
    }
    
    if (LyTypeOf(buffer) == LY_T_STRING) {
        if (LyTypeOf(value) == LY_T_NUMBER) {
            LyNumber *num = value;
            void *original = buffer;
            buffer = LyClone(buffer);
            int i = (int)num->val;
            if (i < 0)
                i *= -1;
            for (; i > 0; i--) {
                LyString_append(buffer, original);
            }
        } else {
            raise(typeError("LYI_MUL", buffer, value));
            return execError;
        }
    } else if (LyTypeOf(buffer) == LY_T_NUMBER) {
        if (LyTypeOf(value) == LY_T_STRING) {
            LyString *str = value;
            float n;
            sscanf(str->string, "%f", &n);
            value = LyNumber_new(n);
        }
        if (LyTypeOf(value) == LY_T_NUMBER) { 
            LyNumber *n1, *n2;
            buffer = LyClone(buffer);
            n1 = buffer; n2 = value;
            n1->val *= n2->val;
        } else {
            raise(typeError("LYI_MUL", buffer, value));
            return execError;
        }
    } else {
        raise(typeError("LYI_MUL", buffer, value));
        return execError;
    }
    return execPacket(buffer);
}

LyExecPacket _LyExec_div (void *buffer, LyTable *ns, LyQueue *queue) 
{
    void *value = LyExec(ns, queue, LYI_ENDLINE);
    LyQueue_prepend(queue, LYI_ENDLINE);
    
    if (value == NULL) {
        raise(unexpectedEolError("LYI_DIV"));
        return execError;
    }
    
    if (buffer == NULL) {
        raise(LYL_SYNTAX_ERROR, "SyntaxError: expecting left hand value");
        return execError;
    }
    
    if (LyTypeOf(buffer) == LY_T_NAME) {
        LyString *name = buffer;
        buffer = getVar(ns, buffer);
        if (buffer == NULL) {
                //raise(LYL_EXEC_ERROR, "Name %s is not defined.", name->string);
            raise(undefinedNameError(name->string));
            return execError;
        }
    }
    
    if (LyTypeOf(buffer) == LY_T_NUMBER) {
        if (LyTypeOf(value) == LY_T_STRING) {
            LyString *str = value;
            float n;
            sscanf(str->string, "%f", &n);
            value = LyNumber_new(n);
        }
        if (LyTypeOf(value) == LY_T_NUMBER) { 
            LyNumber *n1, *n2;
            buffer = LyClone(buffer);
            n1 = buffer; n2 = value;
            n1->val /= n2->val;
        } else {
            raise(typeError("LYI_DIV", buffer, value));
            return execError;
        }
    } else {
        raise(typeError("LYI_DIV", buffer, value));
        return execError;
    }
    return execPacket(buffer);
}



// Clean up: remove local macros
#undef raise
#undef execBlock
#undef getVar

#undef handleCase
#undef execPacket
#undef execError

#undef assignmentError
#undef unexpectedEolError
#undef undefinedNameError
#undef typeError

/*
void * _LyExec (LyTable *ns, LyQueue *queue, LyInstrType endInstr) {
    void *buffer = NULL;
    LyClass *bufferMeta = buffer;
    void *next;
    LyClass *nextMeta;
    while (1) {
        next = LyQueue_pop(queue);
        nextMeta = next;
        if (next == NULL)
            return buffer;
        if (nextMeta->type == LY_T_INSTR) {
            LyInstr *instr = next;
            if (instr->type == endInstr) {
                return buffer;
            }
                
            if (buffer == NULL) {
                if (instr->type == LYI_SUB) {}
                else {
                    LyLog(LYL_EXEC_ERROR, "%s cannot operate on NULL", LyInstr_toString(instr));
                    continue;
                }
            }
            switch(instr->type) {
case LYI_SET: 
    if (bufType != LY_T_NAME)
        LyLog(LYL_EXEC_ERROR, "LYI_SET cannot set to %s", bufTypeAsString);
    else {
        void *value = LyExec(ns, queue, LYI_ENDLINE);
        LyQueue_prepend(queue, LyInstr_new(LYI_ENDLINE));
        LY_EXEC_CHECK_VAL
        LyClass *valueMeta = value;
        if (valType == LY_T_NAME) {
            value = LyTable_getItem(ns, value);
            valueMeta = value;
            LY_EXEC_CHECK_VAL
            if (valType == LY_T_BLOCK) {
                LyTable_setItem(ns, buffer, execBlock(value) *//*LyExec(ns, value, LYI_ENDQUEUE)*//*);
            } else
                LyTable_setItem(ns, buffer, value);
        } else
            LyTable_setItem(ns, buffer, value);
        buffer = NULL;
    }break;
case LYI_ADD: 
if (1) {
    void *value = LyExec(ns, queue, LYI_ENDLINE); 
    LyQueue_prepend(queue, LyInstr_new(LYI_ENDLINE));
    LY_EXEC_CHECK_VAL
    LyClass *valueMeta = value;
    LY_EXEC_GET_BUFFER_FROM_TABLE
    if (bufType == LY_T_STRING) {
        if (valType == LY_T_STRING) {
            buffer = LyClone(buffer);
            LyString_append(buffer, value);
        } else
            LyLog(LYL_EXEC_ERROR, "Cannot add %s to LyString", valTypeAsString);
    } else if (bufType == LY_T_NUMBER) {
        if (valType == LY_T_NUMBER) {
            LyNumber *num = buffer; LyNumber *num2 = value;
            num2->val += num->val;
            buffer = num2;
        } else
            LyLog(LYL_EXEC_ERROR, "Cannot add %s to LY_T_NUMBER", valTypeAsString);
    } else
        LyLog(LYL_EXEC_ERROR, "Cannot add %s to %s.", valTypeAsString, bufTypeAsString);
}   break;
case LYI_SUB:
if (1) {
    void *value = LyExec(ns, queue, LYI_ENDLINE);
    LyQueue_prepend(queue, LyInstr_new(LYI_ENDLINE));
    LY_EXEC_CHECK_VAL
    LyClass *valueMeta = value;
    if (buffer == NULL) {
        if (valType == LY_T_NUMBER) {
            LyNumber *num = value;
            num->val *= -1;
            buffer = num;
        } else {
            LyLog(LYL_EXEC_ERROR, "LyInstr (LYI_SUB) cannot act upon NULL and %s", LyTypeToString(LyTypeOf(value)));
        }
    } else {
        LY_EXEC_GET_BUFFER_FROM_TABLE
        if (bufType == LY_T_NUMBER) {
            if (valType == LY_T_NUMBER) {
                LyNumber *num = LyClone(buffer); LyNumber *num2 = value;
                num->val -= num2->val;
                buffer = num;
            } else
                LyLog(LYL_EXEC_ERROR, "Cannot subtract %s from LY_T_NUMBER", valTypeAsString);
        }
        else 
            LyLog(LYL_EXEC_ERROR, "Cannot subtract %s from %s", valTypeAsString, bufTypeAsString);
    }
}   break;
//unfinished
            }
        } else { //if nextMeta->type != LY_T_INSTR:
            if (buffer != NULL)
                LyLog(LYL_EXEC_WARN, "Overwriting buffer (%s) with %s", bufTypeAsString, nextTypeAsString);
            buffer = next;
            bufferMeta = buffer;
        }
    }    
}
*/
/*
 void * LyExec (LyTable *ns, LyQueue *queue, LyInstrType endInstr) {
 void * retVal = _LyExec(ns, queue, endInstr);
 LyClass *retMeta = retVal;
 if (retVal == NULL)
 return retVal;
 if (retMeta->type == LY_T_NAME) {
 retVal = LyTable_getItem(ns, retVal);
 if (retVal == NULL)
 return NULL;
 retMeta = retVal;
 if (retMeta->type == LY_T_BLOCK) {
 return execBlock(retVal);//LyExec(ns, retVal, LYI_ENDQUEUE);
 }
 }
 return retVal;
 }*/
 /*
void LyCheckSyntax (LyQueue *queue) {
    void *object, *next, *prev;
    LyQueueItem *item = queue->first;
    if (queue->first == NULL)
        return;
    object = item->value;
    next = item->next->value;
    while (item != NULL) {
        object = item->value;
        if (item->next != NULL)
            next = item->next->value;
        else
            next = NULL;
        if (item->prev != NULL)
            prev = item->prev->value;
        else
            prev = NULL;
        
        if (LyTypeOf(object) == LY_T_INSTR) {
            LyInstr *instr = object;
            LyClassType prevType = LyTypeOf(prevType);
            LyClassType nextType = LyTypeOf(nextType);
            switch(instr->type) {
                case LYI_SET:
                    if (prevType != LY_T_NAME) {
                        raise(LYE_TYPE_ERROR, "
                    }
                
                
                
            }
        }
        
        
        
        
        item = item->next;
    }
    
}*/
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

