
#include "assembler.h"
#include "errorHandler.h"

FLAG flag; /* 0 for Success, non-zero for failure */
int ic; /* Instructions Counter */
int dc; /* Data Counter */

/* Prototypes */
static int getNextArg(char *src, char *dest);
static int getOp(const char **ops, const char *str, int opsAmount);
/* End prototypes */

/* Gets a line of code */
input_line *getLine(FILE *input) {
    const char *ops[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea",
                         "inc", "dec", "jmp", "bne", "get", "prn", "jsr", "rts", "hlt", /* Ops until here */
                         ".data", ".string", ".struct", ".extern",
                         ".entry"}; /* Instructions *//* The order of these command stay identical to the order of the enum constants in constants.h so the index will match the enum value */
    char cmdStr[LINE_MAX_LEN];
    char tmpStr[LINE_MAX_LEN];
    char *p1, *p2;
    input_line *line;
    int length; /* Temporary length */
    int strIndex = 0; /* Index for next read in the current line */
    int i, status = 2;
    bool oneSpace = false;

    /* Mem initialisation */
    if (!(line = malloc(sizeof(input_line)))) {
       error(sprintf(errMsg, OUT_OF_RAM));
        freeLine(line);
        return NULL;
    }
    line->cmd = -1;
    line->label = NULL;
    line->isEffectless = false;
    line->isEOF = false;
    line->args = malloc(sizeof(char *) * MAX_ARG_COUNT);

    /* Set all cells to NULL */
    for (i = 0; i < MAX_ARG_COUNT; i++){
        line->args[i] = NULL;
    }


    /* EOF encountered */
    if ((fgets(cmdStr, LINE_MAX_LEN, input)) == NULL) {
        line->isEOF = true;
        return line;
    }
    /* Checking for comments or effect less lines */
    if (strlen(cmdStr) == 0 || sscanf(cmdStr , "%s", tmpStr) == 0 || tmpStr[0] == COMMENT_SIGN || cmdStr[0] == '\r'){
        line->isEffectless = true;
        free(line->args);
        line->args = NULL;
        return line;
    }

    /* Removing white spaces */
    p1 = cmdStr;
    p2 = tmpStr;
    for (i = 0; i < strlen(cmdStr); i++) {
        if(*p1 == SPACE && oneSpace){
            p1++;
            continue;
        }
        if(*p1 == SPACE){
            oneSpace = true;
        }
        if(oneSpace && !(*p1 == SPACE)){
            oneSpace = false;
        }
        *p2 = *p1;
        p1++;
        p2++;
    }
    strcpy(cmdStr,tmpStr);

    /* Recognises the label */
    sscanf(cmdStr, "%s", tmpStr);
    if(tmpStr[strlen(tmpStr)-1] == LABEL_DELIM){
        tmpStr[strlen(tmpStr)-1] = '\0';
        if(!(isValidLabel(tmpStr))){
            error(sprintf(errMsg,ILLEGAL_LABEL));
            freeLine(line);
            return NULL;
        }
        copyStr(&(line->label), tmpStr);
        strIndex += strlen(line->label) + 2;

        if (!sscanf((cmdStr + strIndex), "%s", tmpStr)) { /* Read the next word for the operator recognition */
            error(sprintf(errMsg, SYNTAX_ERROR MISSING_OP));
            freeLine(line);
            return NULL;
        }
    }else{
        line -> label = NULL;
    }

    /* Recognises the operator */
    if ((line->cmd = getOp(ops, tmpStr, sizeof(ops) / sizeof(ops[0]))) == -1) {
        freeLine(line);
        return NULL;
    } else{
        if(strcmp(tmpStr, cmdStr + strIndex) == 0){
            strIndex += strlen(tmpStr);
        }
        else{
            strIndex += strlen(tmpStr) + 1;
        }
    }
    if(line->cmd >= DOT_EXTERN){
        free(line->label);
        line->label = NULL;
    }

    /* Getting the arguments */
    if(!(line->cmd == 14 || line->cmd == 15)){
        i = 0;
        if (!(status = getNextArg(cmdStr + strIndex, tmpStr))) {
            free(line->args);
            line->args = NULL;
        } else if (status == -1){
            freeLine(line);
            return NULL;
        }
        else {
            i++;
            if (!(copyStr(&(line->args[0]), tmpStr))) {
                return line;
            }
            while(status == 2){
                if (!(status = getNextArg(NULL, tmpStr))) {
                    free(line->args);
                    line->args = NULL;
                } else if (status == -1) {
                    freeLine(line);
                    return NULL;
                }
                    if (!(copyStr(&(line->args[i]), tmpStr))) {
                        return line;
                    }
                    if(status == 1){
                        line->args[i+1] = NULL;
                        break;
                    }
                    i++;
                }
            }
        }
    /* End arguments section */
    return line;

}

