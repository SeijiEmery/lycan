//
//  LyMemory.h
//  Lycan
//
//  Created by Seiji Emery on 2/6/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _LY_CORE_H_
#define _LY_CORE_H_

#include <stdlib.h>

typedef char bool;
#ifndef true
    #define true 1
#endif
#ifndef false
    #define false 0
#endif

#define safeFree(ptr) if (ptr != NULL) free(ptr); ptr = NULL


typedef enum LyClassType {
    LY_T_NULL,
    LY_T_STRING,
    LY_T_NAME,
    LY_T_NUMBER,
    LY_T_DEQUE,
    LY_T_QUEUE, //deprecated
    LY_T_TABLE,
    LY_T_BLOCK,
    LY_T_GROUP,
    LY_T_FUNC,
    LY_T_INSTR
}LyClassType;

LyClassType LyTypeOf (void *object);
char * LyTypeToString (LyClassType type);
void LyDebug (void *object);

typedef struct LyClass LyClass;
typedef struct LyMethodTable LyMethodTable;


struct LyClass {
    int refCount;
    LyClassType type;
    LyMethodTable *polyMethods;
};

struct LyMethodTable {
    char * (*type) (void *self);
    char * (*toString) (void *self);
    void (*print) (void *self);
    void (*debug) (void *self);
    void * (*deepCopy) (void *self);
    void * (*shallowCopy) (void *self);
};

void Lycan_init ();
void Lycan_cleanup();

void * LyMem_alloc (size_t size, LyClassType type);
void * LyMem_alloc2 (size_t size, LyClassType type, const LyMethodTable *polyMethods);
void * LyMem_retain (void *object);
void * LyMem_release (void *object);

#define retain (obj) LyMem_retain(obj)
#define release (obj) LyMem_release(obj)

void LyDebug (void *object);
void LyPrint (void *object);
void * LyClone (void *object);
#endif