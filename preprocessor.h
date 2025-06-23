#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "macroTable.h"
/* Preprocessor functions prototypes */
ErrCode executePreprocessor(char *inputFileName, macroTable *macroNames); /* main function for the preprocessor */

ErrCode spreadMacro(macroTable *table, char *macroName, FILE *amFile); /* spread the macro body into the .am file */
ErrCode macroDef(macroTable* table, char* line); /* add a line to the macro body */

void PreprocessorErrorExit(macroTable* table, FILE* asFile, FILE* amFile, char* line, char* fileName); /* clean up and exit the preprocessor */
void preprocessorFreeMemory(macroTable* table, FILE* asFile, FILE* amFile, char* line); /* free the memory allocated from the preprocessor */

#endif
