#ifndef DEFINE_H
#define DEFINE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* defines */
#define MAX_INFILE_LENGTH 81 /* +1 for null terminator */

#define BASE2_INSTRUCTION_LENGTH 10
#define BASE4_INSTRUCTION_LENGTH 5
#define NULL_TERMINATOR 1

/* error codes */
#define NULL_INITIAL 2 /*used to "forget" previous errorCode*/
#define EOF_REACHED 3
#define MALLOC_ERROR 4
#define FILE_READ_ERROR 5
#define FILE_WRITE_ERROR 6


/* functions */
char* base4(int *word);
char* readLine(FILE *fp, int *errorCode);
FILE* openFile(char* filename, char* ending , char* mode, int* errorCode);
#endif