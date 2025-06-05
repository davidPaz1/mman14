#ifndef SCAN_H
#define SCAN_H
#include "global.h"


typedef enum lineType {
    EMPTY_LINE = 0, /* empty line */
    COMMENT_LINE,   /* comment line */
    INSTRUCTION_LINE, /* instruction line */
    DATA_LINE,      /* data line */
    STRING_LINE,    /* string line */
    EXTERN_LINE,    /* extern line */
    ENTRY_LINE      /* entry line */
} lineType;

typedef struct scannedLine {
    char *line; /* the line itself */
    lineType type; /* type of the line */
    Bool isLabel; /* does the line have a label, 1 if so 0 otherwise */
    /* note it doesnt check if the label is valid only its presence */
} scannedLine;

/* scan functions prototypes */
char* readLine(FILE *fp, int *errorCode);
scannedLine* readLine2(FILE *fp, int *errorCode);

char* getFirstWord(char *str); /* get the first word from the string and remove it from the string */
lineType determineLineType(scannedLine *sLine); /* determine the type of the line and if it has a label */
FILE* openFile(char* filename, char* ending , char* mode, int* errorCode);
#endif
