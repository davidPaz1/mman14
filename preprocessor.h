#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"
/* Preprocessor functions prototypes */

ErrCode executePreprocessor(FILE *asFile, FILE *amFile, MacroTable *macroTable, ErrorList *errorList); /* main function for the preprocessor */

void spreadMacro(MacroTable* macroTable, const char* macroName, FILE* amFile); /* spread the macro body into the .am file */
ErrCode macroDef(MacroTable* macroTable, char* line); /* add a line to the macro body */

#endif
