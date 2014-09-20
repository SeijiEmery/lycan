//
//  LyMemory.c
//  Lycan
//
//  Created by Seiji Emery on 2/6/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "LyLog.h"
#include "LyTable.h"
#include "LyQueue.h"
#include "LyDeque.h"
#include "LyNumber.h"
#include "LyInstr.h"

#include "LyCore.h"


void Lycan_init () {
    LyLog_init();
    LyLog(LYL_CORE, "initializing...");
}
void Lycan_cleanup () {
    LyLog(LYL_CORE, "cleaning up - end of process.");
    LyLog_cleanup();
}

void * LyMem_alloc (size_t size, LyClassType type/*, const LyMethodTable *polyMethods*/) {
    LyClass *meta;
    void *object = malloc(size);
    if (object == NULL)
        LyLog(LYL_MEM_ERROR, "Memory could not be allocated.");
    meta = object;
    meta->refCount = 1;
    meta->type = type;
   // meta->polyMethods = polyMethods;
    LyLog(LYL_MEM_DEBUG, "Memory allocated: %p", object);
    return object;
}

void * LyMem_alloc2 (size_t size, LyClassType type, const LyMethodTable *polyMethods) {
    LyClass *meta;
    void *object = malloc(size);
    if (object == NULL)
        LyLog(LYL_MEM_ERROR, "Memory could not be allocated.");
    meta = object;
    meta->refCount = 1;
    meta->type = type;
    meta->polyMethods;
    LyLog(LYL_MEM_DEBUG, "Memory allocated: %p", object);
    return object;
}

void * LyMem_retain (void *object) {
    LyClass *meta = object;
    meta->refCount ++;
    LyLog(LYL_MEM_DEBUG, "Memory retained: %p", object);
    return object;
}

void * LyMem_release (void *object) {
    if (object == NULL) {
        LyLog(LYL_MEM_WARN, "Cannot release a null pointer.");
        return NULL;
    }
    LyClass *meta = object;
    LyLog(LYL_MEM_DEBUG, "Releasing Memory: %p", object);
    meta->refCount --;
    if (meta->refCount == 0) {
        LyLog(LYL_MEM_DEBUG, "Memory freed: %p", object);
        if (meta->type == LY_T_QUEUE) {
            LyQueue_freeQueue(object);
        } else if (meta->type == LY_T_TABLE) {
            LyTable_freeTable(object);
        } else {
            free(object);
        }
        meta = NULL;
        
        return NULL;
    } else if (meta->refCount < 0) {
        LyLog(LYL_MEM_WARN, "Memory has already been freed: %p", object);
        return NULL;
    } else {
        LyLog(LYL_MEM_DEBUG, "Memory released: %p", object);
    }
    return object;
}

void * LyClone (void *object) {
    LyClass *meta = object;
    if (object == NULL)
        return NULL;
    switch(meta->type) {
        case LY_T_NUMBER: {
            LyNumber *num = object;
            return LyNumber_new(num->val);
        }
        case LY_T_STRING: {
            LyString *str = object;
            return LyString_new(str->string);
        }
        case LY_T_NAME: {
            LyString *name = object;
            LyString *new = LyString_new(name->string);
            new->meta.type = LY_T_NAME;
            return new;
        }
        case LY_T_INSTR: {
            LyInstr *instr = object;
            return LyInstr_new(instr->type);
        }
        case LY_T_TABLE:
            return LyTable_cloneTable(object);
        case LY_T_QUEUE:
            return LyQueue_cloneQueue(object);
        /*case LY_T_DEQUE: // unimplemented
            return LyDeque_deepCopy(object);*/
            //return LyDeque_quickCopy(object);
        case LY_T_GROUP:
        case LY_T_BLOCK: {
            LyQueue *block = LyQueue_cloneQueue(object);
            LyLog(LYL_CORE_DEBUG, "Block Clone:\n\tOriginal:\n");
            LyDebug(object);
            LyLog(LYL_CORE_DEBUG, "\n\tClone:\n");
            block->meta.type = meta->type;
            LyDebug(block);
            return block;
        }
        case LY_T_NULL:
            return object;
        default:
            LyLogError(LYL_CORE_ERROR, "LyClone failed - unsupported type: %s", LyTypeToString(meta->type));
    }
    return NULL;
}


