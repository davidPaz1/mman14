#ifndef SECONDPASS_H
#define SECONDPASS_H
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"

/* Preprocessor functions prototypes */
ErrCode executeSecondPass(FILE* amFile, FILE* obFile, MacroTable* macroTable, SymbolTable* symbolTable, ErrorList* errorList); /* main function for the second pass */


void writeEntryFile(FILE* entFile, SymbolTable* symbolTable);
void writeExternFile(FILE* extFile, SymbolTable* symbolTable);

void secPassErrorExit(parsedLine* pLine); /* clean up and exit the second pass */
void secPassFreeMemory(parsedLine* pLine); /* free the memory allocated from the second pass */
#endif
