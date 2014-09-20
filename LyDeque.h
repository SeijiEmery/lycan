//
//  LyDeque.h
//  Lycan
//
//  Created by Seiji Emery on 3/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Lycan_LyDeque_h
#define Lycan_LyDeque_h

#include "LyCore.h"

typedef struct LyDeque LyDeque;
typedef struct LyDequeItem LyDequeItem;


struct LyDeque {
    LyClass meta;
    LyDequeItem *first, *last;
};

struct LyDequeItem {
    void *value;
    LyDequeItem *prev, *next;
};

LyDeque *LyDeque_new ();

void LyDeque_pushFront (LyDeque *deque, void *item);
void LyDeque_pushBack (LyDeque *deque, void *item);

void * LyDeque_popFront (LyDeque *deque);
void * LyDeque_popBack (LyDeque *deque);

void LyDeque_free (LyDeque *deque);
LyDeque *LyDeque_deepCopy (LyDeque *deque);
LyDeque *LyDeque_quickCopy (LyDeque *deque);

inline void LyDeque_print (LyDeque *deque);
char * LyDeque_toString (LyDeque *deque);
inline char * LyDeque_typeAsString (LyDeque *deque);
inline void LyDeque_debug (LyDeque *deque);

#endif
