#include "firstPass.h"
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "util.h"
#include "tables.h"

/* .am -> .ob , .ext , .ent*/
/* 17 has an explanation for the first pass */
/*page 19 or 31 for algorithm for preprocessing */

ErrCode executeFirstPass(FILE* amFile, DataWord* directiveImage, int* DCF, int* ICF, MacroTable* macroNames, SymbolTable* symbolTable, ErrorList* errorList)
{
    int DC = 0; /* data counter */
    int IC = 100; /* instruction counter */
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    Bool inSymbolDef = FALSE; /* flag to indicate if we are in a symbol definition line */
    errorList->currentLine = 0; /* reset the current line number */

    /* table setup? */
    
    while (errorCode != EOF_REACHED_S) {
        parsedLine *pLine; /* parsed line structure to hold the line and its type */
        int L; /* will hold the num of words that the instruction line translates to in machine code*/
        
        if (errorList->fatalError) /* check if there was a fatal error in previous iterations */
            return FIRSTPASS_FAILURE_S;
        
        printf("Reading line %d\n", ++errorList->currentLine);  /* debug print to show the current line number */
        
        pLine = readParsedLine(amFile, &errorCode, macroNames, errorList); /* read a line from the .as file 1 */
        if (errorCode == EOF_REACHED_S)
            break; /* end of file reached, exit the loop */
        
        if (errorCode == LEXER_FAILURE_S) /* if reading the line failed */
            continue; /* continue to the next line */

        if (pLine->typesOfLine == EMPTY_LINE || pLine->typesOfLine == COMMENT_LINE) {
            freeParsedLine(pLine); /* free the parsed line structure */
            continue; /* skip empty or comment lines */
        }

        if (pLine->typesOfLine == INSTRUCTION_LINE)
            printParsedLine(pLine); 

        if(pLine->label != NULL){ /* if the line has a label */
            errorCode = isValidLabelColon(macroNames, pLine->label);  /* check if the label is valid */
            if (errorCode != LEXER_SUCCESS_S) {
                addErrorToList(errorList, errorCode);
                freeParsedLine(pLine);
                continue; /* continue to the next line */
            }
        }
        
        if(FALSE){ /* used so unused variable warning won't appear */
            /* dummy calculations to avoid unused variable warning */
            L = IC + inSymbolDef + DC; 
            return L; /* dummy return to avoid unused variable warning */
        }
        
        freeParsedLine(pLine);
    } /* end of while loop */

    if(errorList->count > 0) /* if there are errors in the error list */
        return FIRSTPASS_FAILURE_S;

    *DCF = DC; /* set the final data counter */
    *ICF = IC; /* set the final instruction counter */
    return FIRSTPASS_SUCCESS_S; 
}
 

void firstPassErrorExit(parsedLine* pLine)
{
    /* add any additional cleanup code here if needed */
    firstPassFreeMemory(pLine); /* free the memory allocated from the first pass */
}


void firstPassFreeMemory(parsedLine* pline)
{
    freeParsedLine(pline); /* free the parsed line structure */
}

void firstPassFreeStr(char* line, char* token)
{
    if (line != NULL) 
        free(line);
    
    if (token != NULL) 
        free(token);
    
}