//
//  LyStack.h
//  Lycan
//
//  Created by Seiji Emery on 2/24/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _LY_INSTR_STACK_H_
#define _LY_INSTR_STACK_H_

#include "LyCore.h"
#include "SuperList.h"

typedef struct LyStackContainer {
    LyClass meta;
    struct LyStackNode *first, *last;
}LyStack;

typedef struct LyStackNode {
    struct LyStackNode *prev, *next;
    void *data;
}LyStackNode;

LyStack * LyStack_new();
void LyStack_free(LyStack *stack);
LyStack *LyStack_clone (LyStack *stack);

LyStack *LyStack_push (LyStack *stack, void *data);
LyStack *LyStack_append (LyStack *stack, void *data);
void *LyStack_pop (LyStack *stack);
void *LyStack_rpop (LyStack *stack);

LyStack *LyStack_next (LyStack *item);
LyStack *LyStack_prev (LyStack *item);
LyStack *LyStack_first (LyStack *stack);
LyStack *LyStack_last (LyStack *stack);

LyStack *LyStack_insert (void *data);
LyStack *LyStack_remove (LyStack *item);

#endif