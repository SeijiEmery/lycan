//
//  LyLog.h
//  Lycan
//
//  Created by Seiji Emery on 2/5/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _LY_LOG_H_
#define _LY_LOG_H_

#include <stdio.h>
#include "lyCore.h"

typedef enum LyLogType {
    LY_CORE,
    LY_DEBUG,
    LY_LOG_MSG,
    LY_LOG_MEM_ERROR,
    LY_LOG_RUNTIME_ERROR,
    LY_LOG_SYNTAX_ERROR,
    LY_LOG_PARSER_ERROR,
    LY_LOG_EXEC_ERROR,
    LY_LOG_ERROR,
    LY_LOG_WARNING
}LyLogType;

typedef bool LyLogCat;

#define LYL_MEM_DEBUG       LY_DEBUG, false
#define LYL_MEM_WARN        LY_LOG_WARNING, true
#define LYL_MEM_ERROR       LY_LOG_MEM_ERROR, true
#define LYL_COMPILE_DEBUG   LY_DEBUG, false
#define LYL_COMPILE_WARN    LY_LOG_WARNING, true
#define LYL_COMPILE_ERROR   LY_LOG_PARSER_ERROR, true
#define LYL_EXEC_DEBUG      LY_DEBUG, false
#define LYL_EXEC_WARN       LY_LOG_WARNING, true
#define LYL_EXEC_ERROR      LY_LOG_EXEC_ERROR, true
#define LYL_SYNTAX_WARN     LY_LOG_WARNING, true
#define LYL_SYNTAX_ERROR    LY_LOG_SYNTAX_ERROR, true
#define LYL_TABLE_DEBUG     LY_DEBUG, false
#define LYL_STRING_DEBUG    LY_DEBUG, false
#define LYL_CORE            LY_CORE, true
#define LYL_CORE_DEBUG      LY_DEBUG, false
#define LYL_CORE_ERROR      LY_LOG_ERROR, true
#define LYL_DEBUG           LY_DEBUG, false
#define LYL_WARN            LY_LOG_WARNING, true
#define LYL_ERROR           LY_LOG_ERROR, true

void LyLog_init ();
void LyLog_flush ();
void LyLog_cleanup ();

FILE * newLog (char *fileName);

void LyLog (LyLogType type, LyLogCat cat, char *msg, ...);
void LyLogError (LyLogType type, LyLogCat cat, char *msg, ...);
void LogToFile (FILE *fp, LyLogType type, char *msg, ...);
void _LogToFile (FILE *fp, LyLogType type, char *msg, va_list arglist);

void LyLog_indent ();
void LyLog_dedent ();
/*
void LyLogMemory (LyLogType type, void *ptr);
void LyLogMemoryWithMessage (LyLogType type, char *string, void *ptr);
*/
#endif