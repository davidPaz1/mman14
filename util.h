#ifndef UTIL_H
#define UTIL_H
#include "global.h"
#include "error.h"

/* utility functions prototypes */
#define OVER_LENGTH  1 /* overlength should store \n or \r if line is under MAX_LINE_FILE_LENGTH */
#define COMMA_LENGTH 1 /* length of the comma character ',' */

/* scanning functions */
char* readLine(FILE *fp, ErrCode *errorCode);
char* getFirstToken(const char *str, ErrCode *errorCode); /* get the first token from the string */
char* cutFirstToken(char *str, ErrCode *errorCode); /* cut the first token from the string */


/* string manipulation functions */
char* strDup(const char* src); /* string duplication */
char* mergeStrings(const char* str1, const char* str2); /* merge two strings */
void cutnChar(char *str, int n); /* cut the first n characters from the string */

/* file management functions */
FILE* openFile(const char *filename, const char *ending, const char *mode, ErrCode *errorCode);
ErrCode delFile(const char *filename, const char *ending);

#endif