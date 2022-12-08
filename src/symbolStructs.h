
#ifndef TEST_SYMBOLSTRUCTS_H
#define TEST_SYMBOLSTRUCTS_H

#include "symbolTable.h"

/* Structures */
typedef struct labelNode {
    label *this;
    struct labelNode *next;
} labelNode;

typedef struct {
    labelNode *head;
} labelList;

typedef struct externNode {
    struct {
        char *labelName;
        int address;
    }this;
    struct externNode *next;
} externNode;

typedef struct entryNode {
    struct {
        char *labelName;
        int address;
    }this;
    struct entryNode *next;
} entryNode ;

typedef struct {
    externNode *head;
} externList;

typedef struct {
    entryNode *head;
} entryList;

extern labelList symbol_list;

#endif 
