#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include "global.h"
#include "error.h"
#include "scan.h"
#include "macroTable.h"
/* Preprocessor functions prototypes */
ErrCode executePreprocessor(char *inputFileName); /* main function for the preprocessor */
Bool isMacroDef(char* line); /* check if the line is a macro def line */
Bool isMacroEndLine(char* line); /* check if the line is a macro end line */
Bool isMacroUse(char *line); /* check if the line is a macro use line */
ErrCode getMacroName(char* line); /* get the macro name from the line */
Bool isMacroNameValid(char* macroName); /* check if the macro name is valid */
void freeFilesAndMemory(macroTable* table, FILE* asFile, FILE* amFile, char* line); /* free the memory allocated for a macro body */

#endif
