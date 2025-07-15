#include "global.h"
#include "secondPass.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"
#include "util.h"

ErrCode executeSecondPass(FILE* amFile, MacroTable* macroTable, SymbolTable* symbolTable, ErrorList* errorList){
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    parsedLine *pLine; /* parsed line structure to hold the line and its type */

    errorList->currentLine = 0; /* reset the current line number */

    while (errorCode != EOF_REACHED_S) {
        if (errorList->fatalError) /* check if there was a fatal error in previous iterations */
            return SECOND_PASS_FAILURE_S;
        errorList->currentLine++;

        pLine = readParsedLine(amFile, &errorCode, macroTable, errorList); /* read a line from the .as file */

    } /* end of while loop */

    if (errorList->count > 0) /* if there were any errors during the second pass */
        return SECOND_PASS_FAILURE_S; /* return failure if there were any errors */

    return SECOND_PASS_SUCCESS_S; /* return success if no errors occurred */
}

void secPassFreeMemory(parsedLine *pLine)
{
    freeParsedLine(pLine); /* free the parsed line structure */
}