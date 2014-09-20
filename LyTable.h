//
//  LyTable.h
//  Lycan
//
//  Created by Seiji Emery on 2/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _LY_TABLE_H_
#define _LY_TABLE_H_
#include "LyCore.h"
#include "LyString.h"

typedef struct LyTable {
    LyClass meta;
    struct LyTable *parent;
    struct LyTableItem *firstItem;
}LyTable;

typedef struct LyTableItem {
    struct LyTableItem *prev, *next;
    LyString *name;
    void *data;
}LyTableItem;

LyTable * LyTable_new ();
void LyTable_freeTable (LyTable *table);
LyTable * LyTable_cloneTable (LyTable *table);

void LyTable_setItem (LyTable *table, LyString *name, void *data);
void LyTable_setDiscardedItem (LyTable *table, LyString *name, void *data);
void * LyTable_getItem (LyTable *table, LyString *name);
void LyTable_removeItem (LyTable *table, LyString *name);

#endif