//
//  LyTypes.h
//  Lycan
//
//  Created by Seiji Emery on 2/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

/*
OLD - UNUSED
*/


#ifndef _LY_TYPES_
#define _LY_TYPES_

// Note: It is crucial to understand how Lycan's memory management works:
// see the note on LyMemoryManagement below.

/*
 == Coding conventions Conventions ==
 C OOP Style:
 macros, enum values: CAPS_WITH_UNDERSCORES
 types: PascalCase
 regular names - variables, functions, etc.: camelCase
 methods: Joined type and method with underscore: MyType_myMethod
 (similar to MyClass::myMethod in C++)
 * A method's first parameter should always be a pointer to the object instance
   (eg. MyList_append (MyList *list, MyListItem *item))
 * Constructors should be provided for all objects; deconstructors should be
   used for container types (which contain other values that need to be freed),
   whereas simple objects can simply be passed to free() (or LyMem_release(), if
   required (See note on LyMemoryManagement)).
 * The return type of a method should generally be void - value setting
   should be done internally using pointers, and method return types should be
   reserved either for getting state or convenience.
 
 Names are based off of Apple's naming conventions (Like C, Objective-C
 doesn't support method overloading or namespaces (it's a strict superset), 
 so naming has similar considerations):
 A short prefix (Ly in this case) is used to emulate namespaces.
 'Overloaded' methods must have different names, so method names should 
 reflect their parameters: the default method should use the base name
 (eg. LyName_new (char *string)), while other versions names should reflect
 their parameters (eg. LyName_newFromSlice (char *string, int start, int end)).
 
 Though much more verbose, method calls are signifitantly easier to understand than if
 written using C++ conventions, since function/method names describe their own parameters,
 and a method's type (class) is immediately visible from the name.
*/

/*
LyTypes:

LyType

LyValue -unfinished-
 LyNumber
 LyString
 LyName
 LyBlock
LyToken -deprecated-

LyThread -todo-
 LyLine
  LyInstr
  
LyList -todo-
 LyListItem
LyMap -todo-
 LyMapItem -todo-
*//*

typedef enum LyType {
    LY_NULL,
    LY_ENDLINE,
    LY_SEP,
    
    LY_NAME,
    LY_INSTR,
    LY_VALUE,
    LY_FLOAT,
    LY_INT,
    LY_STRING,
    LY_LIST_LITERAL,
    LY_LIST_MAP,
    LY_LIST,
    LY_MAP,
    
    LY_OP_SET,
    LY_OP_COPY,
    LY_OP_IF,
    LY_OP_WHILE,
    
    LY_CMP_EQ,
    LY_CMP_NE,
    LY_CMP_LT,
    LY_CMP_GT,
    LY_CMP_LTE,
    LY_CMP_GTE,
    LY_CMP_AND,
    LY_CMP_OR,
    LY_CMP_NOT,
    
    LY_OP_ADD,
    LY_OP_RADD,
    LY_OP_SUB,
    LY_OP_RSUB,
    LY_OP_MUL,
    LY_OP_RMUL,
    LY_OP_DIV,
    LY_OP_RDIV,
    LY_OP_MOD,
    LY_OP_RMOD,
    LY_OP_POW,
    LY_OP_RPOW
}LyType;

/*
#define LY_NULL     0x0 // On encountering this instruction: do nothing.
#define LY_ENDLINE  0x11
#define LY_SEP      0x12
#define LY_NAME     0x13
#define LY_INSTRUCTION  0x14
#define LY_VALUE    0x20
#define LY_FLOAT    0x21
#define LY_STRING   0x22
#define LY_INT      0x23
#define LY_BLOCK    0x24
#define LY_LIST_LITERAL 0x25
#define LY_LIST_MAP     0x26
#define LY_LIST         0x27
#define LY_MAP          0x28

#define LY_OP_SET    0x31
#define LY_OP_COPY   0x32
#define LY_OP_EXEC   0x33
#define LY_OP_IF     0x34
#define LY_OP_WHILE  0x35
#define LY_CMP_EQ     0x41
#define LY_CMP_NE     0x42
#define LY_CMP_NOT    0x43
#define LY_CMP_LT     0x44
#define LY_CMP_GT     0x45
#define LY_CMP_LTE    0x46
#define LY_CMP_GTE    0x47
#define LY_CMP_AND    0x48
#define LY_CMP_OR     0x49
#define LY_GROUP_BEGIN    41
#define LY_GROUP_END      42
#define LY_BLOCK_BEGIN    43
#define LY_BLOCK_END      44
#define LY_NS_BEGIN       45
#define LY_NS_END         46
#define LY_OP_ADD    51
#define LY_OP_SUB    52
#define LY_OP_MUL    53
#define LY_OP_DIV    54
#define LY_OP_MOD    55
#define LY_OP_POW    56
#define LY_OP_RADD   61
#define LY_OP_RSUB   62
#define LY_OP_RMUL   63
#define LY_OP_RDIV   64
#define LY_OP_RMOD   65
#define LY_OP_RPOW   66
*/

