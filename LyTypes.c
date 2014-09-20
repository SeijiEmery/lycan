//
//  LyTypes.c
//  Lycan
//
//  Created by Seiji Emery on 2/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>

#include "LyCore.h"
#include "LyString.h"
#include "LyTypes.h"

/*
OLD - UNUSED
*/


// ===========
// == LyMem ==
// ===========

/* LyMemory Management: ->README<-
 Value types (LyNumber, LyString, LyName) - stored within other objects as void *
 will likely be referenced in multiple places; as such, default C memory management
 of malloc/free is insufficient, since prematurely freeing an object may cause an
 error somewhere, and not freeing it at all will cause memory leaks.
 To solve this, a simple reference counting system is used: LyMem_alloc calls
 malloc and sets the reference count to 1; subsequent calls to LyMem_retain and 
 LyMem_release increment/decrement this counter; when the counter drops to 0 the
 object is freed.
 
 This reference counting system is very basic, and unfortunately, it requires direct
 coordination between the memory management calls, object structure, constructors,
 deconstructors, and any method that either adds a reference to the object, or removes
 or otherwise modifies such a reference. As such, the following rules must be enforced
 in order for the memory management to work properly:
 0. The following memory management rules apply only to VALUE types (LyString, LyName,
 LyNumber).
 1. Object's struct definition must have 'LyMemRef refCount' as the FIRST item. This is
 used to implement struct inheiritance from LyMemRef.
 2. Object constructors need to use LyMem_alloc instead of malloc.
 3. All calls to free need to be replaced with LyMem_release.
 4. LyMem_retain needs to be called whenever a new reference is made to an object; 
 LyMem_release must be called whenever a reference is removed.
 5. Local one-use variables must be released when done - LyMem_alloc must be matched
 with a call to LyMem_release, plus another call for every time LyMem_retain is called.
 (This should be obvious - the same is true of malloc/free)
 6. Don't free instead of LyMem_release - ever. Unless you really need to, and you know
 what you're doing.
*/
/*
void * LyMem_alloc (size_t size) {
    LyMemRef *refCount;
    void *obj;
    obj = malloc(size);
    refCount = obj;
    refCount->count = 1;
    return obj;
}

int LyMem_retain (void *data) {
    LyMemRef *refCount = data;
    refCount->count ++;
    return refCount->count;
}

int LyMem_release (void *data) {
    LyMemRef *refCount = data;
    int count;
    refCount->count --;
    count = refCount->count;
    if (count < 1) {
        free(data);
    }
    return count;
}
*/

