//
//  LyInstr.c
//  Lycan
//
//  Created by Seiji Emery on 2/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "LyInstr.h"

LyInstr * LyInstr_new (LyInstrType op) {
    LyInstr *instr = LyMem_alloc(sizeof(LyInstr), LY_T_INSTR);
    instr->type = op;
    return instr;
}

char * LyInstr_toString (LyInstr *instr) {
    switch (instr->type) {
        case LYI_ENDLINE:
        return "LYI_ENDLINE";
        case LYI_ENDQUEUE:
        return "LYI_ENDQUEUE";
        case LYI_SET:
        return "LYI_SET";
        case LYI_ADD:
        return "LYI_ADD";
        case LYI_SUB:
        return "LYI_SUB";
        case LYI_DIV:
        return "LYI_DIV";
        case LYI_MUL:
        return "LYI_MUL";
        case LYI_POW:
        return "LYI_POW";
        case LYI_MOD:
        return "LYI_MOD";
        case LYI_EQ:
        return "LYI_EQ";
        case LYI_NE:
        return "LYI_NE";
        case LYI_GT:
        return "LYI_GT";
        case LYI_LT:
        return "LYI_LT";
        case LYI_GTE:
        return "LYI_GTE";
        case LYI_LTE:
        return "LYI_LTE";
        case LYI_SCOPE:
        return "LYI_SCOPE";
        default:
        return "Unknown instruction";
    }
}