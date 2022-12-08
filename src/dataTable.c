#include "dataTable.h"
#include "cmdTable.h"
#include "constants.h"
#include "math.h"
#include "converters.h"

/*############STATIC VARIABLES#############*/
dataList data_list = { NULL, 0 };

/* Creates a data node with a dynamic allocated storage */
static dataNode* createDataNode(int data) {
    dataNode* newDataNode = NULL;
    if ((newDataNode = malloc(sizeof(dataNode)))) {
        newDataNode->value = data;
        newDataNode->next = NULL;
    }

    return newDataNode;
}


/*  Adds data (char or int) to data list */
int addData(unsigned int data) {
    dataNode *curr, *newNode;
    data_list.length++;
    if ((newNode = createDataNode(data % (int) pow(2, WORD_SIZE)))
        && cmd_list.length + data_list.length < MAX_MEMORY_SIZE) {
        if (data_list.head == NULL) { /*list is empty*/
            data_list.head = newNode;
        } else {
            /* Skip to the end of the list */
            for (curr = data_list.head; curr->next != NULL; curr = curr->next)
                ;
            curr->next = newNode;
        }
        return true;
    }
    return false;
}
/* reset table */
void resetDataTable(void) {
    dataNode* curr = NULL;
    data_list.length = 0;
    while ((curr = data_list.head) != NULL) { /* set curr to head, stop if list empty. */
        data_list.head = data_list.head->next; /* advance head to next element. */
        free(curr); /* delete saved pointer. */
    }
}
/* print table */
void printDataTable(int lineNumber) {
    dataNode* curr = NULL;
    for (curr = data_list.head; curr != NULL; curr = curr->next, lineNumber++) {
        printf("%d\t%s\n", lineNumber,
               convertDecimalTo32(curr->value));
    }
}