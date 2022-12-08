#include "cmdTable.h"
#include "dataTable.h"
#include "assembler.h"
#include "converters.h"
#include "symbolStructs.h"
#include <string.h>

/* Prototypes */
static bool saveObj(FILE *file, int *i);
static bool saveData(FILE *file, int *i);
static bool saveExt(FILE *file, int *i);
static bool saveEnt(FILE *file, int *i);
char* nameMod(char* arr, char* name, char* ext);

/* Saves the files for successful assembling */
void saveFiles(void) {
    char *name;
    char nameWEx[100];
    FILE *out;
    bool keep;
    int i = MEMORY_START;

    copyStr(&name, currFileName);

    nameMod(nameWEx , name, OBJ_EXT);
    out = fopen(nameWEx, "w");
    keep = saveObj(out, &i);
    fclose(out);
    if (!keep)
        remove(name);

    nameMod(nameWEx, name, EXT_EXT);
    keep = true;

    out = fopen(nameWEx, "w");
    keep = saveExt(out, &i);
    fclose(out);
    if (!keep)
        remove(name);

    keep = true;
    nameMod(nameWEx, name, ENT_EXT);
    out = fopen(nameWEx, "w");
    keep = saveEnt(out, &i);
    fclose(out);
    if (!keep)
        remove(name);
    free(name);
}

/* Saves command to the file.as */
static bool saveObj(FILE *file, int *i) {
    cmdNode* cmd_node = NULL;
    int start = *i, cmdAmount = 0;
    cmd_node = cmd_list.head;
    if (cmd_node != NULL) {
        while (cmd_node != NULL) { /* Get the length because the .length is not actually the length but the address by now */
            cmd_node = cmd_node->next;
            cmdAmount++;
        }
    }
    fprintf(file, "%s\t%s\n", convertDecimalTo32((cmdAmount)),
            convertDecimalTo32((data_list.length)));
    for (cmd_node = cmd_list.head; cmd_node != NULL;
         cmd_node = cmd_node->next, (*i)++) {
        fprintf(file, "%s\t%s\n", convertDecimalTo32((*i)),
                convertDecimalTo32((cmd_node->value.print)));
    }
    if (saveData(file, i))
        return true;

    if (start == *i)
        return false;
    else
        return true;
}

/* Saves data to the file.as */
bool saveData(FILE *file, int *i) {
    dataNode* data_node = NULL;
    int start = *i;

    for (data_node = data_list.head; data_node != NULL;
         data_node = data_node->next, (*i)++) {
        fprintf(file, "%s\t%s\n", convertDecimalTo32((*i)),
                convertDecimalTo32((data_node->value)));
    }
    if (start == *i)
        return false;
    else
        return true;
}

/* Saves externals to the file.as */
bool saveExt(FILE *file, int *i) {
    char *name;
    int num;
    bool hasContent = false;

    while ((name = popExt(&num))) {
        fprintf(file, "%s\t%s\n", name, convertDecimalTo32((num)));
        free(name);
        hasContent = true;
    }
    return hasContent;
}

/* Saves entries to the file.as */
bool saveEnt(FILE *file, int *i) {
    char *name;
    int num;
    bool hasContent = false;

    while ((name = popEnt(&num))) {
        fprintf(file, "%s\t%s\n", name, convertDecimalTo32((num)));
        hasContent = true;
    }
    return hasContent;
}

char* nameMod(char* arr,char* name, char* ext){
    int i = 0;
    while(i <= strlen(arr)){
        arr[i] = '\0';
        i++;
    }
    if(!(strstr(name,".") == NULL)){
        name[strlen(name) - strlen(INPUT_EXT)] = '\0';
    }
    strcpy(arr, "../");
    strcat(arr, name);
    strcat(arr, ext);
    return arr;
}