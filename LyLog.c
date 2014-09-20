//
//  LyLog.c
//  Lycan
//
//  Created by Seiji Emery on 2/5/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "LyCore.h"
#include "LyString.h"

#include "LyLog.h"

FILE *lyLog;
char lyLogIndentBuf[32];
int lyLogIndent;

#define LY_LOG_PATH "Lycan.log"

void LyLog_init () {
    lyLogIndent = 0; 
    lyLogIndentBuf[0] = '\0';
    
    lyLog = fopen(LY_LOG_PATH, "w");
    if (lyLog == NULL)
        printf("ERROR: Log file could not be opened.");
    fprintf(lyLog, "=============\n");
}

void LyLog_flush () { // Deprecated
    if (lyLog == NULL)
        LyLog_init();
    fflush(lyLog);
}

void LyLog_cleanup () {
    fclose(lyLog);
}

FILE * newLog (char *name) {
    return fopen("w", name);
}

void LyLog (LyLogType type, LyLogCat cat, char *msg, ...) {
    va_list arglist;
    if (!cat)
        return;
    if (lyLog == NULL)
        LyLog_init();
    va_start(arglist, msg);
    _LogToFile(stdout, type, msg, arglist);
    va_end(arglist);
}

void LyLogError (LyLogType type, LyLogCat cat, char *msg, ...) {
    va_list arglist;
    if (!cat)
        return;
    if (lyLog == NULL)
        LyLog_init();
    va_start(arglist, msg);
    fprintf(stderr, "<ERROR> ");
    _LogToFile(stderr, type, msg, arglist);
    va_end(arglist);
}




/*
void LyLogMemory (LyLogType type, void *ptr) {
    char buffer[65];
    LyClass *meta = ptr;
    sprintf(buffer, "[%p] refCount: %d", ptr, meta->refCount);
    AltLog(lyLog, type, buffer);
}

void LyLogMemoryWithMessage (LyLogType type, char *msg, void *ptr) {
    char buffer[512];
    LyClass *meta = ptr;
    sprintf(buffer, "%s [%p] refCount: %d", msg, ptr, meta->refCount);
    AltLog(lyLog, type, buffer);
}*/

void LogToFile (FILE *fp, LyLogType type, char *msg, ...) {
    va_list arglist;
    va_start(arglist, msg);
    _LogToFile(fp, type, msg, arglist);
    va_end(arglist);
}

void _LogToFile (FILE *fp, LyLogType type, char *msg, va_list arglist) {
    if (fp == NULL)
        return;
    fprintf(fp, "%s", lyLogIndentBuf);
    switch(type) {
        case LY_CORE:
        fprintf(fp, "Lycan: ");
        break;
        case LY_DEBUG:
        //fprintf(fp, "Debug: ");
        break;
        case LY_LOG_MSG:
        fprintf(fp, "\t");
        break;
        case LY_LOG_MEM_ERROR:
        fprintf(fp, "Memory Error:\t");
        break;
        case LY_LOG_RUNTIME_ERROR:
        fprintf(fp, "Runtime Error:\t");
        break;
        case LY_LOG_SYNTAX_ERROR:
        fprintf(fp, "Syntax Error:\t");
        break;
        case LY_LOG_PARSER_ERROR:
        fprintf(fp, "Parser Error:\t");
        break;
        case LY_LOG_EXEC_ERROR:
        fprintf(fp, "Exec Error:\t");
        break;
        case LY_LOG_ERROR:
        fprintf(fp, "Error:\t");
        case LY_LOG_WARNING:
        fprintf(fp, "Warning:\t");
        break;
    }
    vfprintf(fp, msg, arglist);
    fprintf(fp, " \n");
    fflush(lyLog);
}



void LyLog_indent () {
    lyLogIndent ++;
    if (lyLogIndent > 32)
        lyLogIndent = 32;
    int i = 0;
    for (; i < lyLogIndent; i++)
        lyLogIndentBuf[i] = '\t';
    lyLogIndentBuf[i] = '\0';
}

void LyLog_dedent () {
    lyLogIndent --;
    if (lyLogIndent < 0)
        lyLogIndent = 0;
    int i = 0;
    for (; i < lyLogIndent; i++)
        lyLogIndentBuf[i] = '\t';
    lyLogIndentBuf[i] = '\0';
    
}




