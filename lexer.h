#ifndef LEXER_H
#define LEXER_H

#include "global.h"
#include "error.h"
#include "macroTable.h"

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


/* for first pass mainly */
parsedLine* readLineType(FILE *fp, ErrCode *errorCode);
ErrCode determineLineType(parsedLine *pLine); /* determine the type of the line and if it has a label */
void freeScannedLine(parsedLine *pLine); /* free the memory allocated for the parsedLine structure */
void printParsedLine(parsedLine *pline); /* print the parsed line for debugging purposes */

/* is X functions prototypes */
/* keyword functions prototypes */
Bool isEndOfLine(char* str); /* check if the string is an end of line */
Bool isOperationName(char* arg); /* check if the string is an operation name */
Bool isRegister(char* arg); /* check if the string is a register name */
Bool isDirective(char* arg); /* check if the string is a directive */
Bool isMacroStart(char* arg); /* check if the string is a macro start */
Bool isMacroEnd(char* arg); /* check if the string is a macro end */
Bool isKeywords(char* arg); /* check if the string is a keyword */
Bool isLabel(char* str); /* check if the string is a label */

ErrCode isValidLabel(char *label, macroTable *table); /* check if the label is valid */
ErrCode isMacroNameValid(macroTable* table ,char* macroName);

#endif
