//
//  LyString.c
//  Lycan
//
//  Created by Seiji Emery on 2/6/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>
#include <string.h>

#include "LyCore.h"
#include "LyLog.h"
#include "LyString.h"

inline char * LyStringInternal_typeAsString (void *self) {}





LyString *LyString_new (char *string) {
    LyString *str = LyMem_alloc(sizeof(LyString), LY_T_STRING);
    str->len = strlen(string);
    str->string = malloc(sizeof(char) * (str->len + 1));
    strcpy(str->string, string);
    return str;
}

LyString *LyString_newFromSlice (char *string, int start, int end) {
    LyString *str = LyMem_alloc(sizeof(LyString), LY_T_STRING);
    size_t len = strlen(string);
    if (len < end - start) {
        str->len = len;
    } else {
        str->len = end - start;
    }
    str->string = malloc(sizeof(char) * (str->len + 1));
    for (int i = 0; i < str->len; i++) {
        str->string[i] = string[i + start];
    }
    return str;
}

    //Copy $other onto the end of $base.
void LyString_append (LyString *base, LyString *other) {
    char *basePtr, *otherPtr;
    base->string = realloc(base->string, base->len + other->len + 1);
    basePtr = base->string + base->len;
    otherPtr = other->string;
    while (*otherPtr != 0) {
        *basePtr = *otherPtr;
        basePtr++;
        otherPtr++;
    }/*
    for (size_t i = base->len; i < other->len + 1; i++) {
        base->string[i + base->len] = other->string[i];
    }*/
    base->len += other->len;
}
/*
void LyString_appendLine (LyString *base, LyString 

*/

    //Copy $other onto the end of $base and release $other.
void LyString_merge (LyString *base, LyString *other) {
    LyString_append(base, other);
    LyMem_release(other);
}


    //Return true if all characters in $self match $other
bool LyString_eq (LyString *self, LyString *other) {
    char *sPtr = self->string;
    char *oPtr = other->string;
    if (self->len != other->len) {
        LyLog(LYL_STRING_DEBUG, "LyString_eq debug: != (string length mismatch)");
        return false;
    }
    while (*sPtr != 0 && *oPtr != 0) {
        if (*sPtr != *oPtr) {
            LyLog(LYL_STRING_DEBUG, "LyString_eq debug: != (character mismatch)");
            return false;
        }
        oPtr ++;
        sPtr ++;
    }
    return true;
}


    //Check $fragment against the first characters in $base; return true if they match.
bool LyString_match (LyString *base, LyString *fragment) {
    char *bPtr = base->string;
    char *fPtr = fragment->string;
    if (base->len < fragment->len) {
        LyLog(LYL_STRING_DEBUG, "LyString_match debug: != (base len is less than fragment len)");
        return false;
    }
    while (*bPtr != 0 && *fPtr != 0) {
        if (*bPtr != *fPtr) {
            LyLog(LYL_STRING_DEBUG, "LyString_match debug: != (character mismatch)");
            return false;
        }
        bPtr ++; fPtr++;
    }
    return true;
}

bool LyString_matchRaw (LyString *base, char *fragment) {
    char *bPtr = base->string;
    char *fPtr = fragment;
    while (*bPtr != 0 && *fPtr != 0) {
        if (*bPtr != *fPtr) {
            LyLog(LYL_STRING_DEBUG, "LyString_match debug: != (character mismatch)");
            return false;
        }
        bPtr ++; fPtr ++;
    }
    return true;
}

    // Check $fragment against the first characters in $base.
    // if they match, remove those characters from the start of $base,
    // and return true. (This is specialized behaviour used heavily in parsing phase).
bool LyString_pop (LyString *base, LyString *fragment) {
    char *fPtr, *bPtr, *nPtr, *newString;
    if (!LyString_match(base, fragment)) {
        LyLog(LYL_STRING_DEBUG, "LyString_pop debug: string mismatch.");
        return false;
    }
    base->len -= fragment->len;
    newString = malloc(base->len + 1);
    fPtr = fragment->string; bPtr = base->string; nPtr = newString;
    while (*fPtr != 0) {
        fPtr ++; bPtr ++;
    } while (*bPtr != 0) {
        *nPtr = *bPtr;
        nPtr ++; bPtr ++;
    }
    free(base->string);
    base->string = newString;
    return true;
}

bool LyString_popRaw (LyString *base, char *fragment) {
    char *fPtr, *bPtr, *nPtr, *newString;
    size_t fLen = strlen(fragment);
    if (!LyString_matchRaw(base, fragment)) {
        LyLog(LYL_STRING_DEBUG, "LyString_pop debug: string mismatch.");
        return false;
    }
    base->len -= fLen;
    newString = malloc(base->len + 1);
    fPtr = fragment; bPtr = base->string; nPtr = newString;
    while (*fPtr != 0) {
        fPtr ++; bPtr ++;
    } while (*bPtr != 0) {
        *nPtr = *bPtr;
        nPtr ++; bPtr ++;
    }
    free(base->string);
    base->string = newString;
    return true;
}
/*
    // Remove a slice from $start to $end from $string and return it.
LyString * LyString_popSlice (LyString *string, int start, int end) {
    char *slice, *newBase; 
    char *bPtr, *sPtr, *nPtr;
    size_t sliceLen = end - start;
    size_t newLen = string->len - sliceLen;
    
    slice = malloc(sizeof(char) * (sliceLen + 1));
    newBase = malloc(sizeof(char) * (newLen + 1));
    bPtr = string->string;
    sPtr = slice;
    nPtr = newBase;
    for (int i = 0; i < string->len; i++) {
        if (i < start || i > end) {
            
        }
    }
    
    
}*/




