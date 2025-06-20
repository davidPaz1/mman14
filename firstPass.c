#include "global.h"
#include "scan.h"
#include "util.h"
#include "error.h"
#include "macroTable.h"
#include "firstPass.h"

/* .am -> .ob , .ext , .ent*/
/* 17 has an explanation for the first pass */
/*page 19 or 31 for algorithm for preprocessing */

ErrCode executeFirstPass(char *inputFileName)
{
    int DC = 0; /* data counter */
    int IC = 100; /* instruction counter */
    char *line, *firstToken; /* line to read from the .as file and first token of the line */
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */
    FILE *amFile = NULL; /* file pointer for the assembly file */
    Bool inSymbolDef = FALSE; /* flag to indicate if we are in a symbol definition line */

    amFile = openFile(inputFileName, ".as", "r", &errorCode);
    if (errorCode != SCAN_SUCCESS) 
        return errorCode; /* return the error code */

    /* table setup? */
    
    while (errorCode != EOF_REACHED) {
        line = readLine(amFile, &errorCode); /* read a line from the .as file 1 */
        if (errorCode == EOF_REACHED)
            break; /* end of file reached, exit the loop */
            
        if (errorCode != SCAN_SUCCESS) { /* check if an error occurred while reading the line */
            firstPassErrorExit(amFile, line, NULL); /* clean up and exit the first pass */
            return errorCode; /* return failure if an error occurred */
        }

        firstToken = getFirstToken(line, &errorCode); /* get the first token from the line */
        if(errorCode == END_OF_LINE){ /* if the line is empty or contains only whitespace */
            firstPassFreeStr(line, firstToken); /* free the memory allocated for the line and token */
            continue; /* skip to the next line */
        }

    }

    return FIRSTPASS_SUCCESS; 
}
 

void firstPassErrorExit(FILE* amFile, char* line, char* token)
{
    /* add any additional cleanup code here if needed */
    firstPassFreeMemory(amFile, line, token); /* free the memory allocated from the first pass */
}


void firstPassFreeMemory(FILE* amFile, char* line, char* token)
{
    if (amFile != NULL)
        fclose(amFile);
    firstPassFreeStr(line, token); /* free the memory allocated for the line and token */
}

void firstPassFreeStr(char* line, char* token)
{
    if (line != NULL) {
        free(line);
    }
    if (token != NULL) {
        free(token);
    }
}