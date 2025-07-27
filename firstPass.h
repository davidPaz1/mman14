#ifndef FIRSTPASS_H
#define FIRSTPASS_H
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"

/* Preprocessor functions prototypes */
ErrCode executeFirstPass(FILE* amFile, DataWord* directiveImage, int* DCF, int* ICF, MacroTable* macroTable, SymbolTable* symbolTable, ErrorList* errorList); /* main function for the preprocessor */

void firstPassErrorExit(parsedLine* pLine); /* clean up and exit the first pass */
void firstPassFreeMemory(parsedLine* pLine); /* free the memory allocated from the first pass */
void firstPassFreeStr(char* line, char* token); /* free the memory allocated for the line and token */
#endif
