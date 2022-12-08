
#ifndef TEST_DATATABLE_H
#define TEST_DATATABLE_H

#include <stdio.h>
#include <stdlib.h>

/*###############STRUCTURES#################*/
typedef struct dataNode {
    unsigned int value;
    struct dataNode* next;
} dataNode;

typedef struct {
    dataNode* head;
    int length;
} dataList;

/*############EXTERN VARIABLES#############*/
extern dataList data_list;
/*############FUNCTIONS####################*/
/*
 Adds data (char or int) to data list
 */
int addData(unsigned int);
/* return the length of cmd_list */
int getDataLength(void);
/* reset data table */
void resetDataTable(void);
/* print table */
void printDataTable(int lineNumber);

#endif 
