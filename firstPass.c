#include "firstPass.h"
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "util.h"
#include "tables.h"

/* .am -> .ob , .ext , .ent*/
/* 17 has an explanation for the first pass */
/*page 19 or 31 for algorithm for preprocessing */

ErrCode executeFirstPass(char *inputFileName, int *DCF, int *ICF, macroTable* macroNames)
{
    int lineNumber = 0; /* line number in the .as file [debug]*/
    int DC = 0; /* data counter */
    int IC = 100; /* instruction counter */
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    FILE *amFile = NULL; /* file pointer for the assembly file */
    Bool inSymbolDef = FALSE; /* flag to indicate if we are in a symbol definition line */

    amFile = openFile(inputFileName, ".am", "r", &errorCode);
    if (errorCode != UTIL_SUCCESS) 
        return errorCode; /* return the error code */

    /* table setup? */
    
    while (errorCode != EOF_REACHED) {
        char *restOfLine, *firstToken, *token = NULL; /* line to read from the .as file and first token of the line */
        parsedLine *pLine; /* parsed line structure to hold the line and its type */
        int L; /* will hold the num of words that the instruction line translates to in machine code*/
        printf("Reading line %d\n", ++lineNumber);  /* debug print to show the current line number */

        pLine = readParsedLine(amFile, &errorCode); /* read a line from the .as file 1 */
        if (errorCode == EOF_REACHED)
            break; /* end of file reached, exit the loop */
        
        if (errorCode == EMPTY_LINE_TYPE || errorCode == COMMENT_LINE_TYPE)
            continue; /* skip to the next line */
        
        if (errorCode != LEXER_SUCCESS) { /* check if an error occurred while reading the line */
            firstPassErrorExit(amFile, pLine); /* clean up and exit the first pass */
            return errorCode; /* return failure if an error occurred */
        }

        restOfLine = pLine->restOfLine; /* get the rest of the line from the parsedLine structure (easier to read) */
        firstToken = pLine->firstToken; /* get the first token from the parsedLine structure (easier to read) */

        if(pLine->label != NULL){ /* if the line has a label */
            errorCode = isValidLabel(pLine->label, macroNames); /* check if the label is valid */
            if (errorCode != LEXER_SUCCESS) { /* if the label is not valid */
                firstPassErrorExit(amFile, pLine); /* clean up and exit the first pass */
                return errorCode; /* return failure if an error occurred */
            }
        }
        
        if(FALSE){ /* used so unused variable warning won't appear */
            /* dummy calculations to avoid unused variable warning */
            int a = (int)(*restOfLine + *firstToken + *token);
            L = IC + inSymbolDef + DC + lineNumber; 
            return L + a; /* dummy return to avoid unused variable warning */
        }
        
        freeScannedLine(pLine);
    } /* end of while loop */

    fclose(amFile); /* close the .as file */
    *DCF = DC; /* set the final data counter */
    *ICF = IC; /* set the final instruction counter */
    return FIRSTPASS_SUCCESS; 
}
 

void firstPassErrorExit(FILE* amFile, parsedLine* pLine)
{
    /* add any additional cleanup code here if needed */
    firstPassFreeMemory(amFile, pLine); /* free the memory allocated from the first pass */
}


void firstPassFreeMemory(FILE* amFile, parsedLine* pline)
{
    if (amFile != NULL)
        fclose(amFile);
    freeScannedLine(pline); /* free the parsed line structure */
}

void firstPassFreeStr(char* line, char* token)
{
    if (line != NULL) 
        free(line);
    
    if (token != NULL) 
        free(token);
    
}