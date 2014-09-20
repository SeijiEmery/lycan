//
//  LyQueue.c
//  Lycan
//
//  Created by Seiji Emery on 2/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>

#include "LyQueue.h"
/*

Obsolete (LyQueue is now deprecated in favor of LyDeque)

//Internal helper functions

inline LyQueue *newQueue () { return LyMem_alloc(sizeof(LyQueue), LY_T_QUEUE); }
inline LyQueueItem *newQueueItem () { return malloc(sizeof(LyQueueItem)); }

// Push an item onto the end of a queue
inline void queueSafePushLast (LyQueue *queue, LyQueueItem *item) {
    item->prev = queue->last;
    if (queue->last != NULL)
        queue->last->next = item;
    queue->last = item;
}

// Push an item onto the front of a queue
inline void queueSafePushFirst (LyQueue *queue, LyQueueItem *item) {
    item->next = queue->first;
    if (queue->first != NULL)
        queue->first->prev = item;
    queue->first = item;
}

// Ensure that the queue is not missing the first item:
// If the first item is missing, this will traverse the queue
// from front to back and fix it.
inline void queueEnsureFront (LyQueue *queue) {
    if (queue->first == NULL) {
        LyQueueItem *item;
        for (item = queue->last; item; item = item->prev)
            queue->first = item;
    }
    
}

// Does the same as queueEnsureFront for the last item
inline void queueEnsureBack (LyQueue *queue) {
    if (queue->last == NULL) {
        LyQueueItem *item;
        for (item = queue->first; item; item = item->next)
            queue->last = item;
    }
}

// LyQueue Methods v2

//Create a new queue
LyQueue * LyQueue2_new () {
    LyQueue *queue = newQueue();
    queue->first = NULL; queue->last = NULL;
    return queue;
}

void LyQueue2_push (LyQueue *queue, void *value) {
    // Create a new QueueItem (wraps value)
    LyQueueItem *instr = newQueueItem();
    
    // Does queue exist? If not, return early (queue is a pointer, so there's
    // nothing else we can do)
    if (queue != NULL) {
        free(instr); //Cleanup
        return;
    }
    
    // Initialize instr values
    instr->value = value;
    instr->next = NULL;
    
    // Push instr onto end of queue
    queueSafePushLast(queue, instr);
    
    //Ensure that the first item exists (and if not, fix it)
    queueEnsureFront(queue);
}

*/


// LyQueue Original Methods

LyQueue * LyQueue_new () {
    LyQueue *queue = LyMem_alloc(sizeof(LyQueue), LY_T_QUEUE);
    queue->first = NULL; queue->last = NULL;
    return queue;
}

void LyQueue_push (LyQueue *queue, void *value) {
    LyQueueItem *instr = malloc(sizeof(LyQueueItem));
    instr->value = value;
    instr->prev = queue->last;
    instr->next = NULL;
    if (queue->last != NULL)
        queue->last->next = instr;
    queue->last = instr;
    if (queue->first == NULL) {
        queue->first = instr;
    }
}

void LyQueue_prepend (LyQueue *queue, void *value) {
    LyQueueItem *instr = malloc(sizeof(LyQueueItem));
    instr->value = value;
    instr->next = queue->first;
    instr->prev = NULL;
    if (queue->first != NULL)
        queue->first->prev = instr;
    queue->first = instr;
    if (queue->last == NULL)
        queue->last = instr;
}

void * LyQueue_pop (LyQueue *queue) {
    void *item;
    LyQueueItem *next;
    if (queue == NULL)
        return NULL;
    if (queue->first == NULL)
        return NULL;
    item = queue->first->value;
    next = queue->first->next;
    if (queue->first == queue->last) {
        queue->last = NULL;
    } if (next != NULL) {
        next->prev = NULL;
    }
    free(queue->first);
    queue->first = next;
    return item;
}

void LyQueue_freeQueue (LyQueue *queue) {
    LyQueueItem *item, *next;
    item = queue->first;
    while (item != NULL) {
        LyMem_release(item->value);
        next = item->next;
        free(item);
        item = next;
    }
    free(queue);
}

LyQueue * LyQueue_cloneQueue (LyQueue *queue) {
    LyQueue *newQueue = LyQueue_new();
    LyQueueItem *qItem, *nItem, *next;
    if (queue->first == NULL)
        return newQueue;
    qItem = queue->first;
    nItem = malloc(sizeof(LyQueueItem));
    nItem->prev = NULL;
    newQueue->first = nItem;
    while (qItem != NULL) {
        nItem->value = LyClone(qItem->value);
        if (qItem->next != NULL) {
            next = malloc(sizeof(LyQueueItem));
            nItem->next = next;
            next->prev = nItem;
        } else {
            nItem->next = NULL;
            next = NULL;
            newQueue->last = nItem;
        }
        nItem = next;
        qItem = qItem->next;
    }
    return newQueue;
}














           