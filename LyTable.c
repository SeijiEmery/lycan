//
//  LyTable.c
//  Lycan
//
//  Created by Seiji Emery on 2/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>

#include "LyLog.h"
#include "LyTable.h"

LyTable * LyTable_new () {
    LyTable *table = LyMem_alloc(sizeof(LyTable), LY_T_TABLE);
    table->firstItem = NULL;
    table->parent = NULL;
    return table;
}

#if 1
#define iter(item, first, next) \
for ((item) = (first); (item) != NULL; (item) = (next))
#endif
#if 0
#define iter(item, first) \
for ((item) = (first); (item) != NULL /*&& (item) != (item)->prev*/; (item) = (item)->next)
#endif
void LyTable_tst_freeTable (LyTable *table) {
    LyTableItem *item, *next;
    iter(item, table->firstItem, item->next) {
        LyMem_release(item->name);
        LyMem_release(item->data);
        next = item->next;
        free(item);
    }
    free(table);
}

LyTableItem * newTableItem (LyString *name, void *data) {
    LyTableItem *item = malloc(sizeof(LyTableItem));
    item->name = name;
    item->data = data;
    item->next = NULL;
    item->prev = NULL;
    return item;
}

void releaseTableItemMembers (LyTableItem *item) {
    LyMem_release(item->name);
    LyMem_release(item->data);
}

LyTable * LyTable_tst_cloneTable (LyTable *table) {
    LyTable *tableClone = LyTable_new();
    LyTableItem *item, *copy, *prevCopy;
    tableClone->firstItem = NULL;
    iter(item, table->firstItem, item->next) {
        copy = newTableItem(LyClone(item->name), LyClone(item->data));
        if (!tableClone->firstItem)
            tableClone->firstItem = copy;
        copy->prev = prevCopy;
        if (prevCopy)
            prevCopy->next = copy;
        prevCopy = copy;
    }
    copy->next = NULL;
    //tableClone->lastItem = copy; // no lastItem?
    return tableClone;
}

void LyTable_tst_setItem (LyTable *table, LyString *name, void *data) {
    LyTableItem *item;
    LyMem_retain(name);
    LyMem_retain(data);
    if (table->firstItem == NULL) {
        table->firstItem = newTableItem(name, data);
        return;
    }
    iter(item, table->firstItem, item->next) {
        if (LyString_eq(name, item->name)) {
            releaseTableItemMembers(item);
            item->data = data;
            item->name = name;
            return;
        }
    }
    LyLog(LYL_TABLE_DEBUG, "LyTable_setItem: Item does not exist in LyTable - creating item.");
    
    { // new block to define a new variable
        LyTableItem *newItem = newTableItem(name, data);
        newItem->prev = item;
        item->next = newItem;
    }
}

void * LyTable_tst_getItem (LyTable *table, LyString *name) {
    LyTableItem *item;
    iter(item, table->firstItem, item->next)
        if (LyString_eq(name, item->name))
            return item->data;
    if (table->parent)
        return LyTable_tst_getItem(table->parent, name);
    return NULL;
}

bool LyTable_tst_removeItem (LyTable *table, LyString *name) {
    LyTableItem *item;
    iter(item, table->firstItem, item->next) {
        if (LyString_eq(name, item->name)) {
            releaseTableItemMembers(item);
            item->next->prev = item->prev;
            item->prev->next = item->next;
            free(item);
            return true;
        }
    }
    if (table->parent)
        return LyTable_tst_removeItem(table->parent, name);
    return false;
}


void LyTable_freeTable (LyTable *table) {
    LyTableItem *item, *next;
    item = table->firstItem;
    while (item != NULL) {
        LyMem_release(item->name);
        LyMem_release(item->data);
        next = item->next;
        free(item);
        item = next;
    }
    free(table);
}

LyTable * LyTable_cloneTable (LyTable *table) {
    LyTable *newTable = LyTable_new();
    LyTableItem *tItem, *nItem, *nNext;
    tItem = table->firstItem;
    if (tItem == NULL)
        return newTable;
    nItem = malloc(sizeof(LyTableItem));
    nItem->prev = NULL;
    newTable->firstItem = nItem;
    while (tItem != NULL) {
        nNext = malloc(sizeof(LyTableItem));
        nItem->name = LyClone(tItem->name);
        nItem->data = LyClone(tItem->data);
        if (tItem->next != NULL) {
            nItem->next = nNext;
            nNext->prev = nItem;
        } else
            nItem->next = NULL;
        tItem = tItem->next;
        nItem = nNext;
    }
    return newTable;
}

void LyTable_setDiscardedItem (LyTable *table, LyString *name, void *data) {
    LyTable_setItem(table, name, data);
    LyMem_release(data);
}

void LyTable_setItem (LyTable *table, LyString *name, void *data) {
    LyTableItem *item = table->firstItem;
    LyMem_retain(data);
    LyMem_retain(name);
    if (item == NULL) {
        item = malloc(sizeof(LyTableItem));
        item->data = data; item->name = name;
        item->next = NULL; item->prev = NULL;
        table->firstItem = item;
        return;
    }
    while (1) {
        if (LyString_eq(name, item->name)) {
            LyMem_release(item->name);
            LyMem_release(item->data);
            item->data = data;
            item->name = name;
            return;
        }
        if (item->next == NULL)
            break;
        if (item == item->next)
            break;
        item = item->next;
    }
    LyLog(LYL_TABLE_DEBUG, "LyTable_setItem: Item does not exist in LyTable: Creating item.");
    if (1) { //Used to start new block & declare variables
        LyTableItem *newItem = malloc(sizeof(LyTableItem));
        newItem->data = data;
        newItem->name = name;
        newItem->next = NULL;
        newItem->prev = item;
        item->next = newItem;
    }
}

void * LyTable_getItem (LyTable *table, LyString *name) {
    LyTableItem *item = table->firstItem;
    while (item != NULL) {
        if (LyString_eq(name, item->name)) {
            return item->data;
        }
        item = item->next;
    }
    if (table->parent != NULL) {
        LyLog(LYL_TABLE_DEBUG, "LyTable_getItem: Item does not exist in LyTable: Checking parent table.");
        return LyTable_getItem(table->parent, name);
    }
    LyLog(LYL_TABLE_DEBUG, "LyTable_getItem: Item does not exist in LyTable, and table has no parent.\nReturning NULL.");
    return NULL;
}

void LyTable_removeItem (LyTable *table, LyString *name) {
    LyTableItem *item = table->firstItem;
    while (item != NULL) {
        if (LyString_eq(name, item->name)) {
            LyMem_release(item->name);
            LyMem_release(item->data);
            item->next->prev = item->prev;
            item->prev->next = item->next;
            free(item);
        }
    }
    if (table->parent != NULL) {
        LyLog(LYL_TABLE_DEBUG, "LyTable_removeItem: Item does not exist in LyTable: Checking parent table.");
        LyTable_removeItem(table->parent, name);
    } else {
        LyLog(LYL_TABLE_DEBUG, "LyTable_removeItem: Item does not exist in LyTable.");
    }
}






