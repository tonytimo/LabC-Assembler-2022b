
#include <string.h>
#include "assembler.h"
#include "dataTable.h"
#include "cmdTable.h"
#include "symbolTable.h"
#include "errorHandler.h"
#include <math.h>

static bool addressHandler(input_line*);
static addressing getArgWord(const char *str, word *word1, word *word2);

FLAG secondRun(FILE *src) {
    input_line *line;
    int lineIndex;
    fseek(src, 0L, SEEK_SET);
    relocate(cmd_list.length);
    for (lineIndex = 1; true; lineIndex++) { /* Runs through all the lines. */
        if ((line = getLine(src))) {
            if (line->isEOF) {
                freeLine(line);
                break; /* End when EOF is encountered */
            }
            if (line->isEffectless) {
                continue;
            }
            if (!addressHandler(line)) {
                if (cmd_list.length + data_list.length > MAX_MEMORY_SIZE) {
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
}/* End secondRun */

/* Analyses a line of code and operates accordingly */
static bool addressHandler(input_line *line) {
    switch (line->cmd) {
        /* Two operands group */
        case MOV:
        case CMP:
        case ADD:
        case SUB:
        case LEA:
            if (line->args != NULL && line->args[1] != NULL && line->args[2] == NULL) { /* Two args */
                addressing srcAdders, destAdders;
                word srcArg, destArg, extraWord1, extraWord2;
                /* Handle source arg  */
                switch (srcAdders = getArgWord(line->args[0], &srcArg, &extraWord1)) {
                    case REG:
                        srcArg.reg.srcOperand = srcArg.reg.destOperand;
                        srcArg.reg.destOperand = 0;
                        break;
                    case IMD:
                    case REC:
                        if (line->cmd == LEA) {
                            error(sprintf(errMsg, WRONG_ARG_FOR_FUNC));
                            return false;
                        }
                        break;
                    case DIR:
                        break;
                    default:
                        return true;
                }
                /* handle dest arg  */
                switch (destAdders = getArgWord(line->args[1], &destArg,&extraWord2)) {
                    case IMD:
                        if (line->cmd != CMP) {
                            error(sprintf(errMsg, WRONG_ARG_FOR_FUNC));
                            return false;
                        }
                    case DIR:
                    case REG:
                        break;
                    case REC:
                    default:
                        return true;
                }

                addCmd(ABS, destAdders, srcAdders, line->cmd);
                if (destAdders == REG && srcAdders == REG ) {
                    word multiReg;
                    multiReg.reg.destOperand = destArg.reg.destOperand;
                    multiReg.reg.srcOperand = srcArg.reg.srcOperand;
                    multiReg.reg.decode = ABS;
                    addArg(multiReg);
                } else {
                    if (srcAdders == DIR && srcArg.num.value == 0) {/* If the arg is external */
                        if (!addExt(line->args[0], getCmdLength() + 1)) {
                            return false;
                        }
                    }
                    addArg(srcArg);
                    if(srcAdders == REC){
                        addArg(extraWord1);
                    }else if(destAdders == REC){
                        addArg(extraWord2);
                    }
                    if (destAdders == DIR && destArg.num.value == 0) {/* If the arg is external */
                        if (!addExt(line->args[1], getCmdLength() + 1)) {
                            return false;
                        }
                    }
                    addArg(destArg);
                }
            } else {
                error(sprintf(errMsg, WRONG_ARG_COUNT));
                return false;
                /* Expected 2 arg error */
            }
            break;
            /* One operand's group */
        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case GET:
        case PRN:
        case JSR:
            if (line->args != NULL && line->args[1] == NULL) { /* Only one arg*/
                addressing adders;
                word arg , extraArg;
                switch (adders = getArgWord(line->args[0], &arg ,&extraArg)) {
                    case IMD:
                        if (line->cmd != PRN) {
                            error(sprintf(errMsg, WRONG_ARG_FOR_FUNC));
                            return false;
                        }
                    case DIR:
                    case REG:
                        addCmd(ABS, adders, IMD, line->cmd);
                        if (adders == DIR && arg.num.value == 0) {/* If the arg is external */
                            if (!addExt(line->args[0], getCmdLength() + 1)) {
                                return false;
                            }
                        }
                        addArg(arg);
                        break;
                    case REC:
                    default:
                        return false;
                }
            } else {
                error(sprintf(errMsg, WRONG_ARG_COUNT));
                return false;
                /* Expected 1 arg error */
            }
            break;
            /* No operands group */
        case RTS:
        case HLT:
            if (line->args[0] == NULL) { /*no args*/
                addCmd(0, IMD, IMD, line->cmd);
            } else {
                error(sprintf(errMsg, WRONG_ARG_COUNT));
                return false;
            }
            break;

        case DOT_ENTRY:
            if (line->args != NULL && line->args[1] == NULL) {
                label* entLabel;
                if ((entLabel = getLabel(line->args[0])) != NULL) {
                    if (!(addEnt(entLabel))) {
                        return false;
                    }
                } else {
                    error(sprintf(errMsg, ENT_TO_UNDEF, line->args[0]));
                    return false;
                }
            }
            break;
        case DOT_DATA:
        case DOT_STRING:
        case DOT_STRUCT:
        case DOT_EXTERN:
            /* do nothing */
            break;
        default:
            /* Not a command error. Probably impossible to reach. */
            error(sprintf(errMsg, UNKNOWN_ERR));
            return false;
    }
    return true;
}/* End addressHandler */

/* finding the addressing */
static addressing getArgWord(const char *str, word *word1, word *word2) {
    int num;
    char *tempStr;
    label *lbl;

    if(str[0] == '#'){
        if (!strToInt(str + 1, &num)) {
            error(sprintf(errMsg, SYNTAX_ERROR UNKNOWN_ARG_TYPE));
            return -1;
        }
        word1->num.decode = ABS;
        word1->num.value = num % (int) pow(2, VALUE_SIZE);
        return IMD;
    }
    else if(str[0] == 'r'){
        if (isValidReg(str)) {/* Is register name */
            word1->reg.decode = ABS;
            word1->reg.destOperand = str[1] - '0';
            word1->reg.srcOperand = 0;
            return REG;
        }
    }
    else if (isValidLabel(str)) {/* Is label name */
        if (!(lbl = getLabel(str))) {
            error(sprintf(errMsg, UNKNOWN_LABEL, str));
            return -1;
        }
        word1->num.value = lbl->address;
        word1->num.decode = lbl->isExt ? EXT : RLC;
        return DIR;
    }

    copyStr(&tempStr,str);
    tempStr[strlen(tempStr)- stc_delim(str)] = '\0';
    if(isValidLabel(tempStr)){
        if(str[strlen(tempStr)] == '.' && str[strlen(tempStr)+1] >= '0' && str[strlen(tempStr)+1] <= '9'){
            if (!(lbl = getLabel(tempStr))) {
                error(sprintf(errMsg, UNKNOWN_LABEL, str));
                return -1;
            }
            word1->num.value = lbl->address;
            word1->num.decode = lbl->isExt ? EXT : RLC;
            word2 ->num.value = strToInt(&str[strlen(str)],&num);
            word2->num.decode = 00;
            return REC;
        }
    }
    error(sprintf(errMsg, SYNTAX_ERROR INVALID_ARG, str));/* Syntax error */
    return -1;
}
