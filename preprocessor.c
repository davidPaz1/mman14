#include "preprocessor.h"
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "util.h"
#include "tables.h"

/* * executePreprocessor - main function for the preprocessor.
 * it reads the .as file, processes macro definitions and uses, and writes to the .am file.
 * it also handles errors and adds them to the error list.
 * Returns PREPROCESSOR_SUCCESS_S on success, PREPROCESSOR_FAILURE_S on failure.
 */
ErrCode executePreprocessor(MacroTable *macroTable, ErrorList *errorList, FILE *asFile, FILE *amFile, const char *inputFileName) {
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
            if (errorCode == LINE_TOO_LONG_E) /* if the line is too long, skip it and continue to the next line */
                continue; 
            /* all other errors we didn't check in readLine() are fatal */
            return PREPROCESSOR_FAILURE_S; 
        }

        firstToken = getFirstToken(line, &errorCode); /* get the first token from the line */    
        if(errorCode == END_OF_LINE_S){ /* if the line is empty or contains only whitespace */
            fputs(line, amFile);
            fputc('\n', amFile); /* write the empty line to the .am file */
            freeStrings(line, firstToken, NULL); /* free the memory allocated for the line and first token */
            continue; /* skip to the next line */
        }
        
        if (errorCode != UTIL_SUCCESS_S) { /* check if an error occurred in getFirstToken() */
            addErrorToList(errorList, errorCode);
            free(line); /* clean up and exit the preprocessor */
            /* all other errors we didn't check in getFirstToken() are fatal */
            return PREPROCESSOR_FAILURE_S; 
        }

        if(isMacroExists(macroTable, firstToken)) { /* check if the line is a macro use line 2 */
            cutnChar(line, strlen(firstToken)); /* cut the first word from the line for processing */
            if(!isEndOfLine(line)) /* if there are some extraneous text after the macro name */
                addErrorToList(errorList, EXTRANEOUS_TEXT_E);
            else /* if the line is just a macro use line */
                spreadMacro(macroTable, firstToken, amFile); /* spread the macro body into the .am file */
        }
        else if (isMacroDef(firstToken)) { /* check if the line is a macro definition line 3 */
            cutnChar(line, strlen(firstToken)); /* cut the first word from the line for processing */
            errorCode = macroDef(macroTable, line); /* add the macro definition to the macro table */
            if (errorCode != TABLES_SUCCESS_S) /* check if the macro definition was added successfully */
                addErrorToList(errorList, errorCode); /* add the error to the error list */
            else
                inMacroDef = TRUE; /* set the flag to indicate that we are in a macro definition 4 */
        }
        else if (isMacroEnd(firstToken)) { /* check if the line is a macro end line 7 */
            cutnChar(line, strlen(firstToken)); /* cut the first word from the line for processing */
            if (!isEndOfLine(line)) /* if there are some extraneous text after the mcroend */
                addErrorToList(errorList, EXTRANEOUS_TEXT_E); 
            else if (!inMacroDef) /* if we are not in a macro definition */
                addErrorToList(errorList, UNMATCHED_MACRO_END_E); /* add an error to the error list */
            else
                inMacroDef = FALSE; /* reset the flag to indicate that we are no longer in a macro definition 8 */
        }
        else if (inMacroDef) { /* if we are in a macro definition 6 */
            errorCode = addMacroLine(macroTable, line); /* add the line to the macro body */
            if (errorCode != TABLES_SUCCESS_S)  /* check if the line was added successfully */
                addErrorToList(errorList, errorCode); /* add the error to the error list */
        }
        else { /* if the line is just a regular line unrelated to macros */
            fputs(line, amFile); /* write the line to the .am file */
            fputc('\n', amFile); /* add a newline character after the line */
        }

        freeStrings(line, firstToken, NULL); /* free the line memory */

    } /* end of while loop */

    if(errorList->count > 0) /* if there were any errors during the preprocessing */
        return PREPROCESSOR_FAILURE_S;
    
    return PREPROCESSOR_SUCCESS_S;
}

void spreadMacro(MacroTable *macroTable, const char *macroName, FILE *amFile)
{
    MacroBody *macroBody = findMacro(macroTable, macroName); /* find the macro body in the table */
    /* macroBody cannot be NULL here because we checked if the macro exists before calling this function */

    while (macroBody != NULL) { /* iterate through the macro body */
        MacroBody *next = macroBody->nextLine; /* save the next line */
        fputs(macroBody->line, amFile); /* write the current line to the .am file */
        fputc('\n', amFile); /* write the empty line to the .am file */
        macroBody = next; /* move to the next line */
    }
}

ErrCode macroDef(MacroTable* macroTable, char* line) /* add a line to the macro body */
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

    errorCode = addMacro(macroTable, macroName); /* if the line is not empty, add it to the macro body */
    if (errorCode != TABLES_SUCCESS_S) { /* check if the line was added successfully */
        free(macroName);
        return errorCode; 
    }
    free(macroName);
    return TABLES_SUCCESS_S;
}
