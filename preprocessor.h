#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"
/* Preprocessor functions prototypes */

ErrCode executePreprocessor(MacroTable* macroTable, ErrorList* errorList, FILE* asFile, FILE* amFile, const char* inputFileName);/* main function for the preprocessor */

void spreadMacro(MacroTable* macroTable, const char* macroName, FILE* amFile); /* spread the macro body into the .am file */
ErrCode macroDef(MacroTable* macroTable, char* line); /* add a line to the macro body */

void preprocessorFreeMemory(char* line, char* firstToken); /* free the memory allocated from the preprocessor */

#endif
