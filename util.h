#ifndef UTIL_H
#define UTIL_H
#include "global.h"
#include "error.h"

/* utility functions prototypes */
char* strDup(char* src); /* string duplication */
char* mergeStrings(char* str1, char* str2); /* merge two strings */
void cutnChar(char *str, int n); /* cut the first n characters from the string */

FILE* openFile(char* filename, char* ending , char* mode, ErrCode* errorCode);
void writeLineFile(FILE* fp, char* line); /* write a line to a file */
ErrCode delFile(char* filename, char* ending);


/* keyword functions prototypes */
Bool isOperationName(char* arg); /* check if the string is an operation name */
Bool isRegister(char* arg); /* check if the string is a register name */
Bool isMacroStart(char* arg); /* check if the string is a macro start */
Bool isMacroEnd(char* arg); /* check if the string is a macro end */
Bool isKeywords(char* arg); /* check if the string is a keyword */
#endif