//
//  LyStack.c
//  Lycan
//
//  Created by Seiji Emery on 2/24/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "LyStack.h"

LyStack * LyStack_new() {
    LyStack *stack = LyMem_alloc(sizeof(LyStack), LY_T_STACK);
    stack->first = NULL;
    stack->last = NULL;
    return stack;
}

void LyStack_free(LyStack *stack) {
    LyStackNode *item, *next;
    if (stack == NULL)
        return;
    item = stack->first;
    while (item->next != NULL) {
        next = item->next;
        LyMem_release(item->data);
        free(item);
        item = next;
    }
    free(item);
    free(stack);
}

LyStack *LyStack_clone (LyStack *stack) {
    LyStack *clone;
    LyStack *node = LyStack_new();
    if (stack == NULL)
        return NULL;
    stack = LyStack_first(stack);
    while (stack != NULL) {
        node->data = stack->data;
        node->prev = clone;
        if (clone != NULL)
            clone->next = node;
        stack = stack->next;
        clone = node;
        node = LyStack_new();
    }
    LyMem_release(node);
    return clone;
}

LyStack *LyStack_push (LyStack *stack, void *data) {
    LyStack *newNode = LyStack_new();
    stack = LyStack_first(stack);
    newNode->data = data;
    newNode->next = stack;
    stack->prev = newNode;
    return newNode;
}

LyStack *LyStack_append (LyStack *stack, void *data) {
    LyStack *newNode = LyStack_new();
    stack = LyStack_last(stack);
    newNode->data = data;
    newNode->prev = stack;
    stack->next = newNode;
    return newNode;
}
void *LyStack_pop (LyStack *stack) {
    
}
void *LyStack_rpop (LyStack *stack);

LyStack *LyStack_next (LyStack *item);
LyStack *LyStack_prev (LyStack *item);
LyStack *LyStack_first (LyStack *stack);
LyStack *LyStack_last (LyStack *stack);

LyStack *LyStack_insert (void *data);
LyStack *LyStack_remove (LyStack *item);


