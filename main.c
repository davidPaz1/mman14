#include "global.h"
#include "preprocessor.h"
#include "firstPass.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"
#include "util.h"

void fatalErrorExit(macroTable* macroNames, ErrorList* errorList, ErrCode errCode, FILE* asFile, FILE* amFile);
void freeExecutedFile(macroTable* macroNames, ErrorList* errorList); /* free the macro table and error list */
void executeFile(char* fileName); /* main function to execute the file */

int main(int argc, char const *argv[])
{
    char* inputFileName = NULL;
    int i;
    printf("argc: %d\n", argc);
    if (argc < 2) { /* check if there are no arguments */
        printf("No input files provided. will run with default file name 'test.as'\n");
        inputFileName = "test";
        executeFile(inputFileName); /* execute the default file */
        printf("\ndone with file %s \n\n\n", inputFileName); /* print a new line for better readability */
        return 0; /* exit if no arguments are provided */
    }
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

void executeFile(char* fileName)
{
    ErrCode errCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    FILE *asFile = NULL, *amFile = NULL; /* file pointers for the assembly and macro files */
    int DCF = 0, ICF = 0;
    macroTable* macroNames = createMacroTable(); /* create a macro table to hold all the macros found */
    ErrorList* errorList = createErrorList(fileName); /* create an error list to hold errors */
    if (errorList == NULL || macroNames == NULL) { /* check if memory allocation was successful */
        printErrorMsg(MALLOC_ERROR_F, "preprocessor", 0);
        return; /* exit if memory allocation fails */
    }
    errorList->stage = "preprocessor";
    

    /* Open the .as file for reading and .am file for writing */
    asFile = openFile(fileName, ".as", "r", &errCode);
    if(errCode == FILE_READ_ERROR_F) 
        errCode = INPUT_FILE_UNREADABLE_F;

    if (errCode != UTIL_SUCCESS_S){
        fatalErrorExit(macroNames, errorList, errCode, NULL, NULL); /* exit if an error occurred */
        return; 
    }
    printf("Starting preprocessor...\n");  /* print start message */


    amFile = openFile(fileName, ".am", "w", &errCode);
    if (errCode != UTIL_SUCCESS_S) {
        fatalErrorExit(macroNames, errorList, errCode, asFile, NULL); /* exit if an error occurred */
        return;
    }

    errCode = executePreprocessor(macroNames, errorList, asFile, amFile, fileName); /* test with a sample file name */
    if (errCode == PREPROCESSOR_FAILURE_S) {
        printf("PREPROCESSOR FAILURE returned\n");
        fatalErrorExit(macroNames, errorList, errCode, asFile, amFile); /* exit if an error occurred */
        return; /* exit if an error occurred */
    }
    if (errCode != PREPROCESSOR_SUCCESS_S) { 
        printErrors(errorList, 1); /* print the errors if any */
        printErrorMsg(errCode, NULL, 0); /* print the error message */
        return;
    }
    printf("\nPreprocessor executed successfully.\n"); /* print success message */
    
    printf("Starting first pass...\n");
    errorList->stage = "first pass";
    errCode = executeFirstPass(fileName, &DCF, &ICF, macroNames); /* execute the first pass */
    if (errCode != FIRSTPASS_SUCCESS_S) {
        printErrorMsg(errCode, "first pass", 0); /* print the error message */
        return;
    }
    printf("\nFirst pass executed successfully.\n"); /* print success message */
    
    freeExecutedFile(macroNames, errorList); /* free the macro table and error list */
    printf("successfully executed file %s\n", fileName); 
}

void fatalErrorExit(macroTable* macroNames, ErrorList* errorList, ErrCode errCode, FILE* asFile, FILE* amFile) {
    printErrors(errorList, 1); /* print the errors if any */
    printErrorMsg(errCode, errorList->stage, 0); /* print the error message (if not NULL) */

    freeExecutedFile(macroNames, errorList); /* free the macro table and error list */
}

void freeExecutedFile(macroTable* macroNames, ErrorList* errorList) {
    freeMacroTable(macroNames); /* free the macro table */
    freeErrorsList(errorList); /* free the error list */
}