// ==============
// == LyNumber ==
// ==============
// complete
/*
LyNumber * LyNumber_new (LyType type, double val) {
    LyNumber *num = LyMem_alloc(sizeof(LyNumber), TLyNumber);
    num->type = type;
    num->val = val;
    return num;
}

double dpow (double val, int pow) {
    if (pow == 0)
        return 1;
    if (pow > 0) {
        double v = val * dpow(val, pow-1);
        return v;
        //return val * dpow(val, pow-1);
    } else
        return dpow(val, pow+1) / val;
}

// Parse a LyNumber out of a string; type is automatically inferred (either int (long) or float (double)).
// Supports decimal float notation, and hexadecimal notation. Decimal units default to floats unless a 'i'
// suffix is attached.
// Examples: "1023.892"; "1039"; "1230i"; "0x0FAB23"
// WARNING: Hexadecimal support is dysfunctional, and has been disabled
// Todo: fix hexadecimal support
LyNumber * LyNumber_newFromString (char *string) {
    LyNumber *num = LyMem_alloc(sizeof(LyNumber), TLyNumber);
    int i = 0;
    char isHex = string[0] == '0' && string[1] == 'x';
    int decPos = -1;
    char isInt = 0;
    int len;
    double val = 0;
    
    //Pre-parse
    while (string[i] != 0) {
        if ((string[i] < '0' || string[i] > '9') && string[i] != 'i' && string[i] != '.') {
            if (isHex) {
                if ((string[i] < 'a' || string[i] > 'f') && (string[i] < 'A' || string[i] > 'F') && string[i] != 'x') {
                    printf("Parsing Error: Unexpected non hexadecimal character '%c' found in numeric literal.\n", string[i]);
                }
            } else {
                printf("Parsing Error: Unexpected non decimal character '%c' found in numeric literal.\n", string[i]);
            }
        }
        if (string[i] == '.') {
            if (decPos > 0)
                printf("Parsing Error: multiple decimal places found in numeric literal.\n");
            else
                decPos = i;
        }
        i++;
    }
    isInt = string[i-1] == 'i'; // is the last value 'i'?
    if (isInt)
        len = i - 1;
    else
        len = i;
    if (decPos < 0)
        decPos = len;
    
    // Hexadecimal not working; disabled hex (quick fix)
    isHex = 0;
    
    //Assemble number
    i = 0;
    if (isHex) {
        for (int c = 0; c < len; c++) {
            double v = string[c];
            if (v > 'a' && v < 'z')
                v -= (double)'a' + 10;
            else if (v > 'A' && v < 'B')
                v -= (double)'A' + 10;
            else
                v -= (double)'0';
            if (c == decPos) {
            } else if (c > decPos)
                val += v * dpow(16, decPos - c);
            else
                val += v * dpow(16, decPos - c - 1);
        }
    } else {
        for (int c = 0; c < len; c++) {
            if (c == decPos) {
                //Do nothing
            } else if (c > decPos) {
                int pow = decPos - c;
                double incr = dpow(10, pow);
                double mval = string[c] - '0';
                incr *= mval;
                val += incr;
               // val += ((double)string[c] - '0') * dpow(10, decPos - i - 1);
            } else {
                int pow = decPos - c - 1;
                double incr = dpow(10, pow);
                double mval = string[c] - '0';
                incr *= mval;
                val += incr;
                //val += ((double)string[c] - '0') * dpow(10, decPos - i);
            }
        }
    }
    num->val = val;
    if (isInt || (isHex && decPos < len))
        num->type = LY_INT;
    else
        num->type = LY_FLOAT;
    return num;
}

LyNumber * LyNumber_newFromStringSlice (char *string, int start, int end) {
    LyString *subStr = LyString_newFromSlice(string, start, end);
    LyNumber *num = LyNumber_newFromString(subStr->string);
    LyMem_release(subStr);
    return num;
}

// ==============
// == LyString ==
// ==============
// complete
/*
LyString * LyString_new (char *string) {
    int i = 0;
    LyString *str = LyMem_alloc(sizeof(LyString));
    str->string = string;
    while (string[i] != 0) {
        i++;
    }
    str->len = i;
    return str;
}

LyString * LyString_newWithLen (char *string, int len) {
    LyString *str = LyMem_alloc(sizeof(LyString));
    str->string = string;
    str->len = len;
    return str;
}
LyString * LyString_newFromSlice (char *string, int start, int end) {
    LyString *str = LyMem_alloc(sizeof(LyString));
    if (end - start > 0) {
        str->len = end - start + 1;
        str->string = calloc(sizeof(char), (size_t)str->len);
        for (int i = 0; i < str->len; i++) {
            str->string[i] = string[start + i];
        }
    } else { 
        str->string = NULL;
        str->len = 0;
    }
    return str;
}*/

