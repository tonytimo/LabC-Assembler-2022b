

#include "assembler.h"
#include "dataTable.h"
#include "cmdTable.h"
#include "symbolTable.h"

static bool handleLine(input_line*);

FLAG firstRun(FILE *src) {
    input_line *line;
    int lineIndex;
    fseek(src, 0L, SEEK_SET);
    for (lineIndex = 1; true; lineIndex++) { /* Runs through all the lines. */
        if ((line = getLine(src))) {
            if (line->isEOF) {
                freeLine(line);
                break; /* End when EOF is encountered */
            }
            if (line->isEffectless) {
                continue;
            }
            if (!handleLine(line)) {
                if (cmd_list.length + data_list.length >= MAX_MEMORY_SIZE) {
                    error(sprintf(errMsg, OUT_OF_STORAGE));
                    report(lineIndex);
                    return flag;
                }
                report(lineIndex);
            }
        } else {
            report(lineIndex);
        }
        freeLine(line);
    }
    return flag;
}/* End firstRun */

/* Analyses a line of code and operates accordingly */
static bool handleLine(input_line* line) {
    int i;
    int isOp = false;
    int isExt = false;
    int hasLabel = (line->label ? true : false);
    int wordCounter = 0;
    char temp[50];
    switch (line->cmd) {
        /* Two operands group */
        case MOV:
        case CMP:
        case ADD:
        case SUB:
        case LEA:
            if (line->args != NULL) {
                if (line->args[1] != NULL) {
                    if (!(isValidReg(line->args[0]) && isValidReg(line->args[1]))) { /* If not register to register*/
                        wordCounter++; /* Two extra words*/
                        strncpy(temp, line->args[0], strlen(line->args[0]) - stc_delim(line->args[0]));
                        temp[strlen(line->args[0])- stc_delim(line->args[0])] ='\0';
                        if (isValidLabel(temp)) {
                            if (line->args[0][strlen(temp)] == '.' &&
                                (line->args[0][strlen(temp) + 1] >= '0' &&
                                 line->args[0][strlen(temp) + 1] <= '9')) {
                                wordCounter++;
                            }
                        } else {
                            strncpy(temp, line->args[1], strlen(line->args[1]) - stc_delim(line->args[1]));
                            temp[strlen(line->args[1])- stc_delim(line->args[1])] ='\0';
                            if (isValidLabel(temp)) {
                                if (line->args[1][strlen(temp)] == '.' &&
                                    (line->args[1][strlen(temp) + 1] >= '0' &&
                                     line->args[1][strlen(temp) + 1] <= '9')) {
                                    wordCounter++;
                                }
                            }
                        }
                    }
                } else {
                        error(sprintf(errMsg, TOO_LESS_ARGS));
                        return false;
                        /* Expected 2 args get one error */
                    }
                } else {
                    error(sprintf(errMsg, TOO_LESS_ARGS));
                    return false;
                    /*Expected 2 args error */
                }
                /* no break */
                case NOT:/* One operand's group */
                case CLR:
                case INC:
                case DEC:
                case JMP:
                case BNE:
                case GET:
                case PRN:
                case JSR:
                    wordCounter++; /* One extra word*/
                /* no break */
                case RTS: /* No operands group */
                case HLT:
                    isOp = true;
                /* Count lines */
                if (hasLabel) {
                    if (!(addLabel(line->label, cmd_list.length, isExt, isOp))) {
                        return false;
                    }
                }
                increaseCmdLength(++wordCounter); /* Count number of rows in cmd table*/
                break;
                case DOT_DATA:
                    /* Save in data table and symbol table*/
                    if (line->args != NULL) {
                        if (hasLabel) {
                            if (!(addLabel(line->label, data_list.length, isExt, isOp))) {
                                return false;
                                /* Failed to add a label error */
                            }
                        }
                        i = 0;
                        while (line->args[i] != NULL) {
                            int num;
                            char *rst;
                            /* Convert string to int */
                            num = (int) strtol(line->args[i], &rst, 10);
                            i++;
                            if (strcmp(rst, "") == 0) { /* If no extra string*/
                                if (!(addData(num))) {
                                    error(sprintf(errMsg, OUT_OF_RAM));
                                    return false;
                                    /* Didn't success to addData error */
                                }
                            } else {
                                error(sprintf(errMsg, UNKNOWN_ARG_TYPE));
                                return false;
                                /* String mix with int error */
                            }
                        }
                    } else {
                        error(sprintf(errMsg, TOO_LESS_ARGS));
                        return false;
                        /* Expected at least 1 arg error */
                    }
                break;

                case DOT_STRING:
                    /* Save in data table and symbol table*/
                    if (line->args != NULL && line->args[1] == NULL) { /* Only one arg*/
                        char *c = line->args[0];
                        if (hasLabel) {
                            if (!(addLabel(line->label, data_list.length, isExt, isOp))) {
                                return false;/* Error is set by addLabel already */
                                /* Didn't success to add label error */
                            }
                        }
                        if (*c++ == STR_DELIM) {
                            while (*c != STR_DELIM) {
                                if (!(addData(*c++))) {
                                    error(sprintf(errMsg, OUT_OF_RAM));
                                    return false;
                                    /* Didn't success to add char error */
                                }
                            }
                            if (*++c != '\0') {
                                error(sprintf(errMsg, WRONG_STR_FORMAT));
                                return false;
                            }
                            if (!(addData('\0'))) { /* End of string*/
                                error(sprintf(errMsg, OUT_OF_RAM));
                                return false;
                                /* Didn't succeed to add char error */
                            }
                        } else {
                            error(sprintf(errMsg, WRONG_STR_FORMAT));
                            return false;
                            /* Not in a string format error */
                        }
                    } else {
                        error(sprintf(errMsg, WRONG_ARG_COUNT));
                        return false;
                        /* Expected 1 arg error */
                    }
                break;

                case DOT_STRUCT:
                    /* Save in data table and symbol table*/
                    if (line->args[0] != NULL && line->args[1] != NULL) { /* Only two args*/
                        int num;
                        char *rst;
                        char *c = line->args[1];
                        if (hasLabel) {
                            if (!(addLabel(line->label, data_list.length, isExt, isOp))) {
                                return false;/* Error is set by addLabel already */
                                /* Didn't success to add label error */
                            }
                        }
                        /* Convert string to int */
                        num = (int) strtol(line->args[0], &rst, 10);
                        if (strcmp(rst, "") == 0) { /* If no extra string*/
                            if (!(addData(num))) {
                                error(sprintf(errMsg, OUT_OF_RAM));
                                return false;
                                /* Didn't success to addData error */
                            }
                        }
                        if (*c++ == STR_DELIM) {
                            while (*c != STR_DELIM) {
                                if (!(addData(*c++))) {
                                    error(sprintf(errMsg, OUT_OF_RAM));
                                    return false;
                                    /* Didn't success to add char error */
                                }
                            }
                            if (!(addData('\0'))) { /* End of string*/
                                error(sprintf(errMsg, OUT_OF_RAM));
                                return false;
                                /* Didn't succeed to add char error */
                            }
                        } else {
                            error(sprintf(errMsg, WRONG_STR_FORMAT));
                            return false;
                            /* Not in a string format error */
                        }
                    } else {
                        error(sprintf(errMsg, WRONG_ARG_COUNT));
                        return false;
                        /* Expected 1 arg error */
                    }
                break;

                case DOT_EXTERN:
                    /* Save in symbol table*/
                    isExt = true;
                if (line->args[1] == NULL) { /* Only one arg*/
                    if (isValidLabel(line->args[0])) {
                        if (addLabel(line->args[0], 0, isExt, isOp)) {

                        } else {
                            return false;
                            /* Didn't success to add label error */
                        }
                    } else {
                        error(sprintf(errMsg, ILLEGAL_LABEL, line->args[0]));
                        return false;
                        /* Extern not a valid label error */
                    }
                } else {
                    error(sprintf(errMsg, TOO_LESS_ARGS));
                    return false;
                    /* Expected at least 1 arg error */
                }
                break;
                case DOT_ENTRY:
                    /* Do nothing */
                    break;
                default:
                    /* Not a command error. Probably impossible to reach. */
                    error(sprintf(errMsg, UNKNOWN_ERR));
                return false;
            }
            return true;
    }
/* End handleLine */
