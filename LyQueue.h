//
//  LyQueue.h
//  Lycan
//
//  Created by Seiji Emery on 2/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _LY_INSTR_QUEUE_H_
#define _LY_INSTR_QUEUE_H_

#include "LyCore.h"

typedef struct LyInstrQueue {
    LyClass meta;
    struct LyQueueItem *first, *last;
}LyQueue;

typedef struct LyQueueItem {
    void *value;
    struct LyQueueItem *prev, *next;
}LyQueueItem;

LyQueue * LyQueue_new ();

void LyQueue_push (LyQueue *queue, void *item);
void LyQueue_prepend (LyQueue *queue, void *value);

void * LyQueue_pop (LyQueue *queue);

void LyQueue_freeQueue (LyQueue *queue);
LyQueue * LyQueue_cloneQueue (LyQueue *queue);

#endif