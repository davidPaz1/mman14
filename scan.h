#ifndef SCAN_H
#define SCAN_H
#include "global.h"
#include "error.h"


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

/* mainly for preprocessing */
char* readLine(FILE *fp, ErrCode *errorCode);
char* getFirstToken(char **strPtr, ErrCode *errorCode); /* get the first token from the string */
char* cutFirstToken(char **strPtr, ErrCode *errorCode); /* cut the first token from the string */
Bool isEndOfLine(char* str); /* check if the string is an end of line */

/* for first pass mainly */
scannedLine* readLineType(FILE *fp, ErrCode *errorCode);
lineType* determineLineType(scannedLine *sLine); /* determine the type of the line and if it has a label */
Bool isLabel(char* str); /* check if the string is a label */
#endif
