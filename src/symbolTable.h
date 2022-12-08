
#ifndef TEST_SYMBOLTABLE_H
#define TEST_SYMBOLTABLE_H

#include "constants.h"

typedef struct {
    char *labelName;
    int address;
    bool isExt;
    bool isOp;
} label;


bool addLabel(char *labelName, int address, bool isExt, bool isOp);
void printSymbolTable(void);/* This function is for debug only */
void freeSymbolTable(void);
label *getLabel(const char *name);
bool addExt(char *name, int address);
char *popExt(int *address);
bool addEnt(label *lbl);
char *popEnt(int *address);

#endif 
