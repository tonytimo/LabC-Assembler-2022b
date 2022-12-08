
#include <string.h>
#include <stdlib.h>
#include "assembler.h"
#include "symbolTable.h"
#include "cmdTable.h"
#include "dataTable.h"
#include "saveFiles.h"
#include "converters.h"
#include "preprocces.h"

char *currFileName;

int main(int argc, char* argv[]) {
    int i;
    char name[100];
    FILE *current;
    strcpy(name, "../");
    if (argc < 2) {
        fprintf(stderr, TOO_LESS_FILES);
        return EXIT_FAILURE;
    } else {
        for (i = 1; i < argc; i++) {
            currFileName = malloc(strlen(argv[i]) + EXT_LEN + 1);
            sprintf(currFileName, "%s%s", argv[i], INPUT_EXT);
            if ((current = fopen(strcat(name, currFileName), "r")) != NULL) { /* Opens the file.as */
                if (preprocessor(current) == NO_CHANGE) {/* Checking for macros */
                    if (assembler(current) == ASM_FAILURE) {
                        fprintf(stderr, ASSEMBLER_ERR, argv[i], INPUT_EXT);
                    } else {
                        fprintf(stdout, ASSEMBLER_SUCCESS, argv[i], INPUT_EXT);
                    }
                    fclose(current); /* Closes the file.as */
                }else{
                    fclose(current);
                    name[strlen(name)-3] = '\0';
                    if ((current = fopen(strcat(name,".am"), "r")) != NULL){
                        if (assembler(current) == ASM_FAILURE) {
                            fprintf(stderr, ASSEMBLER_ERR, argv[i], INPUT_EXT);
                        } else {
                            fprintf(stdout, ASSEMBLER_SUCCESS, argv[i], INPUT_EXT);
                        }
                        fclose(current); /* Closes the file.as */
                    }else {
                        fprintf(stderr, "Invalid file.as name or invalid arg path please read README.txt\n");/* Here comes error handler for invalid input file.as */
                    }
                }
            }else {
                fprintf(stderr, "Invalid file.as name or invalid arg path please read README.txt\n");/* Here comes error handler for invalid input file.as */
            }
            free(currFileName);
        }
            return EXIT_SUCCESS;
        }
    }



/* Main assembler function. */
FLAG assembler(FILE *src) {
    flag = ASM_SUCCESS;

    firstRun(src);
    if (flag == ASM_SUCCESS)
        secondRun(src);
    if (flag == ASM_SUCCESS){
       saveFiles();
    }

    resetCmdTable();
    resetDataTable();
    freeSymbolTable();
    return flag;
}