void LyPrint (void *object) {
    LyClass *meta = object;
    if (object == NULL)
        return;
    switch(meta->type) {
        case LY_T_NUMBER:
        if (1) {
            LyNumber *num = object;
            printf("%f\n", num->val);
        }break;
        case LY_T_STRING:
        if (1) {
            LyString *string = object;
            printf("\"%s\"\n", string->string);
        }break;
        case LY_T_NAME:
        if (1) {
            LyString *name = object;
            printf("%s\n", name->string);
        }break;
        case LY_T_TABLE:
            if (1) {
                LyTable *table = object;
                LyTableItem *item = table->firstItem;
                printf("{\n");
                while (item != NULL) {
                    printf("\t%s = ", item->name->string);
                    LyPrint(item->data);
                    item = item->next;
                }
                printf("}\n");
            }break;
        case LY_T_INSTR:
            printf("<LyInstr %s>\n", LyInstr_toString(object));
            break;
        case LY_T_BLOCK:
            printf("<LyBlock>\n");
            break;
        case LY_T_QUEUE:
            printf("<LyInstructionQueue>\n");
            break;
        default:
            printf("<Unknown (%d)>", meta->type);
    }
}

LyClassType LyTypeOf (void *object) {
    LyClass *meta = object;
    if (object == NULL)
        return LY_T_NULL;
    return meta->type;
}

char * LyTypeToString (LyClassType type) {
    switch (type) {
        case LY_T_QUEUE:
        return "LyQueue";
        case LY_T_BLOCK:
        return "LyBlock";
        case LY_T_NAME:
        return "LyName";
        case LY_T_STRING:
        return "LyString";
        case LY_T_NUMBER:
        return "LyNumber";
        case LY_T_TABLE:
        return "LyTable";
        case LY_T_INSTR:
        return "LyInstr";
        case LY_T_GROUP:
        return "LyGroup";
        case LY_T_NULL:
        return "Null";
        case LY_T_FUNC:
        return "LyFunc";
    }
    return "Unknown type";
}

void LyDebug (void *object) {
    LyClass *meta = object;
    if (object == NULL)
        return;
    switch (meta->type) {
        case LY_T_QUEUE:
            LyLog(LYL_CORE_DEBUG, "LyQueue: ");
            LyLog_indent();
            if (1) {
                LyQueue *queue = object;
                LyQueueItem *item = queue->first;
                while (item != NULL) {
                    LyDebug(item->value);
                    item = item->next;
                } LyLog_dedent();
            } break;
        case LY_T_BLOCK:
            LyLog(LYL_CORE_DEBUG, "LyBlock: ");
            LyLog_indent();
            if (1) {
                LyQueue *queue = object;
                LyQueueItem *item = queue->first;
                while (item != NULL) {
                    LyDebug(item->value);
                    item = item->next;
                } LyLog_dedent();
            } break;
        case LY_T_GROUP:
            LyLog(LYL_CORE_DEBUG, "LyGroup: ");
            LyLog_indent();
            if (1) {
                LyQueue *queue = object;
                LyQueueItem *item = queue->first;
                while (item != NULL) {
                    LyDebug(item->value);
                    item = item->next;
                } LyLog_dedent();
            } break;
        case LY_T_TABLE:
            LyLog(LYL_CORE_DEBUG, "LyTable: ");
            LyLog_indent();
            if (1) {
                LyTable *table = object;
                LyTableItem *item = table->firstItem;
                while (item != NULL) {
                    LyDebug(item->name);
                    LyLog_indent();
                    LyDebug(item->data);
                    LyLog_dedent();
                    item = item->next;
                } LyLog_dedent();
            } break;
        case LY_T_STRING:
            if (1) {
                LyString *string = object;
                LyLog(LYL_CORE_DEBUG, "LyString: %s", string->string);
            } break;
        case LY_T_NAME:
            if (1) {
                LyString *name = object;
                LyLog(LYL_CORE_DEBUG, "LyName: %s", name->string);
            } break;
        case LY_T_NUMBER:
            if (1) {
                LyNumber *num = object;
                LyLog(LYL_CORE_DEBUG, "LyNumber: %f", num->val);
            } break;
        case LY_T_INSTR:
            LyLog(LYL_CORE_DEBUG, "LyInstr: %s", LyInstr_toString(object));
            break;
        default:
            LyLog(LYL_CORE_DEBUG, "Unknown object type: %d", meta->type);
    }
    
}






