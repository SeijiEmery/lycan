//
//  LyVM.c
//  Lycan
//
//  Created by Seiji Emery on 2/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

/*
OLD - UNUSED
*/


/*
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "LyTypes.h"
#include "LyVM.h"

typedef char bool;
#define true 1
#define false 0

LyToken * LyTokenizeInput (char *input) {
    int i = 0;
    LyToken *root = NULL;
    
    LyType tokenType;
    void * tokenData;
    
    int lastTokenIndex;
    //int strBufStart, strBufEnd;
    bool inString = false;
    bool inComment = false;
    int commentDepth = 0;
    bool singleLineComment = false;
    
    bool bufferIsAlphaNum = true;
    bool bufferIsNum = false;
    bool bufferHasFloat = false;
    int lastBufferUpdate = 0;
    int bufferLen = 0;
    
    while (input[i] != 0) {
        tokenType = LY_NULL;
        tokenData = NULL;
        if (inComment) {
            if (input[i] == '*' && input[i+1] == '/') {        // "*//*"
                commentDepth --;
                if (commentDepth < 0) {
                    printf("Syntax error: mismatched end paren\n");
                    commentDepth = 0;
                }
                i++;
            } else if (input[i] == '/' && input[i+1] == '*') { // "/*"
                commentDepth ++; i++;
            } else if (input[i] == '\n' || input[i] == ';') { // "\n" || ";"
                singleLineComment = false;
                //tokenType = LY_ENDLINE;
                LyToken_append(root, LyToken_new(LY_ENDLINE, NULL));
            }
            if (!singleLineComment && commentDepth == 0) {
                inComment = false;
            }
            i++;
            continue;
        }
        if (inString) {
            if (input[i] == '"') {      // """
                inString = false;
                tokenType = LY_STRING;
                //tokenData = LyString_newFromSlice(input, lastTokenIndex, i);
                LyToken_append(root, LyToken_new(LY_STRING, LyString_newFromSlice(input, lastTokenIndex, i)));
            }
            i++;
            continue;
        }
        if (input[i] == '/') {
            if (input[i+1] == '/') {        // "//"
                singleLineComment = true;
                inComment = true;
                i++;
            } else if (input[i+1] == '*') { // "/*"
                commentDepth++;
                inComment = true;
                i++;
            } else if (input[i+1] == '=') { // "/="
                tokenType = LY_OP_DIV;
                //LyToken_append(root, LyToken_new(LY_OP_RDIV, NULL));
                i++;
            } else {                        // "/"
                tokenType = LY_OP_DIV;
                //LyToken_append(root, LyToken_new(LY_OP_DIV, NULL));
            }
        } else if (input[i] == '"') {
            inString = true;
            lastTokenIndex = i;
        } else if (input[i] == '=') {
            if (input[i+1] == '=') {
                tokenType = LY_CMP_EQ;
                //LyToken_append(root, LyToken_new(LY_CMP_EQ, NULL));
                i++;
            } else {
                tokenType = LY_OP_SET;
                //LyToken_append(root, LyToken_new(LY_OP_SET, NULL));
            }
        } else if (input[i] == ':' && input[i+1] == '=') {
            LyToken_append(root, LyToken_new(LY_OP_COPY, NULL));
        } else if (input[i] == '+') {
            if (input[i+1] == '=') {
                tokenType = LY_OP_RADD;
                //LyToken_append(root, LyToken_new(LY_OP_RADD, NULL));
                i++;
            } else {
                tokenType = LY_OP_ADD;
                //LyToken_append(root, LyToken_new(LY_OP_ADD, NULL));
            }
        } else if (input[i] == '-') {
            if (input[i+1] == '=') {
                tokenType = LY_OP_RSUB;
                //LyToken_append(root, LyToken_new(LY_OP_RSUB, NULL));
                i++;
            } else {
                tokenType = LY_OP_SUB;
                //LyToken_append(root, LyToken_new(LY_OP_SUB, NULL));
            }
        } else if (input[i] == '*') {
            if (input[i+1] == '=') {
                tokenType = LY_OP_RMUL;
                //LyToken_append(root, LyToken_new(LY_OP_RMUL, NULL));
                i++;
            } else if (input[i+1] == '/') {
                printf("Syntax error: mismatched end paren");
                i++;
            } else {
                tokenType = LY_OP_MUL;
                //LyToken_append(root, LyToken_new(LY_OP_MUL, NULL));
            }
        } // '/' related tokens are resolved near the top (since they are involved with comments)
       /* else if (input[i] == '(') {
            tokenType = LY_BLOCK_BEGIN;
            //LyToken_append(root, LyToken_new(LY_BLOCK_BEGIN, NULL));
        } else if (input[i] == ')') {
            tokenType = LY_BLOCK_END;
            //LyToken_append(root, LyToken_new(LY_BLOCK_END, NULL));
        } else if (input[i] == '(') {
            tokenType = LY_GROUP_BEGIN;
            //LyToken_append(root, LyToken_new(LY_GROUP_BEGIN, NULL));
        } else if (input[i] == ')') {
            tokenType = LY_GROUP_END;
            //LyToken_append(root, LyToken_new(LY_GROUP_END, NULL));
        }*//* else if (input[i] == ' ' || input[i] == '\t') {
            tokenType = LY_NULL;
        } else {
            lastBufferUpdate = i;
            bufferLen++;
            if (input[i] == '.') {
                bufferHasFloat = true;
            } 
            if (input[i] > '0' || input[i] < '9') {
                bufferIsNum = false;
                if ((input[i] < 'a' || input[i] > 'z') && (input[i] < 'A' && input[i] > 'Z') && input[i] != '_') {
                    bufferIsAlphaNum = false;
                }
            }
        }
        if (lastBufferUpdate != i) {
            if (bufferLen > 0) {
                bool bufferIsInt = input[lastBufferUpdate] == 'i';
                if (bufferIsAlphaNum) {
                    LyToken_append(root, LyToken_new(LY_NAME, LyName_newFromSlice(input, lastBufferUpdate - bufferLen, lastBufferUpdate)));
                } else if (bufferHasFloat || !bufferIsInt) {
                    LyToken_append(root, LyToken_new(LY_FLOAT, LyNumber_newFromStringSlice(input, lastBufferUpdate - bufferLen, lastBufferUpdate)));
                } else {
                    LyToken_append(root, LyToken_new(LY_INT, LyNumber_newFromStringSlice(input, lastBufferUpdate - bufferLen, lastBufferUpdate)));
                }
            }
        }
        if (tokenType != LY_NULL) {
            LyToken_append(root, LyToken_new(tokenType, tokenData));
        }
        i++;
    }
    return root;
}*/



