#ifndef SCAN_H
#define SCAN_H

#include "global.h"
#include "error.h"
#include "macroTable.h"

#define OVER_LENGTH  1 /* overlength should store \n or \r if line is under MAX_LINE_FILE_LENGTH */
#define COMMA_LENGTH 1 /* length of the comma character ',' */
#define COLON_LENGTH 1 /* length of the colon character ':' */

typedef enum lineType {
    EMPTY_LINE = 0, /* empty line */
    COMMENT_LINE,   /* comment line */
    INSTRUCTION_LINE, /* instruction line */
    DIRECTIVE_LINE /* directive line */
} lineType;

typedef struct parsedLine {
    char* restOfLine; /* the line itself */
    char* label; /* the label of the line, if it has one */
    char* firstToken; /* the first token of the line */
    lineType typesOfLine; /* type of the line */ 
} parsedLine;

/* scan functions prototypes */

/* mainly for preprocessing */
char* readLine(FILE *fp, ErrCode *errorCode);
char* getFirstToken(char *str, ErrCode *errorCode); /* get the first token from the string */
char* cutFirstToken(char *str, ErrCode *errorCode); /* cut the first token from the string */

Bool isEndOfLine(char* str); /* check if the string is an end of line */
ErrCode isValidLabel(char *label, macroTable *table); /* check if the label is valid */
Bool isLabel(char* str);

/* for first pass mainly */
parsedLine* readLineType(FILE *fp, ErrCode *errorCode);
ErrCode determineLineType(parsedLine *pLine); /* determine the type of the line and if it has a label */
Bool isLabel(char* str); /* check if the string is a label */
void freeScannedLine(parsedLine *pLine); /* free the memory allocated for the parsedLine structure */
#endif
