#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include "global.h"
#include "error.h"
#include "scan.h"
#include "macroTable.h"
/* Preprocessor functions prototypes */
ErrCode executePreprocessor(char *inputFileName); /* main function for the preprocessor */

ErrCode spreadMacro(macroTable *table, char *macroName, FILE *amFile); /* spread the macro body into the .am file */
ErrCode macroDef(macroTable* table, char* line); /* add a line to the macro body */

void PreprocessorErrorExit(macroTable* table, FILE* asFile, FILE* amFile, char* line, char* fileName); /* clean up and exit the preprocessor */
void preprocessorFreeMemory(macroTable* table, FILE* asFile, FILE* amFile, char* line); /* free the memory allocated for a macro body */

#endif
