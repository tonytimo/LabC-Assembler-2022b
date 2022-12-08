
#ifndef TEST_ASSEMBLER_H
#define TEST_ASSEMBLER_H

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "constants.h"
#include "structs.h"
#include "errorHandler.h"

extern FLAG flag;
extern int ic;
extern int dc;

/* Main assembler */
FLAG assembler(FILE *sourceFile);
input_line *getLine(FILE *src);
void freeLine(input_line *line);
bool isValidLabel(char *);
bool isValidReg(char* registerStr);
bool copyStr(char **dest, const char *src);
int stc_delim(char* p);

/* First run functions */
FLAG firstRun(FILE *sourceFile);
void relocate(int paddin);


/* Second run functions */
FLAG secondRun(FILE *);
bool strToInt(const char *str, int *dest);

#endif
