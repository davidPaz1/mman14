#ifndef SECONDPASS_H
#define SECONDPASS_H
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"

/* Preprocessor functions prototypes */
ErrCode executeSecondPass(FILE* amFile, CodeWord instructionImage, MacroTable* macroNames, SymbolTable* symbolTable, ErrorList* errorList); /* main function for the second pass */
void secPassInstructionLine(parsedLine *pLine, unsigned int *ic, CodeWord instructionImage, ErrorList* errorList);

void writeEntryFile(FILE* entFile, SymbolTable* symbolTable);
void writeExternFile(FILE* extFile, SymbolTable* symbolTable);

#endif