// ******************
// Memory Management:
// ******************
/*typedef struct LyMemRef {
    int count;
}LyMemRef;

/* LyMemory Management: ->README<-
 Value types (LyNumber, LyString, LyName) - stored within other objects as void *
 will likely be referenced in multiple places; as such, default C memory management
 of malloc/free is insufficient, since prematurely freeing an object may cause an
 error somewhere, and not freeing it at all will cause memory leaks.
 To solve this, a simple reference counting system is used: LyMem_alloc calls
 malloc and sets the reference count to 1; subsequent calls to LyMem_retain and 
 LyMem_release increment/decrement this counter; when the counter drops to 0 the
 object is freed.
 
 This reference counting system is very basic, and as such, it is unfortunately not
 idiot proof. As such, the following rules must be obeyed:
 0. Use this memory management system only for VALUE types (LyString, LyNumber, LyName).
    Don't ever use LyMem_alloc for a non-supported type.
 1. Object's struct definition must have 'LyMemRef refCount' as the first item. This is
    used to implement struct inheiritance from LyMemRef; if not present the object is
    incompatible with the LyMem functions.
 2. Use LyMem_alloc instead of malloc, and LyMem_release instead of free.
    Don't mix these - ever.
 3. LyMem_retain must be called whenever a new reference is created to an object;
    LyMem_release must be called whenever an object is released.
 4. A call to LyMem_alloc must be matched with a call to LyMem_release, plus an
    additional call to LyMem_release for every time that LyMem_retain is called.
 5. Don't forget to release local, one-use objects before a function returns.
    Failure to do so will cause memory leaks.
 6. Don't call LyMem_release too many times. Like calling free twice on the same block
    of memory, doing so will likely cause the program to crash.
*/
/*
void * LyMem_alloc (size_t size);
int LyMem_retain (void *data);
int LyMem_release (void *data);
*/

// ************
// Value types:
// ************
// LyValue      TODO
//  LyNumber
//  LyString
//  LyName
// LyToken -deprecated-

