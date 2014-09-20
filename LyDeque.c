//
//  LyDeque.c
//  Lycan
//
//  Created by Seiji Emery on 3/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>

#include "LyCore.h"
#include "LyLog.h"

#include "LyDeque.h"

/*
 * Internal helper functions
 */
 
// Note: these names were intentionally chosen to avoid any possible
// name collisions; macros map these names to simpler ones *in this file
// only* (It never hurts to be safe...)


inline char * LyDequeInternal_typeAsString (void *self) { return LyDeque_typeAsString(self); }
inline char * LyDequeInternal_toString (void *self) { return LyDeque_toString(self); }
inline void LyDequeInternal_print (void *self) { LyDeque_print(self); }
inline void LyDequeInternal_debug (void *self) { LyDeque_debug(self); }
inline void * LyDequeInternal_deepCopy (void *self) { return LyDeque_deepCopy(self); }
inline void * LyDequeInternal_quickCopy (void *self) { return LyDeque_quickCopy(self); }

const LyMethodTable LyDequeMethods = {
    &LyDequeInternal_typeAsString,
    &LyDequeInternal_toString,
    &LyDequeInternal_print,
    &LyDequeInternal_debug,
    &LyDequeInternal_deepCopy,
    &LyDequeInternal_quickCopy
};

 
inline LyDeque *LyInternal_Deque_new () { 
    return LyMem_alloc2(sizeof(LyDeque), LY_T_DEQUE, &LyDequeMethods); 
}
#define newDeque() LyInternal_Deque_new()


inline LyDequeItem *LyInternal_DequeItem_new () { 
    return malloc(sizeof(LyDequeItem));
}
#define newItem() LyInternal_DequeItem_new()

// Push an item onto the end of a queue
inline void LyInternal_Deque_safePushLast 
(LyDeque *deque, LyDequeItem *item) {
    item->prev = deque->last;
    if (deque->last)
        deque->last->next = item;
    deque->last = item;
}
#define safePushLast(deque, item) LyInternal_Deque_safePushLast(deque, item)

// Push an item onto the front of a queue
inline void LyInternal_Deque_safePushFirst 
(LyDeque *deque, LyDequeItem *item) {
    item->next = deque->first;
    if (deque->first)
        deque->last->next = item;
    deque->last = item;
}
#define safePushFirst(deque, item) LyInternal_Deque_safePushFirst(deque, item)

// Ensure that the queue is not missing the first item:
// If the first item is missing, this will traverse the queue
// from front to back and fix it.
inline void LyInternal_Deque_ensureFirst (LyDeque *deque) {
    if (deque->first == NULL) {
        LyDequeItem *item;
        for (item = deque->last; item; item = item->prev)
            deque->first = item;
    }
}
#define ensureFirst(deque) LyInternal_Deque_ensureFirst(deque)

// Does the same as queueEnsureFront for the last item
inline void LyInternal_Deque_ensureLast (LyDeque *deque) {
    if (deque->last == NULL) {
        LyDequeItem *item;
        for (item = deque->first; item; item = item->next)
            deque->last = item;
    }
}
#define ensureLast(deque) LyInternal_Deque_ensureLast(deque)



/*
 * Deque Methods
 */

LyDeque * LyDeque_new () {
    LyDeque *deque = newDeque();
    deque->first = NULL; deque->last = NULL;
    return deque;
}

void LyDeque_pushLast (LyDeque *deque, void *value) {
    // deque must exist to perform any operations on it.
    // (can't make a new deque and push to that, since deque 
    // is given as a pointer (and this function has no return))
    if (deque) {
        // Create a new DequeItem (wraps value)
        LyDequeItem *item = newItem();
        // Initialize item values
        item->value = value;
        item->next = NULL;
        // Push item onto the end of queue
        safePushLast(deque, item);
        // Ensure deque consistency (first is not null)
        ensureFirst(deque);
    }
}

void LyDeque_pushFirst (LyDeque *deque, void *value) {
    if (deque != NULL) {
        LyDequeItem *item = newItem();
        
        item->value = value;
        item->next = NULL;
        
        safePushFirst(deque, item);
        ensureLast(deque);
    }
}

void * LyDeque_popFirst (LyDeque *deque) {
    if (deque == NULL) // There is no item to return if the deque doesn't
        return NULL;   // exist.
        
    if (deque->first == NULL) {
        // If both the first and last items are null, then the deque is 
        // empty, and contains no value that can be returned.
        if (deque->last == NULL) 
            return NULL;
        // If only the first item is null, then the deque is inconsistent
        // and this can be fixed:
        ensureFirst(deque); 
        // (Traversing from deque->last, this assigns the 'first' obtainable
        // value in the deque to deque->first).
        // The rest of our code will now work.
    }
    
    if (deque->first == deque->last) {
        // If the first and last items are the same, then this deque
        // contains only one item; remove that and return its value
        // (thus clearing the deque)
        void *item = deque->first->value; // store our value
        free(deque->first);               // free the container
        deque->first = NULL; deque->last = NULL;
        return item;
    } else { // Normal:
        // Remove the first item and return its value
        void *item = deque->first->value; // store our value
        LyDequeItem *next = deque->first->next; // store the next node
        free(deque->first); // free the first node
        deque->first = next; // and replace it with next
        return item;
    }
}

