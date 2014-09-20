//
//  LyString.h
//  Lycan
//
//  Created by Seiji Emery on 2/6/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _LY_STRING_H_
#define _LY_STRING_H_

#include <stdlib.h>
#include "LyCore.h"

typedef struct LyString {
    LyClass meta;
    char *string;
    size_t len;
}LyString;
// =Constructors=
LyString *LyString_new (char *string);
LyString *LyString_newFromSlice (char *string, int start, int end);
// =Methods=
//Copy $other onto the end of $base.
void LyString_append (LyString *base, LyString *other);
//void LyString_appendLine (LyString *base, LyString *other);
//Copy $other onto the end of $base and release $other.
void LyString_merge (LyString *base, LyString *other);
//Return true if all characters in $self match $other
bool LyString_eq (LyString *self, LyString *other);
//Check $fragment against the first characters in $base; return true if they match.
bool LyString_match (LyString *base, LyString *fragment);
bool LyString_matchRaw (LyString *base, char *fragment);
// Check $fragment against the first characters in $base.
// if they match, remove those characters from the start of $base,
// and return true. (This is specialized behaviour used heavily in parsing phase).
bool LyString_pop (LyString *base, LyString *fragment);
bool LyString_popRaw (LyString *base, char *fragment);
// Remove a slice from $start to $end from $string and return it.
LyString * LyString_popSlice (LyString *string, int start, int end);

LyString * LyString_combine (LyString *base, void *other);

#endif