/*
// ==LyNumber== 
typedef struct LyNumber {
    LyMemRef refCount;
    LyType type;
    double val;
}LyNumber;
// =Constructors=
LyNumber * LyNumber_new (LyType type, double val);
LyNumber * LyNumber_newFromString (char* string);
LyNumber * LyNumber_newFromStringSlice (char* string, int start, int end);
// =Methods=


// ==LyString==
/*
typedef struct LyString {
    LyMemRef refCount;
    char *string;
    int len;
}LyString;
// =Constructors=
LyString * LyString_new (char *string);
LyString * LyString_newWithLen (char *string, int len);
LyString * LyString_newFromSlice (char *string, int start, int end);
// =Methods=
*/
/*
// ==LyName==
typedef struct LyName {
    LyMemRef refCount;
    char *name;
    //int nsId;
    //struct LyNamespace *namespace;
}LyName;
// =Constructors=
LyName  * LyName_new (char *name);
//LyName  * LyName_newWithNsInfo (char *name, int nsId, LyNamespace *namespace);
LyName  * LyName_newFromSlice (char *string, int start, int end);
// =Methods=


// ==LyToken==
// -deprecated-
typedef struct LyToken {
    LyType type;
    struct LyToken *next, *prev;
    void *data;
}LyToken;
// =Constructors=
LyToken * LyToken_new (LyType type, void *data);
        // Append a token to the given token stack; return $token. 
// =Methods=
LyToken * LyToken_append (LyToken *stack, LyToken *token);
        // Remove a token from its stack
void LyToken_removeFromStack (LyToken *token);
        // Remove and a token from its stack and free the memory
void LyToken_removeFromStackAndFree (LyToken *token);
        // Free all items below the stack element (next elements; not prev)
void LyToken_freeStack (LyToken *stack);
        // Free all linked elements in the stack
void LyToken_freeEntireStack (LyToken *stack);


// ==LyValue==
// -deprecated-
typedef struct LyValue {
    LyType type;
    void *data;
}LyValue;
// =Constructors=
LyValue * LyValue_new (LyType type, void *data);
// =Methods=


// ******************
// Instruction Thread:
// ******************
// LyThread     TODO
//  LyLine      TODO
//    LyInstr   TODO


// ==LyInstr==
typedef struct LyInstr {
    LyType type;
    struct LyInstr *prev, *next;
    void *data;
}LyInstr;
// =Constructors=
LyInstr * LyInstr_new (LyType type, void *data);
LyInstr * LyInstr_newWithNeighbors (LyType type, void *data, LyInstr *prev, LyInstr *next);
// =Methods=


// ==LyLine==
typedef struct LyLine {
    struct LyThread *parent;
    struct LyLine *prev, *next;
    LyInstr *instr;
}LyLine; 
// =Constructors=
LyLine * LyLine_new (struct LyThread *parent);
// =Methods=


// ==LyThread==
typedef struct LyThread {
    struct LyStack *parent;
    LyLine *line;
}LyThread;
// =Constructors=
LyThread * LyThread_new ();
LyThread * LyThread_newWithParent (struct LyThread *parent);
// =Methods=
void LyThread_append (LyThread *thread, LyInstr *instr);
void LyThread_appendLine (LyThread *thread, LyLine *line);
// The following methods are for iteration:
// These methods change the context (LyInstr/LyLine) of the thread 
// (Jumping to the front/end or moving forward/backward), and return
// the new context.
// (Context is changed internally; the return is only for convenience)
LyInstr *LyThread_gotoLastItem (LyThread *thread);
LyInstr *LyThread_gotoFirstItem (LyThread *thread);
LyInstr *LyThread_iterForward (LyThread *thread);
LyInstr *LyThread_iterBack (LyThread *thread);

LyLine *LyThread_gotoFirstLine (LyThread *thread);
LyLine *LyThread_gotoLastLine (LyThread *thread);
LyLine *LyThread_gotoPrevLine (LyThread *thread);
LyLine *LyThread_gotoNextLine (LyThread *thread);



// ************
// LyContainers
// ************
// LyList       TODO
//  LyListItem  TODO
// LyMap        TODO
//  LyMapItem   TODO



// ==LyListItem==
typedef struct LyListItem {
    struct LyListItem *prev, *next;
    LyValue *val;
}LyListItem;
// =Constructors=

// =Methods=


// ==LyList==
typedef struct LyList {
    LyListItem *first, *last;
}LyList;
// =Constructors=
//LyList * LyList_newFromThreadLiteral (LyThread *literal);
// =Methods=


// ==LyMapItem==
typedef struct LyMapItem {
    struct LyMap *parent;
    struct LyMapItem *up, *left, *right;
    LyName *name;
    LyType type;
    void *data;
}LyMapItem;
// =Constructors=
//LyMap *LyMap_newFromThreadLiteral (LyThread *literal);
// =Methods=


// ==LyMap==
typedef struct LyMap {
    struct LyMap *parent;
    LyMapItem *root;
}LyMap;
// =Constructors=

// =Methods=

/*

typedef struct LyInstruction {
    LyType type;
    struct LyInstruction *prev, *next;
    void *data;
}LyInstruction;


typedef struct LyNsNode {
    LyVariable *var;
    struct LyNsNode *left, *right;
}LyNsNode;

typedef struct LyNamespace {
    struct LyNamespace *parent;
    LyNsNode *root;
}LyNamespace;

typedef struct LyBlock {
    LyNamespace *localNs;
    LyStack *localStack;
}LyBlock;
*/

/*
void         LyNamespace_set (LyNamespace *ns, LyVariable *var);
LyVariable * LyNamespace_find (LyNamespace *ns, char *name);
void         LyNamespace_setValueByName (LyNamespace *ns, char *name, LyValue *value);
LyValue    * LyNamespace_getValueByName (LyNamespace *ns, char *name);


*/












#endif