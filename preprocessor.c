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
    Bool inMacroDef = FALSE; /* flag to indicate if the current line is a macro definition line */

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
    table = createMacroTable(&errorCode);
    if (errorCode != MACROTABLE_SUCCESS) { /* check if the macro table was created successfully */
        printErrorMsg(errorCode, "while creating macro table"); /* print the error message */
        freeFilesAndMemory(table, asFile, amFile, NULL); /* free all allocated memory and close files */
        return errorCode; /* return failure if an error occurred */
    }
    while (errorCode != EOF_REACHED) {
        line = readLine(asFile, &errorCode); /* read a line from the .as file 1 */
        
        if (errorCode == EOF_REACHED) 
            break; /* end of file reached, exit the loop */
            
        if (errorCode != SCAN_SUCCESS) { /* check if an error occurred while reading the line */
            printErrorMsg(errorCode, "while reading line from .as file"); /* print the error message */
            freeFilesAndMemory(table, asFile, amFile, line);
            return errorCode; /* return failure if an error occurred */
        }

        
        /* identify type of line and */

        if((errorCode = isMacroUse(table ,line)) != PREPROCESSOR_FAILURE) { /* check if the line is a macro use line 2 */
            if (errorCode != PREPROCESSOR_SUCCESS) { /* if the line is not a macro use line */
                printErrorMsg(errorCode, "while checking if line is macro use"); /* print the error message */
                freeFilesAndMemory(table, asFile, amFile, line); /* free all allocated memory and close files */
                return errorCode; /* return failure if an error occurred */
            }
            /* put lines from macro table into the .am file */
        }
        else if ((errorCode = isMacroDef(line)) != PREPROCESSOR_FAILURE) { /* check if the line is a macro definition line 3 */
            if (errorCode != PREPROCESSOR_SUCCESS) { /* if the line is not a macro definition line */
                printErrorMsg(errorCode, "while checking if line is macro definition"); /* print the error message */
                freeFilesAndMemory(table, asFile, amFile, line); /* free all allocated memory and close files */
                return errorCode; /* return failure if an error occurred */
            }
            inMacroDef = TRUE; /* set the flag to indicate that we are in a macro definition 4 */

            /* set the new macro into the macro table */
        }
        else if (inMacroDef) { /* if we are in a macro definition 6 */
            
            /*put line in macro table*/

        }
        else if ((errorCode = isMacroEndLine(line)) != PREPROCESSOR_FAILURE) { /* check if the line is a macro end line 7 */
            if (errorCode != PREPROCESSOR_SUCCESS) { /* if the line is not a macro end line */
                printErrorMsg(errorCode, "while checking if line is macro end"); /* print the error message */
                freeFilesAndMemory(table, asFile, amFile, line); /* free all allocated memory and close files */
                return errorCode; /* return failure if an error occurred */
            }
            inMacroDef = FALSE; /* reset the flag to indicate that we are no longer in a macro definition 8 */

            /* some more code */

            continue; /* skip the rest of the loop and continue to the next line */
        }
        else
        fputs(line, amFile); /* write the line to the .am file */
        fputc('\n', amFile); /* add a newline character after the line */
        free(line); /* free the line memory */
    }
    freeFilesAndMemory(table, asFile, amFile, line); /* free all allocated memory and close files */
    return PREPROCESSOR_SUCCESS; /* return success */
}

ErrCode isMacroDef(char *line) {
    ErrCode errCode = NULL_INITIAL; /* reset error code to initial state */
    char* firstWord = getFirstWord(&line, &errCode);  /* get the first word of the line */
    if (errCode != SCAN_SUCCESS)  /* check if an error occurred while getting the first word */
        return errCode; /* return the error code */
    
    if(strcmp(firstWord, "mcro") == 0) {
        cutnChar(line, strlen(firstWord)); /* cut the first word from the line for processing */
        errCode = PREPROCESSOR_SUCCESS;
    }
    else 
        errCode = PREPROCESSOR_FAILURE; /* if the first word is not "mcro", return failure */
    free(firstWord);
    return errCode; /* if the line is "mcro", it is a macro definition line */
}

ErrCode isMacroEndLine(char *line)
{
    ErrCode errCode = NULL_INITIAL; /* reset error code to initial state */
    char* firstWord = getFirstWord(&line, &errCode); /* get the first word of the line */
    if (errCode != SCAN_SUCCESS) /* check if an error occurred while getting the first word */
        return errCode; /* return the error code */
    
    if(strcmp(firstWord, "mcroend") == 0) 
        errCode = PREPROCESSOR_SUCCESS; /* if the line is "mcroend", it is a macro end line */
    else 
        errCode = PREPROCESSOR_FAILURE;
    free(firstWord);
    return errCode;
}

ErrCode isMacroUse(macroTable* table, char *line)
{
    ErrCode errCode = NULL_INITIAL; /* reset error code to initial state */
    char* firstWord = getFirstWord(&line, &errCode); /* get the first word of the line */
    if (errCode != SCAN_SUCCESS) /* check if an error occurred while getting the first word */
        return errCode; /* return the error code */

    if (isMacroExists(table, firstWord)) 
        errCode = PREPROCESSOR_SUCCESS;
    else 
        errCode = PREPROCESSOR_FAILURE;
    
    free(firstWord);
    return errCode;
}

char* getMacroName(char *line, ErrCode *errorCode) /* get the macro name from the .as file */
{
    char* firstWord = getFirstWord(&line, errorCode); /* get the first word of the line */
    if (*errorCode != SCAN_SUCCESS) /* check if an error occurred while getting the first word */
        return NULL; /* return NULL if an error occurred */

    return firstWord;
}


void freeFilesAndMemory(macroTable* table, FILE* asFile, FILE* amFile, char* line)
{
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
