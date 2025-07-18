#include "global.h"
#include "secondPass.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"
#include "util.h"

ErrCode executeSecondPass(FILE* amFile, FILE* obFile, MacroTable* macroTable, SymbolTable* symbolTable, ErrorList* errorList){
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    parsedLine *pLine; /* parsed line structure to hold the line and its type */

    errorList->currentLine = 0; /* reset the current line number */

    while (errorCode != EOF_REACHED_S) {
        if (errorList->fatalError) /* check if there was a fatal error in previous iterations */
            return SECOND_PASS_FAILURE_S;

        errorList->currentLine++;

        pLine = readParsedLine(amFile, &errorCode, macroTable, errorList); /* read a line from the .as file */
        if (errorCode == EOF_REACHED_S)
            break;

        if (errorCode != LEXER_SUCCESS_S) { /* check if an error occurred while reading the line */
            freeParsedLine(pLine);
            return SECOND_PASS_FAILURE_S; /* return failure if an error occurred */
        }

        if (pLine->typesOfLine == EMPTY_LINE || pLine->typesOfLine == COMMENT_LINE) {
            freeParsedLine(pLine); /* free the parsed line structure */
            continue; /* skip empty or comment lines */
        }

    } /* end of while loop */

    if (errorList->count > 0) /* if there were any errors during the second pass */
        return SECOND_PASS_FAILURE_S; /* return failure if there were any errors */

    return SECOND_PASS_SUCCESS_S; /* return success if no errors occurred */
}





void writeEntryFile(FILE* entFile, SymbolTable* symbolTable) {

    /* this function is not implemented yet, so we return immediately */
    
    return;
}

void writeExternFile(FILE* extFile, SymbolTable* symbolTable) {

    /* this function is not implemented yet, so we return immediately */

    return;
}

void secPassFreeMemory(parsedLine *pLine)
{
    freeParsedLine(pLine); /* free the parsed line structure */
}