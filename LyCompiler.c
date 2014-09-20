//
//  LyCompiler.c
//  Lycan
//
//  Created by Seiji Emery on 2/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "LyCore.h"
#include "LyString.h"
#include "LyQueue.h"
#include "LyInstr.h"
#include "LyNumber.h"
#include "LyLog.h"

#include "LyCompiler.h"

// Helper functions:
void appendNumber (LyQueue *queue, char *buffer, int *n) {
    LyLog(LYL_COMPILE_DEBUG, "appending number %s", buffer);
    buffer[(*n)+1] = '\0';
    LyQueue_push(queue, LyNumber_new(atof(buffer)));
}

void appendName (LyQueue *queue, char *buffer, int *n) {
    LyLog(LYL_COMPILE_DEBUG, "appending name %s", buffer);
    buffer[(*n)+1] = '\0';
    LyString *name = LyString_new(buffer);
    name->meta.type = LY_T_NAME;
    LyQueue_push(queue, name);
} 

void resetState (char *buffer, int *n, bool *isName, bool *isNumber) {
    *n = 0;
    safeFree(buffer);
    buffer = malloc(sizeof(char) * 512);
    *isName = false;
    *isNumber = false;
}


#define MifMatch(str) if (strMatch(input, str)) { i += MstrLen(str)


