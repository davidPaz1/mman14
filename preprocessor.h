#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"
/* Preprocessor functions prototypes */

ErrCode executePreprocessor(macroTable *macroNames, ErrorList *errorList, FILE *asFile, FILE *amFile, const char *inputFileName);/* main function for the preprocessor */

ErrCode spreadMacro(macroTable *table, const char *macroName, FILE *amFile); /* spread the macro body into the .am file */
ErrCode macroDef(macroTable* table, char* line); /* add a line to the macro body */

void preprocessorFreeMemory(char* line, char* firstToken); /* free the memory allocated from the preprocessor */

#endif