void * LyDeque_popLast (LyDeque *deque) {
    if (deque == NULL)
        return NULL;
        
    if (deque->last == NULL) {
        if (deque->first == NULL)
            return NULL;
        ensureLast(deque);
    }
    if (deque->last == deque->first) {
        void *item = deque->last->value;
        free(deque->last);
        deque->last = NULL; deque->first = NULL;
        return item;
        
    } else { // normal:
        void *item = deque->last->value;
        LyDequeItem *prev = deque->last->prev;
        free(deque->last);
        deque->last = prev;
        return item;
    }
}

// Free/release all data associated with the deque, and the deque
// itself.
// Called by LyMem_release() when ref count is less than 1.
// This acts like free(), so don't call it manually (use LyMem_release()
// instead).
void LyDeque_free (LyDeque *deque) {
    if (deque == NULL) // Don't try to do anything if deque is NULL
        return;
    if (deque->meta.refCount > 0) // safety mechanism 
        return; // (to avoid this first manually zero out the ref count)
        
    LyDequeItem *item, *next;    
    for (item = deque->first; item != NULL; item = next) { 
        // Iterate through all the items in the deque:
        next = item->next; // store the next node
        LyMem_release(item->value); // release the stored value
        free(item);                 // free the container
    }
    free(deque);
}

#define deque_iter(iteritem, first, next) \
for ((iteritem) = (first); (iteritem) != NULL; (iteritem) = (next))

LyDeque * LyDeque_tst_deepCopy (LyDeque *deque) {
    LyDeque *dequeClone = newDeque();
    LyDequeItem *node, *copyNode, *prevCopy;
    dequeClone->first = NULL;
    deque_iter(node, deque->first, node->next) {
        copyNode = newItem();
        if (!dequeClone->first)
            dequeClone->first = copyNode;
        copyNode->value = LyClone(node->value);
        copyNode->prev = prevCopy;
        if (prevCopy)
            prevCopy->next = copyNode;
        prevCopy = copyNode;
    }
    copyNode->next = NULL;
    dequeClone->last = copyNode;
    return dequeClone;
}

// Make a deepcopy of the deque.
LyDeque * LyDeque_deepCopy (LyDeque *deque) {
    LyDeque *dequeClone = newDeque();
    LyDequeItem *node, *copyNode, *prevCopy;
    
    dequeClone->first = NULL;
    
    for (node = deque->first; node != NULL; node = node->next) {
        // Iterate through the deque, making a complete copy of all nodes:
    
        copyNode = newItem(); // Construct a new node
        
        if (!dequeClone->first) // Run only on the first iteration:
            dequeClone->first = copyNode; 
            
        // Copy the node's value into the copy node container
        copyNode->value = LyClone(node->value);
        
        // Set the node links between copyNode and prevCopy
        copyNode->prev = prevCopy;
        if (prevCopy)
            prevCopy->next = copyNode;
        // Prepare for the next iteration step
        prevCopy = copyNode;
    }
    // At the end of iteration, copyNode is the last node.
    copyNode->next = NULL;
    dequeClone->last = copyNode;
    
    return dequeClone;
}

// Make a shallow copy of the deque (shares the same references (ref count
// is incrimented for each value) as the original object)
// Note: this is not necessarily that much faster than deepCopy if the 
// deque contains simple object values (this *will* be faster when LyClone
// would be called recursively - ie. if the deque contains references to
// other deques and tables).
LyDeque * LyDeque_quickCopy (LyDeque *deque) {
    LyDeque *dequeClone = newDeque();
    LyDequeItem *node, *copyNode, *prevCopy;
    dequeClone->first = NULL;
    
    for (node = deque->first; node != NULL; node = node->next) {
        copyNode = newItem();
        if (!dequeClone->first)
            dequeClone->first = copyNode;
        
        // Copy the reference to the value and retain()
        // instead of cloning it:
        copyNode->value = node->value;
        LyMem_retain(node->value);
        
        copyNode->prev = prevCopy;
        if (prevCopy)
            prevCopy->next = copyNode;
        prevCopy = copyNode;
    }
    copyNode->next = NULL;
    dequeClone->last = copyNode;
    return dequeClone;
}

void LyDeque_print (LyDeque *deque) {
    bool printComma = false;
    printf("LyDeque( ");
    for (LyDequeItem *item = deque->first; item; item = item->next) {
        if (printComma)
            printf(", ");
        LyPrint(item);
        printComma = true;
    }
    printf(")");
}
char * LyDeque_toString (LyDeque *deque);
inline char * LyDeque_typeAsString (LyDeque *deque) {
    return "LyDeque";
}


void LyDeque_debug (LyDeque *deque) {
    LyLog(LYL_CORE_DEBUG, "LyDeque: ");
    LyLog_indent();
    for (LyDequeItem *item = deque->first; item; item = item->next)
        LyDebug(item->value);
    LyLog_dedent();
}




