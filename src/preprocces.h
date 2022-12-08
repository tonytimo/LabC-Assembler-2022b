

#ifndef TEST_PREPROCCES_H
#define TEST_PREPROCCES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "structs.h"

extern FLAG flag;
FLAG preprocessor(FILE *src);
char* checkLine(FILE *input);

#endif
