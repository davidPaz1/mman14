#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include "global.h"
#include "error.h"
#include "scan.h"
#include "macroTable.h"
/* Preprocessor functions prototypes */
ErrCode executePreprocessor(char *inputFileName); /* main function for the preprocessor */
ErrCode isMacroDef(char* line); /* check if the line is a macro def line */
ErrCode isMacroEndLine(char* line); /* check if the line is a macro end line */
ErrCode isMacroUse(macroTable* table, char *line); /* check if the line is a macro use line */
char* getMacroName(char *line, ErrCode *errorCode); /* get the macro name from the line */


void freeFilesAndMemory(macroTable* table, FILE* asFile, FILE* amFile, char* line); /* free the memory allocated for a macro body */

#endif
