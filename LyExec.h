//
//  LyExec.h
//  Lycan
//
//  Created by Seiji Emery on 2/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _LY_EXEC_H_
#define _LY_EXEC_H_

#include "LyTable.h"
#include "LyQueue.h"
#include "LyInstr.h"

void * LyRun (LyTable *ns, LyQueue *queue);
void * LyExec (LyTable *ns, LyQueue *queue, LyInstrType endInstr);

#endif