#ifndef FIRSTPASS_H
#define FIRSTPASS_H
#include "global.h"
#include "error.h"
#include "scan.h"


/* Preprocessor functions prototypes */
ErrCode executeFirstPass(char *inputFileName); /* main function for the preprocessor */


void firstPassErrorExit(FILE* amFile, char* line, char* token); /* clean up and exit the first pass */
void firstPassFreeMemory(FILE* amFile, char* line, char* token); /* free the memory allocated from the first pass */
void firstPassFreeStr(char* line, char* token); /* free the memory allocated for the line and token */
#endif
