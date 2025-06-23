#ifndef FIRSTPASS_H
#define FIRSTPASS_H
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "macroTable.h"

/* Preprocessor functions prototypes */
ErrCode executeFirstPass(char *inputFileName, int *DCF, int *ICF, macroTable macroNames); /* main function for the preprocessor */


void firstPassErrorExit(FILE* amFile, parsedLine* pLine); /* clean up and exit the first pass */
void firstPassFreeMemory(FILE* amFile, parsedLine* pLine); /* free the memory allocated from the first pass */
void firstPassFreeStr(char* line, char* token); /* free the memory allocated for the line and token */
#endif
