#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include "global.h"
#include "scan.h"
#include "macroTable.h"
/* Preprocessor functions prototypes */
int executePreprocessor(char *inputFileName); /* main function for the preprocessor */
Bool isMacroDef(char* line); /* check if the line is a macro def line */
Bool isMacroEndLine(char* line); /* check if the line is a macro end line */
Bool isMacroUse(char *line, MacroBody **body); /* check if the line is a macro use line */
void freeFilesAndMemory(macroTable* table, MacroBody* body, FILE* asFile, FILE* amFile, char* line); /* free the memory allocated for a macro body */

#endif
