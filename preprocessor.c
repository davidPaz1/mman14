#include "preprocessor.h"
#include "global.h"
#include "error.h"
#include "lexer.h" /* for isLabel */
#include "util.h"
#include "tables.h"

/* * executePreprocessor - main function for the preprocessor.
 * it reads the .as file, processes macro definitions and uses, and writes to the .am file.
 * it also handles errors and adds them to the error list.
 * Returns PREPROCESSOR_SUCCESS_S on success, PREPROCESSOR_FAILURE_S on failure.
 */
ErrCode executePreprocessor(macroTable *table, ErrorList *errorList, FILE *asFile, FILE *amFile, const char *inputFileName) {
    char *line, *firstToken; /* line to read from the .as file and first token of the line */
    ErrCode errorCode = NULL_INITIAL; /* Initialize error code */
    Bool inMacroDef = FALSE; /* flag to indicate if the current line is a macro definition line */
    errorList->currentLine = 0; /* reset the current line number */

    while (errorCode != EOF_REACHED_S) {

        if (errorList->fatalError) /* check if there was a fatal error in previous iterations */
            return PREPROCESSOR_FAILURE_S;
        
        errorList->currentLine++;

        line = readLine(asFile, &errorCode); /* read a line from the .as file 1 */
        if (errorCode == EOF_REACHED_S)
            break; /* end of file reached, exit the loop */

        if (errorCode != UTIL_SUCCESS_S) { /* check if an error occurred while reading the line */            
            addErrorToList(errorList, errorCode);
            /* all other errors we didn't check in readLine() are fatal */
            return PREPROCESSOR_FAILURE_S; 
            
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

        if(errorCode == END_OF_LINE_S){ /* if the line is empty or contains only whitespace */
            fputs(line, amFile);
            fputc('\n', amFile); /* write the empty line to the .am file */
            free(line);
            free(firstToken);
            continue; /* skip to the next line */
        }
        
        if (errorCode != UTIL_SUCCESS_S) { /* check if an error occurred in getFirstToken() */
            addErrorToList(errorList, errorCode);
            preprocessorFreeMemory(line, NULL); /* clean up and exit the preprocessor */
            /* all other errors we didn't check in getFirstToken() are fatal */
            return PREPROCESSOR_FAILURE_S; 
        }

        if(isMacroExists(table, firstToken)) { /* check if the line is a macro use line 2 */
            errorCode = spreadMacro(table, firstToken, amFile); /* spread the macro body into the .am file */
            if (errorCode != MACROTABLE_SUCCESS_S)  /* check if the macro was spread successfully */
                addErrorToList(errorList, errorCode);
        }
        else if (strcmp(firstToken, "mcro") == 0) { /* check if the line is a macro definition line 3 */
            cutnChar(line, strlen(firstToken)); /* cut the first word from the line for processing */
            errorCode = macroDef(table, line); /* add the macro definition to the macro table */
            if (errorCode != MACROTABLE_SUCCESS_S) /* check if the macro definition was added successfully */
                addErrorToList(errorList, errorCode); /* add the error to the error list */
            else
                inMacroDef = TRUE; /* set the flag to indicate that we are in a macro definition 4 */
        }
        else if (strcmp(firstToken, "mcroend") == 0) { /* check if the line is a macro end line 7 */
            if (!inMacroDef) /* if we are not in a macro definition */
                addErrorToList(errorList, UNMATCHED_MACRO_END_E); /* add an error to the error list */
            inMacroDef = FALSE; /* reset the flag to indicate that we are no longer in a macro definition 8 */
        }
        else if (inMacroDef) { /* if we are in a macro definition 6 */
            errorCode = addMacroLine(table, line); /* add the line to the macro body */
            if (errorCode != MACROTABLE_SUCCESS_S)  /* check if the line was added successfully */
                addErrorToList(errorList, errorCode); /* add the error to the error list */
        }
        else { /* if the line is just a regular line unrelated to macros */
            fputs(line, amFile); /* write the line to the .am file */
            fputc('\n', amFile); /* add a newline character after the line */
        }

        preprocessorFreeMemory(line, firstToken); /* free the line memory */

    } /* end of while loop */

    if(errorList->count > 0) /* if there were any errors during the preprocessing */
        return PREPROCESSOR_FAILURE_S;
    
    return PREPROCESSOR_SUCCESS_S;
}

ErrCode spreadMacro(macroTable *table, const char *macroName, FILE *amFile)
{
    macroBody *macroBody, *next; /* will hold the body of the macro we are about to spread */
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    
    macroBody = findMacro(table, macroName, &errorCode); /* find the macro body in the table */
    if (errorCode != MACROTABLE_SUCCESS_S)
        return errorCode; /* return failure if an error occurred */
    
    while (macroBody != NULL) { /* iterate through the macro body */
        next = macroBody->nextLine; /* save the next line */
        fputs(macroBody->line, amFile); /* write the current line to the .am file */
        fputc('\n', amFile); /* write the empty line to the .am file */
        macroBody = next; /* move to the next line */
    }

    return MACROTABLE_SUCCESS_S; /* return success */
}

ErrCode macroDef(macroTable* table, char* line) /* add a line to the macro body */
{
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    char* macroName = cutFirstToken(line, &errorCode); /* get the macro name from the line */
    if (errorCode != UTIL_SUCCESS_S) { /* check if an error occurred while getting the macro name */
        return errorCode; /* return failure if an error occurred */
    }

    if (!isEndOfLine(line)) { /* if there are some extraneous text after the macro name */
        free(macroName); /* free the macro name memory */
        return EXTRANEOUS_TEXT_E;
    }

    errorCode = addMacro(table, macroName); /* if the line is not empty, add it to the macro body */
    if (errorCode != MACROTABLE_SUCCESS_S) { /* check if the line was added successfully */
        free(macroName);
        return errorCode; 
    }
    free(macroName);
    return MACROTABLE_SUCCESS_S;
}

void preprocessorFreeMemory(char *line, char *firstToken)
{
    if (line != NULL) {
        free(line);
    }
    if (firstToken != NULL) {
        free(firstToken);
    }
}
