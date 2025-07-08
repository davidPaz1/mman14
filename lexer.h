#ifndef LEXER_H
#define LEXER_H

#include "global.h"
#include "error.h"
#include "tables.h"

#define COLON_LENGTH 1 /* length of the colon character ':' */

typedef enum lineType {
    /* empty line and comment line are set as an errorCode so i could return an empty struct */
    INSTRUCTION_LINE = 0, /* instruction line */
    DIRECTIVE_LINE, /* directive line */
    UNSET_LINE = -1 /* unset line type, used for error handling */
} lineType;

typedef struct parsedLine {
    char* restOfLine; /* the line itself */
    char* label; /* the label of the line, if it has one */
    char* firstToken; /* the first token of the line */
    lineType typesOfLine; /* type of the line */ 
} parsedLine;


/* for first pass mainly */
parsedLine* readParsedLine(FILE *fp, ErrCode *errorCode);
ErrCode determineLineType(parsedLine *pLine); /* determine the type of the line and if it has a label */
void freeScannedLine(parsedLine *pLine); /* free the memory allocated for the parsedLine structure */
void printParsedLine(parsedLine *pline); /* print the parsed line for debugging purposes */

/* is X functions prototypes */
/* keyword functions prototypes */
Bool isEndOfLine(const char* str); /* check if the string is an end of line */
Bool isOperationName(const char* arg); /* check if the string is an operation name */
Bool isRegister(const char* arg); /* check if the string is a register name */
Bool isDirective(const char* arg); /* check if the string is a directive */
Bool isMacroStart(const char* arg); /* check if the string is a macro start */
Bool isMacroEnd(const char* arg); /* check if the string is a macro end */
Bool isKeywords(const char* arg); /* check if the string is a keyword */
Bool isLabel(const char* str); /* check if the string is a label */

ErrCode isValidLabel(macroTable *table, const char *label); /* check if the label is valid */
ErrCode isMacroNameValid(macroTable* table , const char* macroName);

#endif
