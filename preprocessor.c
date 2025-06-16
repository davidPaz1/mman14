#include "global.h"
#include "scan.h"
#include "util.h"
#include "error.h"
#include "macroTable.h"
#include "preprocessor.h"
/* .as -> .am -> .ob , .ext , .ent*/
/*page 15 or 31 for algorithm for preprocessing */


ErrCode executePreprocessor(char *inputFileName) {
    FILE* asFile, *amFile;
    char* line, errorContext[MAX_ERROR_MSG_LENGTH] = ""; /* buffer for error context */
    ErrCode errorCode = NULL_INITIAL; /* Initialize error code */
    macroTable* table = NULL;
    macroBody* body = NULL;
    Bool isMacroDefLine = FALSE; /* flag to indicate if the current line is a macro definition line */

    /*
    macroTable *macroTable = NULL;
    macroBody *macroBody = NULL;
    */
    /* Open the .as file for reading and .am file for writing */
    asFile = openFile(inputFileName, ".as", "r", &errorCode);
    if (errorCode != SCAN_SUCCESS) {
        sprintf(errorContext, "while opening file %s.as", inputFileName);
        printErrorMsg(errorCode, errorContext);  /* print the error message */
        return errorCode; /* return the error code */
    }

    amFile = openFile(inputFileName, ".am", "w", &errorCode);
    if (errorCode != SCAN_SUCCESS) {
        sprintf(errorContext, "while opening file %s.am", inputFileName);
        printErrorMsg(errorCode, errorContext);  /* print the error message */
        fclose(asFile);
        return errorCode; /* return the error code */
    }

    /* need to add preprocessing algorithm */
    while (errorCode != EOF_REACHED) {
        line = readLine(asFile, &errorCode); /* read a line from the .as file */
        
        if (errorCode == EOF_REACHED) 
            break; /* end of file reached, exit the loop */
            
        if (line == NULL) { /* check if the line is NULL */
            printErrorMsg(errorCode, "while reading line from .as file"); /* print the error message */
            freeFilesAndMemory(table, body, asFile, amFile, line);
            return PREPROCESSOR_FAILURE; /* return failure if an error occurred */
        }

        fputs(line, amFile); /* write the line to the .am file */
        fputc('\n', amFile); /* add a newline character after the line */
        free(line); /* free the line memory */
    }
    freeFilesAndMemory(table, body, asFile, amFile, line); /* free all allocated memory and close files */
    return PREPROCESSOR_SUCCESS; /* return success */
}

Bool isMacroDef(char *line) {
    char* firstWord = getFirstWord(line); /* get the first word of the line */
    if( firstWord != NULL && strcmp(firstWord, "mcro") == 0) {
        cutnChar(line, strlen(firstWord)); /* cut the first word from the line for processing */
        return TRUE; /* if the line is "mcro", it is a macro definition line */
    }
    return FALSE;
}

Bool isMacroEndLine(char *line)
{
    char* firstWord = getFirstWord(line); /* get the first word of the line */
    if( firstWord != NULL && strcmp(firstWord, "mcroend") == 0) {
        return TRUE; /* if the line is "mcroend", it is a macro end line */
    }
    return FALSE;
}

Bool isMacroUse(char *line)
{
    /* check if the line is a macro use line */
    return TRUE;
}

void freeFilesAndMemory(macroTable* table, macroBody* body, FILE* asFile, FILE* amFile, char* line)
{
    if (body != NULL) {
        freeMacroBody(body); /* free the macro body if it was created */
    }
    if (table != NULL) {
        freeMacroTable(table); /* free the macro table if it was created */
    }
    if (line != NULL) {
        free(line);
    }
    if (asFile != NULL)
        fclose(asFile); /* close the .as file */
    if (amFile != NULL)
        fclose(amFile); /* close the .am file */
}
