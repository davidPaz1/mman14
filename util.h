#ifndef UTIL_H
#define UTIL_H
#include "global.h"

/* utility functions prototypes */
char* strDup(char* src); /* string duplication */
Bool isOperationName(char* str); /* check if the string is an operation name */
char* mergeStrings(char* str1, char* str2); /* merge two strings */
void cutnChar(char *str, int n); /* cut the first n characters from the string */
#endif