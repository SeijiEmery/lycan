
//
//  main.c
//  Lycan
//
//  Created by Seiji Emery on 2/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "LyCore.h"
#include "LyQueue.h"
#include "LyCompiler.h"
#include "LyExec.h"
#include "LyTable.h"
#include "LyInstr.h"
#include "LyLog.h"
#include "LyNumber.h"

void runLine (char *input, LyTable *ns) {
    LyQueue *queue = LyCompile(input);
    //LyDebug(queue);
    LyPrint(LyExec(ns, queue, LYI_ENDLINE));
    LyMem_release(queue);
    LyLog(LYL_DEBUG, "Namespace:");
    LyLog_indent();
    LyDebug(ns);
    LyLog_dedent();
}

void runLycanInterpreter (LyTable *ns) {
    char input[1024];

    Lycan_init();
    printf(">> ");
    while (fgets(input, 1024, stdin) != NULL) {
        if (strcmp(input, "quit") == 0)
            break;
        runLine(input, ns);
        
        printf(">> ");
        /*queue = LyCompile(input);
        LyLog(LYL_DEBUG, "Input: \"%s\"\n", input);
        LyDebug(queue);
        LyPrint(LyExec(ns, queue, LYI_ENDLINE));
        LyDebug(ns);
        LyMem_release(queue);
        printf(">>");*/
    }
    Lycan_cleanup();
}

void tstRunLine (char *input, LyTable *ns) {
    LyQueue *queue = LyCompile(input);
    printf(">> %s", input);
    LyPrint(LyExec(ns, queue, LYI_ENDLINE));
    LyMem_release(queue);
    LyDebug(ns);
}


void runLycanTest (LyTable *ns) {
    char *l1 = "x = 10\n";
    char *l2 = "x = x + 10\n";
    char *l3 = "x\n";
    char *l4 = "x = 25\n";
    char *l5 = "x + 10\n";
    
    tstRunLine(l1, ns);
    tstRunLine(l2, ns);
    tstRunLine(l3, ns);
    tstRunLine(l4, ns);
    tstRunLine(l5, ns);
}

int main (int argc, const char * argv[])
{
    /*LyQueue *queue;
    LyTable *ns = LyTable_new();
    char *input = "x = 10; x;";

    Lycan_init();
    
    queue = LyCompile(input);
    LyDebug(queue);
    LyLog_flush();
    LyPrint(LyExec(ns, queue, LYI_ENDLINE));
    LyLog(LYL_DEBUG, "Phase 2:");
    LyDebug(queue);
    LyDebug(ns);
    LyLog_flush();
    LyPrint(LyExec(ns, queue, LYI_ENDLINE));
    LyDebug(ns);
    
    Lycan_cleanup();*/
    LyTable *ns = LyTable_new();
    Lycan_init();
    //runLycanTest(ns);
    runLycanInterpreter(ns);
    Lycan_cleanup();
}

