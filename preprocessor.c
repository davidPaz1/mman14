#include "global.h"
#include "scan.h"
#include "util.h"
#include "error.h"
#include "macroTable.h"
#include "preprocessor.h"
/* .as -> .am -> .ob , .ext , .ent*/
/*page 15 or 31 for algorithm for preprocessing */


ErrCode executePreprocessor(char *inputFileName) {
    int count = 0; /* for debugging purposes */
    FILE *asFile, *amFile;
    char *line, *firstToken, errorContext[MAX_ERROR_MSG_LENGTH] = ""; /* buffer for error context */
    ErrCode errorCode = NULL_INITIAL; /* Initialize error code */
    macroTable* table; /* the table of all the macros found */
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
        printf("Reading line %d\n", ++count); /* debug print */
        line = readLine(asFile, &errorCode); /* read a line from the .as file 1 */
        if (errorCode == EOF_REACHED)
            break; /* end of file reached, exit the loop */
            
        if (errorCode != SCAN_SUCCESS) { /* check if an error occurred while reading the line */
            printErrorMsg(errorCode, "while reading line from .as file"); /* print the error message */
            freeFilesAndMemory(table, asFile, amFile, line);
            return errorCode; /* return failure if an error occurred */
        }

        firstToken = getFirstToken(line, &errorCode); /* get the first token from the line */
        
        if(errorCode == TOKEN_IS_LABEL) {
            fputs(firstToken, amFile); /* write the line to the .am file as is */
            cutnChar(line, strlen(firstToken)); /* cut the label from the line */
            free(firstToken); /* free the first token memory */
            firstToken = getFirstToken(line, &errorCode);
        }

        if(errorCode == END_OF_LINE){ /* if the line is empty or contains only whitespace */
            writeLineFile(amFile, line); /* write the empty line to the .am file */
            free(line);
            free(firstToken);
            continue; /* skip to the next line */
        }
        
        if (errorCode != SCAN_SUCCESS) { /* check if an error occurred while getting the first token */
            printErrorMsg(errorCode, "while getting first token from line"); /* print the error message */
            freeFilesAndMemory(table, asFile, amFile, line); /* free all allocated memory and close files */
            return errorCode; /* return failure if an error occurred */
        }
        /* identify type of line and */

        if(isMacroExists(table, firstToken)) { /* check if the line is a macro use line 2 */
            printf("Spreading macro: %s\n", firstToken); /* debug print */
            errorCode = spreadMacro(table, firstToken, amFile); /* spread the macro body into the .am file */
            if (errorCode != MACROTABLE_SUCCESS) { /* check if the macro was spread successfully */
                printErrorMsg(errorCode, "while spreading macro"); /* print the error message */
                freeFilesAndMemory(table, asFile, amFile, line); /* free all allocated memory and close files */
                return errorCode; /* return failure if an error occurred */
            }

        }
        else if (strcmp(firstToken, "mcro") == 0) { /* check if the line is a macro definition line 3 */
            inMacroDef = TRUE; /* set the flag to indicate that we are in a macro definition 4 */
            cutnChar(line, strlen(firstToken)); /* cut the first word from the line for processing */
            errorCode = macroDef(table, line); /* add the macro definition to the macro table */
            if (errorCode != MACROTABLE_SUCCESS) { /* check if the macro definition was added successfully */
                printErrorMsg(errorCode, "while adding macro definition to macro table"); /* print the error message */
                freeFilesAndMemory(table, asFile, amFile, line); /* free all allocated memory and close files */
                return errorCode; /* return failure if an error occurred */
            }
            
        }
        else if (strcmp(firstToken, "mcroend") == 0) { /* check if the line is a macro end line 7 */
           inMacroDef = FALSE; /* reset the flag to indicate that we are no longer in a macro definition 8 */
        }
        else if (inMacroDef) { /* if we are in a macro definition 6 */
            errorCode = addMacroLine(table, line); /* add the line to the macro body */
            if (errorCode != MACROTABLE_SUCCESS) { /* check if the line was added successfully */
                printErrorMsg(errorCode, "while adding macro line to macro table"); /* print the error message */
                freeFilesAndMemory(table, asFile, amFile, line); /* free all allocated memory and close files */
                return errorCode; /* return failure if an error occurred */
            }
        }
        else { /* if the line is just a regular line unrelated to macros */
            writeLineFile(amFile, line); /* write the line to the .am file */
        }

        free(line); /* free the line memory */
        free(firstToken); /* free the first token memory */

    } /* end of while loop */

    freeFilesAndMemory(table, asFile, amFile, line); /* free all allocated memory and close files */
    return PREPROCESSOR_SUCCESS; /* return success */
}

ErrCode spreadMacro(macroTable *table, char *macroName, FILE *amFile)
{
    macroBody *macroBody, *next; /* will hold the body of the macro we are about to spread */
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    if (table == NULL || amFile == NULL)  /* check if the table or file is NULL */
        return UNEXPECTED_NULL_INPUT; /* set error code to unexpected NULL input */
    
    macroBody = findMacro(table, macroName, &errorCode); /* find the macro body in the table */
    if (errorCode != MACROTABLE_SUCCESS)
        return errorCode; /* return failure if an error occurred */
    
    while (macroBody != NULL) { /* iterate through the macro body */
        next = macroBody->nextLine; /* save the next line */
        writeLineFile(amFile, macroBody->line); /* write the line to the .am file */
        macroBody = next; /* move to the next line */
    }

    return MACROTABLE_SUCCESS; /* return success */
}

ErrCode macroDef(macroTable* table, char* line) /* add a line to the macro body */
{
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    char* macroName = cutFirstToken(line, &errorCode); /* get the macro name from the line */
    if (errorCode != SCAN_SUCCESS) { /* check if an error occurred while getting the macro name */
        return errorCode; /* return failure if an error occurred */
    }

    if (!isEndOfLine(line)) { /* if there are some extraneous text after the macro name */
        free(macroName); /* free the macro name memory */
        return EXTRANEOUS_TEXT;
    }

    errorCode = addMacro(table, macroName); /* if the line is not empty, add it to the macro body */
    if (errorCode != MACROTABLE_SUCCESS) { /* check if the line was added successfully */
        free(macroName); /* free the macro name memory */
        return errorCode; /* return failure if an error occurred */
    }
    free(macroName); /* free the macro name memory */
    return MACROTABLE_SUCCESS; /* return success */
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
