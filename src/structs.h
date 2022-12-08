
#ifndef TEST_STRUCTS_H
#define TEST_STRUCTS_H
#include "constants.h"

/* Structure of an operation (command) word */
typedef struct {
    unsigned int decode 	: DECODE_SIZE;
    unsigned int destOperand: OPERAND_SIZE;
    unsigned int srcOperand : OPERAND_SIZE;
    unsigned int opcode 	: OPCODE_SIZE;
} cmd_word;

/* Structure of an argument word */
typedef struct {
    unsigned int decode: DECODE_SIZE;
    unsigned int value : VALUE_SIZE;
} arg_word;


typedef struct {
    unsigned int decode 	: DECODE_SIZE;
    unsigned int destOperand: REG_OPERAND_SIZE;
    unsigned int srcOperand : REG_OPERAND_SIZE;
} reg_word;

typedef union {
    cmd_word cmd;
    reg_word reg;
    arg_word num;
    unsigned int print :10;
} word;

typedef struct{
    char *label;
    int cmd;
    char **args;
    bool isEffectless;
    bool isEOF;
}input_line;

typedef struct {
    char name[15];
    char content[200];
} macro;

#endif 
