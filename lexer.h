#ifndef LEXER_H
#define LEXER_H

#include "global.h"
#include "error.h"
#include "tables.h"

#define COLON_LENGTH 1 /* length of the colon character ':' */

typedef enum lineType {
    INSTRUCTION_LINE = 0, 
    DIRECTIVE_LINE, 
    COMMENT_LINE, 
    EMPTY_LINE, 
    UNSET_LINE = -1 /* used for error handling */
} lineType;

typedef struct parsedLine {
    char* label; /* the label at the start of the line, if it doesn't have one it will be NULL */
    lineType typesOfLine; /* type of the line */ 
    union dataOrInstruction
    {
        /* if the line is a directive (typesOfLine == DIRECTIVE_LINE), this will hold the directive data */
        struct directiveData {
            char* directiveName; /* the name of the directive, e.g. .data , .string , .mat , .entry , .extern */
            unsigned int dataCount; /* number of data items in the directive (also to move DC) */
            int* dataItems; /* .data , .string , .mat items (length is dataCount [for .str +1]) */
            char* directiveLabel; /* .entry or .extern label */
        } directive;

        /* if the line is an instruction (typesOfLine == INSTRUCTION_LINE), this will hold the instruction data */
        struct instructionData {
            char* operationName; /* the name of the operation */
            unsigned int wordCount; /* number of words needed for the binary code of the instruction (to move IC) */
            unsigned int operandCount; /* number of operands in the instruction */
            char* operand1; /* first operand */
            char* operand2; /* second operand */
        } instruction;
    }lineContentUnion; /* union to hold either directive or instruction data */
} parsedLine;

#define INITIAL_DATA_ITEMS_SIZE 10 /* initial size of the dataItems array for directives */

/* for first pass mainly */
parsedLine* readParsedLine(FILE *fp, ErrCode *errorCode, macroTable *table, ErrorList *errorList); /* read a line from the file and return a parsedLine structure */
ErrCode getLabelFromLine(parsedLine *pline, char *line, macroTable *macroNames, ErrorList *errorList); /* get the label from the line if it exists */
ErrCode determineLineType(parsedLine *pLine, char *line); /* determine the type of the line and if it has a label */
ErrCode parseDirectiveLine(parsedLine *pline, char *line, ErrorList *errorList); 
ErrCode parseDataDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList);
ErrCode parseStrDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList);
ErrCode parseMatDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList);
ErrCode parseEntryDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList);
ErrCode parseExternDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList);
ErrCode parseInstructionLine(parsedLine *pLine, char *line, ErrorList *errorList);
void freeParsedLine(parsedLine *pLine); /* free the memory allocated for the parsedLine structure */
void printParsedLine(parsedLine *pLine);
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
Bool isValidInteger(int value); /* check if the integer value is valid for the assembler */

ErrCode isValidLabel(macroTable *table, const char *label); /* check if the label is valid */
ErrCode isMacroNameValid(macroTable* table , const char* macroName);

#endif
