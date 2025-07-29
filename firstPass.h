#ifndef FIRSTPASS_H
#define FIRSTPASS_H
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"

#define EXTERN_SYMBOL_ADDRESS 0 /* address of an extern symbol in the symbol table */

/* Preprocessor functions prototypes */
ErrCode executeFirstPass(FILE* amFile, DataWord dataImage[], unsigned int* DC, unsigned int* IC, MacroTable* macroTable, SymbolTable* symbolTable, ErrorList* errorList); /* main function for the preprocessor */

void firstPassDirectiveLine(parsedLine *pLine, unsigned int *DC, DataWord dataImage[], SymbolTable* symbolTable, ErrorList* errorList); /* handle directive lines in the first pass */
void firstPassInstructionLine(parsedLine *pLine, unsigned int *IC, MacroTable *macroNames, SymbolTable *symbolTable, ErrorList *errorList); /* handle instruction lines in the first pass */

#endif
