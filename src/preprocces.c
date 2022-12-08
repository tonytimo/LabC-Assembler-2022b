
#include <stdio.h>
#include <string.h>
#include "errorHandler.h"
#include "constants.h"
#include "structs.h"
#include "assembler.h"

char* checkLine(FILE *input);

bool inMacro = false;
macro m[10];
int macroIndex = 0;
char cmd[LINE_MAX_LEN];

MFLAG preprocessor(FILE *src){
    char *p1;
    FILE *fp;
    char *temp;
    char name[100];

    strcpy(name, "../");
    copyStr(&temp, currFileName);
    temp[strlen(temp) - 3] = '\0';
    strcat(temp, ".am");
    fp = fopen(strcat(name, temp), "w");

    while (true){
        p1 = checkLine(src); /*checking each line for macros*/
        if (strcmp(p1,"stop") == 0){ /*checking for EOF*/
            break;
        }
        fprintf(fp, "%s",p1);/*writing each row to the new file.as without macros*/
    }
    fclose(fp);/*closing the file.as*/

    if(m[0].name[0] == '\0'){/*checking if we found macros*/
        if(remove(name) == 0){/*if not remove the new file.as*/
            return NO_CHANGE;
        }
    }
    return MACRO_SUCCESS;
}

char* checkLine(FILE *input){
    char temp[LINE_MAX_LEN];
    char arg[LINE_MAX_LEN];
    int i;

    if ((fgets(cmd, LINE_MAX_LEN, input)) == NULL) {
        return "stop";
    }
    sscanf(cmd , "%s %s", temp, arg);

    if(inMacro){
        /* end macro encountered */
        if(strcmp(temp, "endmacro") == 0){
            inMacro = false;
            macroIndex++;
            return "";
        }
        if(m[macroIndex].content[0] == '\0'){
            strcpy(&m[macroIndex].content[0], cmd);
            return "";
        }else{
            strcpy(&m[macroIndex].content[strlen(m[macroIndex].content)], cmd);
            return "";
        }
    }

    /* start macro encountered*/
    if(strcmp(temp, "macro") == 0){
        inMacro = true;
        strcpy(&m[macroIndex].name[0], arg);
        return "";
    }
    for (i = 0;  i<=10 ; i++) {
        if(m[i].name[0] == '\0'){
            break;
        }
        if(strcmp(temp,m[i].name) == 0){
            return m[i].content;
        }
    }
    return cmd;
}
