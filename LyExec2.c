//
//  LyExec2.c
//  Lycan
//

#include "LyCore.h"
#include "LyError.h"
#include "LyStack.h"
#include "LyString.h"
#include "LyNumber.h"
#include "LyTable.h"
#include "LyInstr.h"

#include "LyExec.h"



// Debugging utils
#define trace(...args)      LyTrace_logMsg(args)
#define beginTrace(...args) LyTrace_begin(args)
#define endTrace()          LyTrace_end()
#define raise(...args)      LyTrace_raise(args); state->err = true; return
#define traceStore(state)      LyTrace_setTable(LyExecState_getTable(state))

/*
 (conceptual) debugging/error handling system:
 a traceback stack is maintained by all compliant functions:
 beginTrace() pushes a new session onto the stack with a given name
 trace() appends a message to the current session
 endTrace() pops the current session off of the stack
   (function returned normally without errors)
 traceStore() stores a table of LyObjects as key/value pairs into the
   current session. In this case it stores the current exec state
   (buffer, value, queue, and ns).
   This provides inspection of the state of the process at the point
   of failure. Stores separate states for each session.
 raise() does several things:
   print the error message to stderr
   dump a traceback using traceback session info collected using the above
     trace-related execCalls (nested sessions w/ messages and state info)
     if enabled (should be enabled for devs; disabled for release by default).
   register that an error has occured w/ exec state
     (for this reason all functions need to use execCall(func) instead of just
     func() to handle early errors)
   return early
 */


// Function execCall wrapper:
// execCall the function, and check the state for errors (return early if there is one)
#define execCall(func) func(state); if (state->err) return

// LyExec local macros 
// (assumes existance of LyExecState *state)
// do NOT use anywhere else
#define buffer  state->buffer;
#define value   state->value;
#define ns      state->ns;
#define queue   state->queue;

    // Error macros
#define assignmentError(op, assignedObj) \
    "assignment error: %s cannot assign to %s", \
    op, LyTypeToString(LyTypeOf(assignedObj))
#define typeError(op, recieverObj, senderObj) \
    "type error: %s %s to %s", op, \
    LyTypeToString(LyTypeOf(senderObj)), \
    LyTypeToString(LyTypeOf(recieverObj))
#define unexpectedEolError \
    "parsing error: unexpected end of line"


//
// LyExec entrypoint
//

// ...

//
// LyExec subroutines
//

void _LyExec_set (LyExecState *state) {
    beginTrace("LyExec op: LYI_SET");
    traceStore(state);
    
    if (LyTypeOf(buffer) != LY_T_NAME) {
        raise(assignmentError("LYI_SET", buffer));
    }
    
    trace("Obtaining value from LyExec");
    //value = LyExec(ns, queue, LYI_ENDLINE);
    value = execCall(LyExec); 
    // equivalent to 'value = LyExec(state);
    //                if (state->err) return;'
    traceStore(state);
    if (value == NULL) {
        raise(unexpectedEolError);
    }
    
    if (LyTypeOf(value) == LY_T_GROUP) {
        trace("Value type is LyGroup: executing group.");
        value = execBlock(value);
        traceStore(state);
        if (value == NULL) {
            raise(unexpectedEolError);
        }
    }
    trace("Setting item.");
    LyTable_setItem(ns, buffer, value);
    endTrace();
}

// ...

#undef buffer
#undef value
#undef ns
#undef queue
#undef execCall

#undef assignmentError
#undef typeError
#undef unexpectedEolError

#undef raise
#undef trace
#undef beginTrace
#undef endTrace
#undef traceStore




