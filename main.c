#include "global.h"
#include "preprocessor.h"
#include "firstPass.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"
#include "util.h"

void freeFiles(FILE* asFile, FILE* amFile, FILE* obFile);
void freeTableAndLists(macroTable* macroNames, ErrorList* errorList); /* free the macro table and error list */
void executeAssembler(char* fileName); /* main function to execute the file */
void PreProcessorFileErrorExit(ErrorList* errorList, FILE* amFile);

int main(int argc, char const *argv[])
{
    char* inputFileName = NULL;
    int i;
    printf("argc: %d\n", argc);
    if (argc < 2) { /* check if there are no arguments */
        printf("No input files provided. will run with default file name 'test.as'\n\n");
        inputFileName = "test1";
        executeAssembler(inputFileName); /* execute the default file */
        printf("\ndone with file %s \n\n\n", inputFileName); /* print a new line for better readability */
        return 0; /* exit if no arguments are provided */
    }

    for (i = 1; i < argc; i++) {
        inputFileName = strDup((char*)argv[i]); /* use the provided file name */
        printf("argv[%d]: %s\n", i, argv[i]); /* print all arguments */
        executeAssembler(inputFileName);
        printf("\ndone with file %s \n\n\n", inputFileName); /* print a new line for better readability */
        free(inputFileName); /* free the input file name */
    }


    return 0;
}

void executeAssembler(char* fileName)
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
        printf("Error opening file %s.as: %s\n", fileName, getErrorMessage(errCode));
        freeTableAndLists(macroNames, errorList); /* free the macro table and error list */
        return; 
    }
    printf("Starting preprocessor...\n");  /* print start message */


    amFile = openFile(fileName, ".am", "w", &errCode);
    if (errCode != UTIL_SUCCESS_S) {
        printf("Error opening file %s.am: %s\n", fileName, getErrorMessage(errCode));
        freeTableAndLists(macroNames, errorList); /* free the macro table and error list */
        freeFiles(asFile, NULL, NULL);
    }

    errCode = executePreprocessor(macroNames, errorList, asFile, amFile, fileName); /* test with a sample file name */
    if (errCode == PREPROCESSOR_FAILURE_S) {
        PreProcessorFileErrorExit(errorList, amFile); /* clean up and exit the preprocessor */
        freeTableAndLists(macroNames, errorList); /* free the macro table and error list */
        freeFiles(asFile, amFile, NULL); /* close the files if they were opened */
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
    
    freeTableAndLists(macroNames, errorList); /* free the macro table and error list */
    freeFiles(asFile, amFile, NULL); /* close the files if they were opened */
    printf("successfully executed file %s\n", fileName); 
}

void PreProcessorFileErrorExit(ErrorList* errorList, FILE* amFile)
{
    ErrCode errorCode = delFile(errorList->filename, ".am"); /* delete the .am file if it was created */
    if (errorCode != UTIL_SUCCESS_S) { /* check if the file was deleted successfully */
        errorList->count++; /* increment the error count */
    }
    printErrors(errorList); /* print the errors if any */
    
    if (errorCode != UTIL_SUCCESS_S)
        printErrorMsg(errorCode, "preprocessor", 0); /* print the error message */
}

void freeTableAndLists(macroTable* macroNames, ErrorList* errorList) {
    freeMacroTable(macroNames); /* free the macro table */
    freeErrorsList(errorList); /* free the error list */
}

void freeFiles(FILE* asFile, FILE* amFile, FILE* obFile) {
    if (asFile != NULL) {
        fclose(asFile); /* close the file if it was opened */
    }
    if (amFile != NULL) {
        fclose(amFile); /* close the file if it was opened */
    }
    if (obFile != NULL) {
        fclose(obFile); /* close the file if it was opened */
    }
}