// Main routine: compile a string into a LyInstructionQueue, which can then be fed to LyExec()
LyQueue * LyCompile (char *input) {
    LyQueue *queue = LyQueue_new();
    char *buffer = malloc(sizeof(char) * 512);
    int n = 0;
    bool isName = false;
    bool isNumber = false;
    
    void *next;
    LyLog(LYL_COMPILE_DEBUG, "Compiling \"%s\"", input);
    for (int i=0; input[i] != '\0'; i++) { // loop over every char in input, and handle accordingly
        /*
         Structure:
         Check sequentially for comments, keywords, strings, etc.
         -Comments, strings, and blocks are handled via iterative sub-loops
         -Keywords are handled on a case by case basis
         These are all temp. stored in $next.
         Other characters (names and numbers) are pushed onto $buffer after every other special
         case has been handled. If a special token (say, a space, addition operator, left paren, etc.) is found
         following a name or number, it will be handled and temp. set in $next. The name/token handler (the 
         'else' block near the end) will not be invoked. Instead, the contents of $buffer (if buffer is not empty)
         are parsed and pushed onto $queue, and then reset. Finally, $next is pushed onto $queue, maintaining proper
         precedence: 
         "myNum=10" -> LyQueue (LyName("x"), LyInstr(LY_SET), LyNumber("10"))
            1. myNum is sequentially pushed onto $buffer (marked as 'LyName')
            2. '=' is encountered => LyInstr(LY_SET) is stored in $next
            3. the non-name/number routine is invoked:
                $buffer ("myNum") is pushed onto $queue as a LyName
                $buffer is reset
                $next (LyInstr(LY_SET)) is pushed onto $queue
            4. 10 is sequentially pushed onto $buffer (marked as 'LyNumber')
            5. EOL is reached - cleanup:
                $buffer ("10") is pushed onto $queue as a LyNumber
                $buffer is safeFreed; $queue is returned
        */
        LyLog(LYL_COMPILE_DEBUG, "Parsing: %c", input[i]);
        // Handle single line comment
        if (input[i] == '/' && input[i+1] == '/') { // "//"
            for (; input[i] != '\0'; i++) { // enter a sub loop; ignore every character until a '\n' is found
                if (input[i] == '\n') {
                    break;
                }
            }
        }
        // Handle multiple line comment
        else if (input[i] == '/' && input[i+1] == '*') { // "/*"
            int indent = 1; // indent tracks the number of /* and */ pairs there are.
            i += 2;
            for (; input[i] != '\0'; i++) { // enter a sub loop: 
                if (input[i] == '/' && input[i+1] == '*') { //Sub-multi comment: incriment index
                    indent++; i++; 
                } else if (input[i] == '*' && input[i+1] == '/') { //End of multi-line comment: decrement index
                    indent--; i++; 
                } if (indent < 1) { // end the sub loop when all comment index reaches 0
                    break;
                }
            }
        }
        //Handle string
        else if (input[i] == '"') { // '\"'
            int start = i + 1; // Track the start of the string
            while(input[i] != '\0') { // Loop over all items until a matching '"' is found
                if (input[++i] == '"') {
                    break;
                }
            } // Push the string contents to the instruction queue:
            next = LyString_newFromSlice(input, start, i);
        }
        
        //Handle block
        else if (input[i] == '{') {
            int indent = 1;
            int start = ++i;
            LyString *sub;
            LyQueue *block;
            while (input[i++] != '\0') {
                if (input[i] == '}') {
                    indent --;
                } else if (input[i] == '{') {
                    indent ++;
                } if (indent < 1) {
                    break;
                }
            }
            sub = LyString_newFromSlice(input, start, i);
            block = LyCompile(sub->string);
            LyMem_release(sub);
            block->meta.type = LY_T_BLOCK;
            next = block;
        }
        
        //Handle group
        else if (input[i] == '(') {
            int indent = 1;
            int start = ++i;
            LyString *sub;
            LyQueue *group;
            while (input[i++] != '\0') {
                if (input[i] == ')') {
                    indent --;
                } else if (input[i] == '(') {
                    indent ++;
                } if (indent < 1) {
                    break;
                }
            }
            sub = LyString_newFromSlice(input, start, i);
            LyLog(LYL_COMPILE_DEBUG, sub->string);
            group = LyCompile(sub->string);
            LyMem_release(sub);
            group->meta.type = LY_T_GROUP;
            next = group;
        }
        
        //Handle endline
        else if (input[i] == ';' || input[i] == '\n') {
            LyLog(LYL_COMPILE_DEBUG, "adding endline");
            next = LyInstr_new(LYI_ENDLINE);
        }
            
        //Handle space
        else if (input[i] == ' ' || input[i] == '\t') {
            continue; // (ignore whitespace)
        }
        //Handle keywords & symbols
        else if (input[i] == '=') {
            if (input[i+1] == '=') {
                next = LyInstr_new(LYI_EQ); i++;
            } else {
                next = LyInstr_new(LYI_SET);
                LyLog(LYL_COMPILE_DEBUG, "adding set");
            }
        } else if (input[i] == '+') {
            next = LyInstr_new(LYI_ADD);
        } else if (input[i] == '-') {
            next = LyInstr_new(LYI_SUB);
        } else if (input[i] == '*') {
            if (input[i+1] == '*') {
                next = LyInstr_new(LYI_POW); i++;
            } else 
                next = LyInstr_new(LYI_MUL);
        } else if (input[i] == '/') {
            next = LyInstr_new(LYI_DIV);
        } else if (input[i] == '^') {
            next = LyInstr_new(LYI_POW);
        }
        
        //Unfinished
        else {
            //Character is not a keyword or symbol.
            // Could be a name, or number
            if (n == 0) {
                //Handle name
                if ((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z') || input[i] == '_') {
                    buffer[n] = input[i]; n++; isName = true;
                    LyLog(LYL_COMPILE_DEBUG, "starting name");
                } else if ((input[i] >= '0' && input[i] <= '9')) {
                    buffer[n] = input[i]; n++; isNumber = true;
                    LyLog(LYL_COMPILE_DEBUG, "starting number");
                } else if (input[i] == '`') { // Allows numeric names: for any name that starts with '`', the '`' is ignored
                    isName = true;            // but the rest (even if numeric) is treated as a name.
                                              // `12 = 10 -> LyName("12"), LyInstr(LYI_SET), LyNumber(10)
                    LyLog(LYL_COMPILE_DEBUG, "starting name");
                }else {
                    LyLog(LYL_COMPILE_WARN, "Unexpected character: %s", input[i]);
                }
            } else {
                if (isName) {
                    LyLog(LYL_COMPILE_DEBUG, "continuing name");
                    if ((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z') 
                    || input[i] == '_' || (input[i] >= '0' && input[i] <= '9')) {
                        buffer[n] = input[i]; n++;
                    } else if (input[i] == '.') {
                        appendName(queue, buffer, &n);
                        resetState(buffer, &n, &isName, &isNumber);
                        LyQueue_push(queue, LyInstr_new(LYI_SCOPE));
                        isName = true;
                        /*
                        LyString *name = LyString_newFromSlice(buffer, 0, n);
                        name->meta.type = LY_T_NAME;
                        LyQueue_push(queue, LyInstr_new(LYI_SCOPE));
                        LyQueue_push(queue, name);
                        isName = false; isNumber = false; n = 0;
                        safeFree(buffer); buffer = malloc(sizeof(char) * 512);*/
                    } else {
                        LyLog(LYL_COMPILE_WARN, "Unexpected character in name %s: %c", buffer, input[i]);
                        appendName(queue, buffer, &n);
                        resetState(buffer, &n, &isName, &isNumber);
                    }
                } else if (isNumber) {
                    LyLog(LYL_COMPILE_DEBUG, "continuing number");
                    if ((input[i] >= '0' && input[i] <= '9') || input[i] == '.' || input[i] == 'x' || input[i] == 'i') {
                        buffer[n] = input[i]; n++;
                    } else {
                        LyLog(LYL_COMPILE_WARN, "Unexpected character in number %s: %c", buffer, input[i]);
                        appendNumber(queue, buffer, &n);
                        resetState(buffer, &n, &isName, &isNumber);
                    }
                }
            }
            continue; //Prevent execution of the following code
        }
        // Else loop was not entered (character was a keyword or symbol)
        // In that case, first add the name/number buffer to the queue (if existant), followed
        // by 'next' (the next instruction). Then reset the name/number buffer.
        if (n > 0) {
            if (isName) {
                
                appendName(queue, buffer, &n);/*
                LyString *name = LyString_newFromSlice(buffer, 0, n+1);
                name->meta.type = LY_T_NAME;
                LyQueue_push(queue, name);*/
            } else if (isNumber) {
                
                appendNumber(queue, buffer, &n); /*
                LyQueue_push(queue, LyNumber_new(atof(buffer)));*/
            }
            resetState(buffer, &n, &isName, &isNumber); /*
            isName = false;
            isNumber = false;
            n = 0;
            safeFree(buffer);
            buffer = malloc(sizeof(char) * 512);*/
        }
        LyLog(LYL_COMPILE_DEBUG, "Appending next");
        LyQueue_push(queue, next);
        next = NULL;
    }
    //CLEANUP:
    if (n > 0) {
        if (isName) {
            appendName(queue, buffer, &n);
        } else if (isNumber) {
            appendNumber(queue, buffer, &n);
        }
        resetState(buffer, &n, &isName, &isNumber);
    }
    LyLog(LYL_COMPILE_DEBUG, "appending next");
    LyQueue_push(queue, LyInstr_new(LYI_ENDQUEUE));
    safeFree(buffer);
    return queue;
}
