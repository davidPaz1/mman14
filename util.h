#ifndef UTIL_H
#define UTIL_H
#include "global.h"

/* utility functions prototypes */
char* strDup(char* src); /* string duplication */
Bool isOperationName(char* str); /* check if the string is an operation name */

void cutnChar(char *str, int n); /* cut the first n characters from the string */
#endif