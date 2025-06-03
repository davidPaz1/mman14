#ifndef SCAN_H
#define SCAN_H
#include "global.h"

/* scan functions prototypes */
char* readLine(FILE *fp, int *errorCode);
FILE* openFile(char* filename, char* ending , char* mode, int* errorCode);

#endif 
