//
//  LyInstr.h
//  Lycan
//
//  Created by Seiji Emery on 2/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _LY_INSTR_H_
#define _LY_INSTR_H_

#include "LyCore.h"

typedef enum LyInstrType {
    LYI_ENDLINE,
    LYI_ENDQUEUE,
    LYI_SET,
    LYI_ADD,
    LYI_SUB,
    LYI_DIV,
    LYI_MUL,
    LYI_MOD,
    LYI_POW,
    LYI_EQ,
    LYI_NE,
    LYI_GT,
    LYI_LT,
    LYI_GTE,
    LYI_LTE,
    LYI_SCOPE // eg. myThing.x -> queue: LyName('myThing'), LyInstr(LYI_SCOPE), LyName('x')
}LyInstrType;

typedef struct LyInstr {
    LyClass meta;
    LyInstrType type;
}LyInstr;

LyInstr * LyInstr_new (LyInstrType op);

char * LyInstr_toString (LyInstr *instr);

#endif