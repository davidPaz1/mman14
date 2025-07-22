#ifndef SECONDPASS_H
#define SECONDPASS_H
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"

/* Preprocessor functions prototypes */
ErrCode executeSecondPass(FILE* amFile, FILE* obFile, MacroTable* macroNames, SymbolTable* symbolTable, ErrorList* errorList); /* main function for the second pass */
void secPassInstructionLine(parsedLine *pLine, MacroTable* macroNames, SymbolTable* symbolTable, ErrorList* errorList);
void secPassDirectiveLine(parsedLine *pLine, MacroTable* macroNames, SymbolTable* symbolTable, ErrorList* errorList); /* process the directive line */

void writeEntryFile(FILE* entFile, SymbolTable* symbolTable);
void writeExternFile(FILE* extFile, SymbolTable* symbolTable);

void secPassFreeMemory(parsedLine* pLine); /* free the memory allocated from the second pass */
#endif
