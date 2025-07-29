#include "firstPass.h"
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "util.h"
#include "tables.h"

/* .am -> .ob , .ext , .ent*/
/* 17 has an explanation for the first pass */
/*page 19 or 31 for algorithm for preprocessing */

ErrCode executeFirstPass(FILE* amFile, DataWord dataImage[], unsigned int* DC, unsigned int* IC, MacroTable* macroNames, SymbolTable* symbolTable, ErrorList* errorList)
{
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    errorList->currentLine = 0; /* reset the current line number */

    /* table setup? */
    
    while (errorCode != EOF_REACHED_S) {
        parsedLine *pLine; /* parsed line structure to hold the line and its type */
        
        if (errorList->fatalError) /* check if there was a fatal error in previous iterations */
            return FIRSTPASS_FAILURE_S;
        
        pLine = readParsedLine(amFile, &errorCode, macroNames, errorList); /* read a line from the .as file 1 */
        if (errorCode == EOF_REACHED_S)
            break; /* end of file reached, exit the loop */
        
        if (errorCode == LEXER_FAILURE_S) /* if reading the line failed */
            continue; /* continue to the next line */

        if (pLine->typesOfLine == EMPTY_LINE || pLine->typesOfLine == COMMENT_LINE) {
            freeParsedLine(pLine); /* free the parsed line structure */
            continue; /* skip empty or comment lines */
        }

        if (pLine->typesOfLine == DIRECTIVE_LINE)
            firstPassDirectiveLine(pLine, DC, dataImage, symbolTable, errorList); /* handle directive lines */
        else if (pLine->typesOfLine == INSTRUCTION_LINE) 
            firstPassInstructionLine(pLine, IC, macroNames, symbolTable, errorList); /* parse the instruction line */
        
        freeParsedLine(pLine);
    } /* end of while loop */

    if(errorList->count > 0) /* if there are errors in the error list */
        return FIRSTPASS_FAILURE_S;

    addToAddress(symbolTable, 100, "code");
    addToAddress(symbolTable, 100 + *IC, "data");

    return FIRSTPASS_SUCCESS_S; 
}
 

void firstPassDirectiveLine(parsedLine *pLine, unsigned int *DC, DataWord dataImage[], SymbolTable* symbolTable, ErrorList* errorList)
{
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    const char* directiveName = pLine->lineContentUnion.directive.directiveName; /* get the directive name */
    
    if (strcmp(directiveName, ".entry") == 0)
        return; /* .entry directive is handled in the second pass */

    if (strcmp(directiveName, ".data") == 0 ||
        strcmp(directiveName, ".string") == 0 ||
        strcmp(directiveName, ".mat") == 0) {
        int i;

        if (*DC + pLine->lineContentUnion.directive.dataCount >= MAX_MEMORY_SIZE) /* check if there is enough space in the data image */
                return;

        if (pLine->label != NULL) { /* if the line has a label */
            errorCode = addSymbol(symbolTable, pLine->label, *DC, directiveName); /* add the label to the symbol table */
            if (errorCode != TABLES_SUCCESS_S) {
                addErrorToList(errorList, errorCode); /* add the error to the error list */
                return;
            }
        }

        for (i = 0; i < pLine->lineContentUnion.directive.dataCount; i++) 
            dataImage[*DC + i].value = pLine->lineContentUnion.directive.dataItems[i]; /* set the data items in the directive image */
        
        *DC += pLine->lineContentUnion.directive.dataCount; /* increase the data counter by the number of data items */
        return; /* return after handling the directive */
    }

    if (strcmp(directiveName, ".extern") == 0){ 
        errorCode = addSymbol(symbolTable, pLine->lineContentUnion.directive.directiveLabel, EXTERN_SYMBOL_ADDRESS, directiveName); /* add the extern label to the symbol table */
        if (errorCode != TABLES_SUCCESS_S) {
            addErrorToList(errorList, errorCode); /* add the error to the error list */
            return;
        }
    }

}

void firstPassInstructionLine(parsedLine *pLine, unsigned int *IC, MacroTable *macroNames, SymbolTable *symbolTable, ErrorList *errorList)
{
    ErrCode errorCode = NULL_INITIAL;

    if (pLine->label != NULL) { /* if the line has a label */
        errorCode = addSymbol(symbolTable, pLine->label, *IC, pLine->lineContentUnion.instruction.operationName); /* add the label to the symbol table */
        if (errorCode != TABLES_SUCCESS_S) 
            addErrorToList(errorList, errorCode); /* add the error to the error list */
    }

    /* increase the instruction counter by the number of words in the instruction */
    *IC += pLine->lineContentUnion.instruction.wordCount;
}
