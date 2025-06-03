#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include "global.h"
#include "scan.h"
#include "macroTable.h"
/* Preprocessor functions prototypes */
int executePreprocessor(char *inputFileName); /* main function for the preprocessor */
Bool isMacroLine(char* line); /* check if the line is a macro line */
Bool isMacroEndLine(char* line); /* check if the line is a macro end line */


#endif 