// ============
// == LyName ==
// ============
// complete
/*
LyName  * LyName_new (char *name) {
    LyName *lyName = LyMem_alloc(sizeof(LyName), TLyName);
    lyName->name = name;
    //lyName->nsId = 0;
    //lyName->namespace = NULL;
    return lyName;
}
/*
LyName  * LyName_newWithNsInfo (char *name, int nsId, LyNamespace *namespace) {
    LyName *lyName = malloc(sizeof(LyName));
    lyName->name = name;
    lyName->nsId = nsId;
    lyName->namespace = namespace;
    return lyName;
}*//*
LyName * LyName_newFromSlice (char *string, int start, int end) {
    LyString *subStr = LyString_newFromSlice(string, start, end);
    LyName *lyName = LyName_new(subStr->string);
    LyMem_release(subStr);
    return lyName;
}

// =============
// == LyToken ==
// =============
// complete; deprecated


LyToken * LyToken_new (LyType type, void *data) {
    LyToken *token = malloc(sizeof(LyToken));
    token->type = type;
    token->data = data;
    token->next = NULL;
    token->prev = NULL;
    return token;
}

LyToken * LyToken_append (LyToken *stack, LyToken *token) {
    if (stack == NULL) {
        stack = token;
    } else {
        LyToken *tmp;
        stack->prev = token;
        token->next = stack;
        tmp = stack;
        stack = token;
        token = tmp;
        /*token->next = stack->next;
        if (stack->next != NULL)
            stack->next->prev = token;
        token->prev = stack;
        stack->next = token;*//*
    }
    return token;
}
void LyToken_removeFromStack (LyToken *token) {
    token->next->prev = token->prev;
    token->prev->next = token->next;
    token->next = NULL;
    token->prev = NULL;
}
void LyToken_removeFromStackAndFree (LyToken *token) {
    LyToken_removeFromStack(token);
    free(token);
}

// frees all items below the stack element (next elements; not prev)
void LyToken_freeStack (LyToken *stack) {
    LyToken *next;
    while (stack != NULL) {
        next = stack->next;
        free(stack);
        stack = next;
    }
}
// Frees all linked elements in the stack
void LyToken_freeEntireStack (LyToken *stack) {
    while (stack->prev != NULL) {
        stack = stack->prev;
    }
    LyToken_freeStack(stack);
}

// =============
// == LyValue ==
// =============
// deprecated

LyValue * LyValue_new (LyType type, void *data) {
    LyValue *val = malloc(sizeof(LyValue));
    val->type = type;
    val->data = data;
    return val;
}

// =============
// == LyInstr ==
// =============
// partial

LyInstr * LyInstr_new (LyType type, void *data) {
    LyInstr * instr = malloc(sizeof(LyInstr));
    instr->type = type;
    instr->data = data;
    instr->next = NULL;
    instr->prev = NULL;
    return instr;
}

LyInstr * LyInstr_newWithNeighbors (LyType type, void *data, LyInstr *prev, LyInstr *next) {
    LyInstr *instr = malloc(sizeof(LyInstr));
    instr->type = type;
    instr->data = data;
    instr->prev = prev;
    instr->next = next;
    return instr;
}


// ============
// == LyLine ==
// ============
// partial

LyLine * LyLine_new (LyThread *parent) {
    LyLine *line = malloc(sizeof(LyLine));
    line->parent = parent;
    line->instr = NULL;
    return line;
}

void LyLine_freeLineAndFreeValues (LyLine *line) {
    while (line->instr->prev != NULL) {
        line->instr = line->instr->prev;
    }
    while (line->instr != NULL) {
        free(line->instr->data);
        line->instr = line->instr->next;
        free(line->instr->prev);
    }
    free(line);
}

void LyLine_freeLineAndRetainValues (LyLine *line) {
    while (line->instr->prev != NULL) {
        line->instr = line->instr->prev;
    }
    while (line->instr != NULL) {
        
    }
}

// ==============
// == LyThread ==
// ==============
// TODO

LyThread * LyThread_newWithParent (LyThread *parent) {
    LyThread * thread = malloc(sizeof(LyThread));
    thread->parent = parent;
    thread->line = NULL;
    return thread;
}
LyThread * LyThread_new () {
    return LyThread_newWithParent(NULL);
}

void LyThread_appendLine (LyThread *thread, LyLine *line) {
    while (thread->line->next != NULL) {
        thread->line = thread->line->next;
    }
    thread->line->next = line;
    line->prev = thread->line;
    thread->line = line;
}

void LyThread_append (LyThread *thread, LyInstr *instr) {
    if (instr->type == LY_NULL)
        return;
    if (thread->line == NULL) {
        if (instr->type == LY_ENDLINE)
            return;
        thread->line = LyLine_new(thread);
        thread->line->instr = instr;
    } else {
        if (instr->type == LY_ENDLINE) {
            LyThread_appendLine(thread, LyLine_new(thread));
        } else if (thread->line->instr == NULL) {
            thread->line->instr = instr;
        } else {
            while (thread->line->instr->next != NULL) {
                thread->line->instr = thread->line->instr->next;
            }
            thread->line->instr->next = instr;
            instr->prev = thread->line->instr;
            thread->line->instr = instr;
        }
    }
}

void LyThread_removeItem (LyThread *thread, LyInstr *instr) {
    if (thread->line->instr == instr) {
        if (instr->next != NULL) {
            thread->line->instr = instr->next;
        } else {
            thread->line->instr = instr->prev;
        }
    }
    instr->prev->next = instr->next;
    instr->next->prev = instr->prev;
    instr->next = NULL;
    instr->prev = NULL;
}

void LyThread_removeAndFreeItem (LyThread *thread, LyInstr *instr) {
    LyThread_removeItem(thread, instr);
    free(instr);
}
/*
void LyThread_freeThreadAndFreeValues (LyThread *thread) {
    LyThread_gotoFirstLine(thread);
    while (thread->line != NULL) {
        LyLine_freeLine 
    }
} *//*

void LyThread_freeThreadAndRetainValues (LyThread *thread) {
    LyThread_gotoFirstLine(thread);
    while (thread->line != NULL) {
        LyLine_freeLineAndRetainValues(thread->line);
    }
    free(thread);
}

//Iteration:
LyInstr * LyThread_gotoLastItem (LyThread *thread) {
    while (thread->line->instr->next != NULL) {
        thread->line->instr = thread->line->instr->next;
    }
    return thread->line->instr;
}

LyInstr * LyThread_gotoFirstItem (LyThread *thread) {
    while (thread->line->instr->prev != NULL) {
        thread->line->instr = thread->line->instr->prev;
    }
    return thread->line->instr;
}

LyInstr * LyThread_iterForward (LyThread *thread) {
    if (thread->line->instr != NULL)
        thread->line->instr = thread->line->instr->prev;
    return thread->line->instr;
}

LyInstr * LyThread_iterBack (LyThread *thread) {
    if (thread->line->instr != NULL)
        thread->line->instr = thread->line->instr->next;
    return thread->line->instr;
}

LyLine * LyThread_gotoPrevLine (LyThread *thread) {
    if (thread->line != NULL)
        thread->line = thread->line->prev;
    return thread->line;
}

LyLine * LyThread_gotoNextLine (LyThread *thread) {
    if (thread->line != NULL)
        thread->line = thread->line->next;
    return thread->line;
}

LyLine * LyThread_gotoFirstLine (LyThread *thread) {
    while (thread->line->prev != NULL)
        thread->line = thread->line->prev;
    return thread->line;
}

LyLine * LyThread_gotoLastLine (LyThread *thread) {
    while (thread->line->next != NULL)
        thread->line = thread->line->next;
    return thread->line;
}

//== LyListItem ==
// TODO

// == LyList ==
// TODO

// == LyMapItem ==
// TODO

// == LyMap ==
// TODO



/*
void LyNamespace_set (LyNamespace *ns, LyVariable *var) {
    LyNsNode *node, newNode;
    node = ns->root;
    char *c = var->name->name;
    var->name->namespace = ns;
    var->name->nsId = 0;
    while (*c != 0) {
        var->name->nsId += *c;
        c++;
    }
    newNode->var = var;
    if (ns->root == NULL) {
        ns->root = newNode;
        return;
    }
    while (1) {
        if (var->name->nsId < node->var->name->nsId) {
            if (node->left == NULL) {
                node->left = &newNode;
                return;
            } else
                node = node->left;
        } else {
            if (node->right == NULL) {
                node->right = &newNode;
                return;
            } else
                node = node->right;
        }
    }
}

LyVariable * LyNamespace_find (LyNamespace *ns, char *name) {
    LyNsNode *node = ns->root;
    char *c = name;
    int nsId = 0;
    while (*c != 0) {
        nsId += *c;
        c++;
    }
    while (node != NULL) {
        if (node->var->name->nsId == nsId) {
            if (strcmp(node->var->name->name, name)) {
                return node->var;
            }
        }
        if (node->var->name->nsId < nsId) {
            node = node->left;
        } else 
            node = node->right;
    }
    if (ns->parent != NULL) {
        return LyNamespace_find(ns->parent, name);
    }
    return NULL;
}

void LyNamespace_setValueByName (LyNamespace *ns, char *name, LyValue *value) {
    LyVariable var;
    LyName name_;
    name_.name = name;
    var.value = value;
    var.name = &name_;
    LyNamespace_set(ns, &var);
}

LyValue * LyNamespace_getValueByName (LyNamespace *ns, char *name) {
    return LyNamespace_find(ns, name)->value;
}*/