/* Release all allocated mem from struct */
void freeLine(input_line *line) {
    int i;
    if (!line)
        return;
    free(line->label);
    if (line->args != NULL)/* Release args */
        for (i = 0; line->args[i] != NULL; i++)
            free(line->args[i]);
    free(line->args);
    free(line);
}/* End freeLine */

/* Checking for valid register arg */
bool isValidReg(char* reg){
    if(reg[0] == 'r' && reg[1] >= '0' && reg[1] <= '7' && reg[2] == '\0'){
        return true;
    }
    return false;
}

/* Checking for valid register label */
bool isValidLabel(char* label){
    int i;
    if(isValidReg(label)){
        return false;
    }
    if(!(isalpha(label[0]))){
        return false;
    }
    for (i = 0; i < strlen(label); i++) {
        if((!isalnum(label[i]))){
            return false;
        }
    }
    return true;
}

/* Getting the operation part */
static int getOp(const char **ops, const char *str, const int opsAmount) {
    int i;
    for (i = 0; i < opsAmount; i++){
        if (strcmp(str, ops[i]) == 0){
            return i;
        } /* Found the op */
    }
    error(sprintf(errMsg, SYNTAX_ERROR UNKNOWN_OP));
    return -1;
}

/* Gets the next argument from the current line */
static int getNextArg(char *src, char *dest) {
    static char *cmdStr;
    int inStr = 0, i;
    if (src != NULL) {
        cmdStr = src;
    }
    while (*cmdStr == SPACE){
        cmdStr++;
    }
    if (*cmdStr == '\0'){
        return 0;
    }
    for (i = 0; *cmdStr != ARG_SEPERATOR && *cmdStr != '\0' && *cmdStr != '\r'; cmdStr++) {
        if (inStr) {
            if (*cmdStr == STR_DELIM) {
                inStr = 0;
                dest[i] = *cmdStr;
                i++;
                break;
            }
            else if (isspace(*cmdStr)){
                break;
            }
        }
        else if (*cmdStr == STR_DELIM) {
            inStr = 1;
        }
        dest[i] = *cmdStr;
        i++;
    }
    dest[i] = '\0';

    if (i == 0) {
        error(sprintf(errMsg, SYNTAX_ERROR EMPTY_ARG));
        return 0;
    }
    while (*cmdStr == SPACE){
        cmdStr++;
    }

    switch (*cmdStr) {
        case '\0':
            return 1;
        case ARG_SEPERATOR:
            cmdStr++;
            if (*cmdStr == '\0' || *cmdStr == '\n') {
                error(sprintf(errMsg, SYNTAX_ERROR EMPTY_ARG));
                return -1;
            }
            return 2;
        case '\r':
            return 1;
    }
    return 1;
}/* End getNextArg */

/* Copies the text from src to *dest */
bool copyStr(char **dest, const char *src) {
    int strIndex = 0;
    if (!(*dest = malloc(strIndex = (strlen(src) + 1)))) {
        error(sprintf(errMsg, OUT_OF_RAM));
        return false;
    }
    strcpy(*dest, src);
    return true;
}
/* Converts str to int */
bool strToInt(const char *str, int *dest){
    int num;
    char *end;
    num = strtol(str, &end, 10);
    if (strcmp(end, "") == 0) {
        *dest = num;
        return true;
    }
    return false;
}

int stc_delim(char* p){
    int len = strlen(p);
    int x = 0;
    while(*p != '\0') {
        p++;
    }
    while (*p != '.' && x != len){
        x++;
        p--;
    }
    if(x == len){
        return 0;
    }
    return x;
}