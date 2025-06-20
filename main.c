#include "global.h"
#include "macroTable.h" /* test only */
#include "preprocessor.h"
#include "scan.h"
#include "error.h"

int main(int argc, char const *argv[])
{
    int ec = 0; /* for gdb */
    ErrCode errcode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    char* inputFileName = "test1"; /* input file name */
    printf("%dStarting preprocessor...\n", ec); /* ec so warning unused variable won't appear */
    printf("argc: %d\n", argc);
    if (argc >= 2) {
        printf("argv: %s\n", argv[1]);
        inputFileName = (char*) argv[1]; /* use the provided file name */
    }

    errcode = executePreprocessor(inputFileName); /* test with a sample file name */
    if (errcode != PREPROCESSOR_SUCCESS) /* check if the preprocessor executed successfully */
        printErrorMsg(errcode, "\nwhile executing preprocessor"); /* print the error message */
    else
        printf("\nPreprocessor executed successfully.\n"); /* print success message */
        
    return 0;
}

/* test main function for macroTable */
/*
int main(int argc, char const *argv[])
{
    char* macroName = malloc(32 * sizeof(char));
    char* line = malloc(64 * sizeof(char));
    if (macroName == NULL || line == NULL) {
        fprintf(stderr, "Memory allocation failed for macro name or line\n");
        return 1;
    }
    strcpy(macroName, "testMacro");
    strcpy(line, "This is a test line for the macro body.");


    macroTable* table = createMacroTable(macroName, line);
    if (table == NULL) {
        fprintf(stderr, "Failed to create macro table\n");
        return 1;
    }

    printf("Macro Table created with name: %s\n", table->head->macroName);
    printf("First line in macro body: %s\n", table->head->bodyHead->line);

    freeMacroTable(table);   
    printf("Macro Table freed successfully\n");
    free(macroName);        
    free(line);            
    
    return 0;
}

*/ 

/* test main function for base4 conversion
int main(int argc, char *argv[]){
    int input[BASE2_INSTRUCTION_LENGTH] = {0,0,0,1,1,0,1,1,2,9};
    char *base4_output;
    base4_output = base4(input);

    printf("Base 4 output: %s\n", base4_output);
    free(base4_output);
    return 0;
}
*/
