#include "global.h"
#include "secondPass.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"
#include "util.h"

ErrCode executeSecondPass(FILE* amFile, CodeWord* instructionImage, MacroTable* macroNames, SymbolTable* symbolTable, ErrorList* errorList){
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    parsedLine *pLine; /* parsed line structure to hold the line and its type */
    unsigned int ic = 0; /* instruction counter */
    errorList->currentLine = 0; /* reset the current line number */

    while (errorCode != EOF_REACHED_S) {
        if (errorList->fatalError) /* check if there was a fatal error in previous iterations */
            return SECOND_PASS_FAILURE_S;

        errorList->currentLine++;

        pLine = readParsedLine(amFile, &errorCode, macroNames, errorList); /* read a line from the .as file */
        if (errorCode == EOF_REACHED_S)
            break;

        if (errorCode == LEXER_FAILURE_S) /* if reading the line failed */
            continue; /* continue to the next line */

        if (pLine->typesOfLine == DIRECTIVE_LINE && strcmp(pLine->lineContentUnion.directive.directiveName, ".entry") == 0) {
            SymbolNode* symbol = findSymbol(symbolTable, pLine->lineContentUnion.directive.directiveLabel);
            if (symbol == NULL) {
                addErrorToList(errorList, ENTRY_LABEL_DOES_NOT_EXIST_E); /* add error if the label does not exist */
            } else {
                symbol->isEntry = TRUE; /* mark the symbol as an entry */
                symbolTable->haveEntry = TRUE; /* set the flag that there is an entry symbol */
            }
        }
        else if (pLine->typesOfLine == INSTRUCTION_LINE) {
            errorCode = parseLabelOperandsValid(pLine, symbolTable, errorList);
            if (errorCode != LEXER_SUCCESS_S) {
                freeParsedLine(pLine); /* free the parsed line structure */
                continue; /* skip to the next line if there was an error */
            }
            secPassInstructionLine(pLine, &ic, instructionImage, errorList); /* process the instruction line */
        }
        
        /* if the line is a directive empty or a comment, we skip it */
        freeParsedLine(pLine); /* free the parsed line structure */
    } /* end of while loop */

    if (errorList->count > 0) /* if there were any errors during the second pass */
        return SECOND_PASS_FAILURE_S; /* return failure if there were any errors */

    return SECOND_PASS_SUCCESS_S; /* return success if no errors occurred */
}

void secPassInstructionLine(parsedLine *pLine, unsigned int *ic, CodeWord* instructionImage, ErrorList* errorList)
{
    /* This function is not implemented yet, so we return immediately */
    
    return;
}
