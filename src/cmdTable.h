
#ifndef TEST_CMDTABLE_H
#define TEST_CMDTABLE_H

#include "assembler.h"

/*###############STRUCTURES#################*/
typedef struct cmdNode {
    word value;
    struct cmdNode* next;
} cmdNode;

typedef struct {
    cmdNode* head;
    int length;
} cmdList;

/*############EXTERN VARIABLES#############*/
extern cmdList cmd_list;
/*############FUNCTIONS####################*/

/* Adds command to cmd_list */
int addCmd(unsigned int decode, unsigned int destOperand, unsigned int srcOperand,
           unsigned int opcode);
/* Adds argument word to cmd_list */
int addArg(word arg);
/* return the length of cmd_list */
int getCmdLength(void);
/* increase the length of cmd_list by n */
void increaseCmdLength(int n);
/* reset data table */
void resetCmdTable(void);
/* print cmd table */
void printCmdTable(void);

#endif 
