#include "preprocessor.h"
#include "global.h"
#include "error.h"
#include "lexer.h" /* for isLabel */
#include "util.h"
#include "tables.h"

ErrCode executePreprocessor(char *inputFileName, macroTable *macroNames) {
    FILE *asFile, *amFile;
    char *line, *firstToken; /* line to read from the .as file and first token of the line */
    ErrCode errorCode = NULL_INITIAL; /* Initialize error code */
    macroTable* table; /* the table of all the macros found */
    Bool inMacroDef = FALSE; /* flag to indicate if the current line is a macro definition line */

    /* Open the .as file for reading and .am file for writing */
    asFile = openFile(inputFileName, ".as", "r", &errorCode);
    if (errorCode != SCAN_SUCCESS) 
        return errorCode; /* return the error code */
    

    amFile = openFile(inputFileName, ".am", "w", &errorCode);
    if (errorCode != SCAN_SUCCESS) {
        fclose(asFile);
        return errorCode; /* return the error code */
    }

    /* need to add preprocessing algorithm */
    table = createMacroTable(&errorCode);
    if (errorCode != MACROTABLE_SUCCESS) { /* check if the macro table was created successfully */
        PreprocessorErrorExit(table, asFile, amFile, NULL, inputFileName); /* clean up and exit the preprocessor */
        return errorCode; /* return failure if an error occurred */
    }
    
    while (errorCode != EOF_REACHED) {
        line = readLine(asFile, &errorCode); /* read a line from the .as file 1 */
        if (errorCode == EOF_REACHED)
            break; /* end of file reached, exit the loop */
            
        if (errorCode != SCAN_SUCCESS) { /* check if an error occurred while reading the line */
            PreprocessorErrorExit(table, asFile, amFile, line, inputFileName); /* clean up and exit the preprocessor */
            return errorCode; /* return failure if an error occurred */
        }

        firstToken = getFirstToken(line, &errorCode); /* get the first token from the line */
        
        /* note preprocessor doesnt check if token is a valid label, it just checks if it is a label 
        and if it is - it writes it to the .am file */
        if(isLabel(firstToken)) { /* check if the first token is a label */
            fputs(firstToken, amFile); /* write the label to the .am file as it is */
            fputc(' ', amFile); /* add a space character after the label */
            cutnChar(line, strlen(firstToken)); /* cut the label from the line */
            free(firstToken); /* free the first token memory */
            firstToken = getFirstToken(line, &errorCode);
        }

        if(errorCode == END_OF_LINE){ /* if the line is empty or contains only whitespace */
            fputs(line, amFile);
            fputc('\n', amFile); /* write the empty line to the .am file */
            free(line);
            free(firstToken);
            continue; /* skip to the next line */
        }
        
        if (errorCode != SCAN_SUCCESS) { /* check if an error occurred while getting the first token */
            PreprocessorErrorExit(table, asFile, amFile, line, inputFileName); /* clean up and exit the preprocessor */
            return errorCode; /* return failure if an error occurred */
        }
        /* identify type of line and */

        if(isMacroExists(table, firstToken)) { /* check if the line is a macro use line 2 */
            errorCode = spreadMacro(table, firstToken, amFile); /* spread the macro body into the .am file */
            if (errorCode != MACROTABLE_SUCCESS) { /* check if the macro was spread successfully */
                PreprocessorErrorExit(table, asFile, amFile, line, inputFileName); /* clean up and exit the preprocessor */
                return errorCode; /* return failure if an error occurred */
            }

        }
        else if (strcmp(firstToken, "mcro") == 0) { /* check if the line is a macro definition line 3 */
            inMacroDef = TRUE; /* set the flag to indicate that we are in a macro definition 4 */
            cutnChar(line, strlen(firstToken)); /* cut the first word from the line for processing */
            errorCode = macroDef(table, line); /* add the macro definition to the macro table */
            if (errorCode != MACROTABLE_SUCCESS) { /* check if the macro definition was added successfully */
                PreprocessorErrorExit(table, asFile, amFile, line, inputFileName); /* clean up and exit the preprocessor */
                return errorCode; /* return failure if an error occurred */
            }
            
        }
        else if (strcmp(firstToken, "mcroend") == 0) { /* check if the line is a macro end line 7 */
            if (!inMacroDef) { /* if we are not in a macro definition */
                PreprocessorErrorExit(table, asFile, amFile, line, inputFileName); /* clean up and exit the preprocessor */
                free(firstToken); /* free the first token memory */
                return UNMATCHED_MACRO_END; /* return failure if an error occurred */
            }
            inMacroDef = FALSE; /* reset the flag to indicate that we are no longer in a macro definition 8 */
        }
        else if (inMacroDef) { /* if we are in a macro definition 6 */
            errorCode = addMacroLine(table, line); /* add the line to the macro body */
            if (errorCode != MACROTABLE_SUCCESS) { /* check if the line was added successfully */
                PreprocessorErrorExit(table, asFile, amFile, line, inputFileName); /* clean up and exit the preprocessor */
                return errorCode; /* return failure if an error occurred */
            }
        }
        else { /* if the line is just a regular line unrelated to macros */
            fputs(line, amFile); /* write the line to the .am file */
            fputc('\n', amFile); /* add a newline character after the line */
        }

        free(line); /* free the line memory */
        free(firstToken); /* free the first token memory */

    } /* end of while loop */

    preprocessorFreeMemory(table, asFile, amFile, line); /* free all allocated memory and close files */
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
        fputs(macroBody->line, amFile); /* write the current line to the .am file */
        fputc('\n', amFile); /* write the empty line to the .am file */
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

void PreprocessorErrorExit(macroTable *table, FILE *asFile, FILE *amFile, char *line, char *fileName)
{
    if (fileName != NULL) { /* check if the file name is NULL */
        ErrCode errorCode = delFile(fileName, ".am"); /* delete the .am file if it exists */
        if (errorCode != SCAN_SUCCESS) { /* check if the file was deleted successfully */
            char errorContext[MAX_ERROR_MSG_LENGTH] = ""; /* buffer for error context */
            sprintf(errorContext, "while deleting file %s.am", fileName); /* set the error context */
            printErrorMsg(errorCode, errorContext); /* print the error message */
        }
    }
    preprocessorFreeMemory(table, asFile, amFile, line); /* free all allocated memory and close files */
}

void preprocessorFreeMemory(macroTable *table, FILE *asFile, FILE *amFile, char *line)
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
