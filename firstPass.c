#include "firstPass.h"
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "util.h"
#include "tables.h"

/* .am -> .ob , .ext , .ent*/
/* 17 has an explanation for the first pass */
/*page 19 or 31 for algorithm for preprocessing */

ErrCode executeFirstPass(FILE* amFile, int *DCF, int *ICF, macroTable* macroNames, ErrorList* errorList)
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
        
        if (errorCode != LEXER_SUCCESS_S) { /* check if an error occurred while reading the line */
            firstPassErrorExit(pLine); /* clean up and exit the first pass */
            return FIRSTPASS_FAILURE_S; /* return failure if an error occurred */
        }

        if (pLine->typesOfLine == EMPTY_LINE || pLine->typesOfLine == COMMENT_LINE) {
            freeParsedLine(pLine); /* free the parsed line structure */
            continue; /* skip empty or comment lines */
        }

        if(pLine->label != NULL){ /* if the line has a label */
            errorCode = isValidLabel(macroNames, pLine->label); /* check if the label is valid */
            if (errorCode != LEXER_SUCCESS_S) { /* if the label is not valid */
                firstPassErrorExit(pLine); /* clean up and exit the first pass */
                return errorCode; /* return failure if an error occurred */
            }
        }
        
        if(FALSE){ /* used so unused variable warning won't appear */
            /* dummy calculations to avoid unused variable warning */
            L = IC + inSymbolDef + DC; 
            return L; /* dummy return to avoid unused variable warning */
        }
        
        freeParsedLine(pLine);
    } /* end of while loop */

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