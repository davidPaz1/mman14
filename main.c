#include "global.h"
#include "preprocessor.h"
#include "firstPass.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"
#include "util.h"


void freeExecutedFile(macroTable* macroNames, ErrorList* errorList); /* free the macro table and error list */
void executeFile(char* filename); /* main function to execute the file */

int main(int argc, char const *argv[])
{
    char* inputFileName = NULL;
    int i;
    printf("argc: %d\n", argc);
    if (argc >= 2) {
        printf("argv: %s\n", argv[1]);
    }

    for (i = 1; i < argc; i++) {
        inputFileName = strDup((char*)argv[i]); /* use the provided file name */
        printf("argv[%d]: %s\n", i, argv[i]); /* print all arguments */
        executeFile(inputFileName);
        printf("\ndone with file %s \n\n\n", inputFileName); /* print a new line for better readability */
    }


    return 0;
}

void executeFile(char* filename){
    ErrCode errcode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    int DCF = 0, ICF = 0;
    macroTable* macroNames = malloc(sizeof(macroTable)); /* allocate memory for the macro table */
    ErrorList* errorList = malloc(sizeof(ErrorList)); /* allocate memory for the error list */
    if (errorList == NULL || macroNames == NULL) { /* check if memory allocation was successful */
        printErrorMsg(MALLOC_ERROR_F, "Failed to allocate memory for error list or macro table");
        return; /* exit if memory allocation fails */
    }
    createErrorList(errorList, filename); /* create an error list to hold errors */
    printf("Starting preprocessor...\n"); /* print start message */
    errorList->stage = "preprocessor";
    errcode = executePreprocessor(filename, macroNames); /* test with a sample file name */
    if (errcode != PREPROCESSOR_SUCCESS_S) {
        printErrorMsg(errcode, "\nwhile executing preprocessor"); /* print the error message */
        /*printErrors(errorList, 0);*/
        return;
    } 
    printf("\nPreprocessor executed successfully.\n"); /* print success message */
    
    printf("Starting first pass...\n");
    errorList->stage = "first pass";
    errcode = executeFirstPass(filename, &DCF, &ICF, macroNames); /* execute the first pass */
    if (errcode != FIRSTPASS_SUCCESS_S) {
        printErrorMsg(errcode, "\nwhile executing first pass"); /* print the error message */
        return;
    }
    printf("\nFirst pass executed successfully.\n"); /* print success message */
    
    freeExecutedFile(macroNames, errorList); /* free the macro table and error list */
    printf("successfully executed file %s\n", filename); 
}

void freeExecutedFile(macroTable* macroNames, ErrorList* errorList) {
    freeMacroTable(macroNames); /* free the macro table */
    freeErrorsList(errorList); /* free the error list */
}