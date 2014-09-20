//
//  LyNumber.c
//  Lycan
//
//  Created by Seiji Emery on 2/13/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>

#include "LyString.h"

#include "LyNumber.h"


LyNumber * LyNumber_new (float val) {
    LyNumber *num = LyMem_alloc(sizeof(LyNumber), LY_T_NUMBER);
    num->val = val;
    return num;
}

LyNumber * LyNumber_newFromString (char* string) {
    LyNumber *num = LyMem_alloc(sizeof(LyNumber), LY_T_NUMBER);
    num->val = atof(string);
    return num;
}

LyNumber * LyNumber_newFromStringSlice (char* string, int start, int end) {
    LyNumber *num = LyMem_alloc(sizeof(LyNumber), LY_T_NUMBER);
    LyString *subStr = LyString_newFromSlice(string, start, end);
    num->val = atof(subStr->string);
    LyMem_release(subStr);
    return num;
}