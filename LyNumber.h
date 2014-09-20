//
//  LyNumber.h
//  Lycan
//
//  Created by Seiji Emery on 2/13/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _LY_NUMBER_H_
#define _LY_NUMBER_H_

#include "LyCore.h"

typedef struct LyNumber {
    LyClass meta;
    float val;
}LyNumber;

LyNumber * LyNumber_new (float val);
LyNumber * LyNumber_newFromString (char* string);
LyNumber * LyNumber_newFromStringSlice (char* string, int start, int end);

#endif