#include "global.h"
#include "preprocessor.h"
#include "firstPass.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"

int main(int argc, char const *argv[])
{
    ErrCode errcode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    char* inputFileName = "test1"; /* input file name */
    int DCF = 0, ICF = 0;
    macroTable* macroNames = malloc(sizeof(macroTable)); /* macro table to store macro names */

    printf("argc: %d\n", argc);
    if (argc >= 2) {
        printf("argv: %s\n", argv[1]);
        inputFileName = (char*) argv[1]; /* use the provided file name */
    }
    
    printf("Input file name: %s\n", inputFileName); /* print the input file name */
    printf("Starting preprocessor...\n"); /* print start message */
    errcode = executePreprocessor(inputFileName, macroNames); /* test with a sample file name */
    if (errcode != PREPROCESSOR_SUCCESS) {
        printErrorMsg(errcode, "\nwhile executing preprocessor"); /* print the error message */
        return 0; /* return the error code */
    } 
    printf("\nPreprocessor executed successfully.\n"); /* print success message */
    
    printf("Starting first pass...\n");
    errcode = executeFirstPass(inputFileName, &DCF, &ICF, macroNames); /* execute the first pass */
    if (errcode != FIRSTPASS_SUCCESS) {
        printErrorMsg(errcode, "\nwhile executing first pass"); /* print the error message */
        return 0; /* return the error code */
    }
    printf("\nFirst pass executed successfully.\n"); /* print success message */
    
    freeMacroTable(macroNames); /* free the macro table */
    printf("Exiting program...\n"); /* print exit message */
    return 0;
}
