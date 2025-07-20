#ifndef LEXER_H
#define LEXER_H

#include "global.h"
#include "error.h"
#include "tables.h"

#define COLON_LENGTH 1 /* length of the colon character ':' */
#define QUOTE_LENGTH 1 /* length of the quote character '"' */

typedef enum lineType {
    UNSET_LINE = 0, /* used for error handling */
    INSTRUCTION_LINE = 1, 
    DIRECTIVE_LINE, 
    COMMENT_LINE, 
    EMPTY_LINE
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
parsedLine* createParsedLine(); /* create a new parsedLine structure */
parsedLine* readParsedLine(FILE *fp, ErrCode *errorCode, MacroTable *macroNames, ErrorList *errorList); /* read a line from the file and return a parsedLine structure */
ErrCode getLabelFromLine(parsedLine *pline, char *line, MacroTable *macroNames, ErrorList *errorList); /* get the label from the line if it exists */
ErrCode determineLineType(parsedLine *pLine, char *line); /* determine the type of the line and if it has a label */
ErrCode parseDirectiveLine(parsedLine *pline, char *line, MacroTable *macroNames, ErrorList *errorList);
ErrCode parseDataDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList);
ErrCode parseStrDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList);
ErrCode parseMatDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList);
ErrCode parseEntryExternDirectiveLine(parsedLine *pLine, char *line, MacroTable *macroNames, ErrorList *errorList);
ErrCode parseInstructionLine(parsedLine *pLine, char *line, ErrorList *errorList);

short int numOfWordsInInstruction(parsedLine *pLine); /* return the number of words needed for the binary code of the instruction */
short int numOfOperandsInInstruction(const char *instructionName); /* return the number of operands in the instruction */
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
Bool isColonLabel(const char* str); /* check if the string is a label with a colon at the end */
Bool isValidInteger(int value); /* check if the integer value is valid for the assembler */

ErrCode isValidLabel(SymbolTable *symbolTable, const char *label); /* check if the label is valid */
ErrCode isValidLabelColon(MacroTable *table, const char *label); /* check if the label is valid without the colon */
ErrCode isValidLabelName(MacroTable *table, const char *label);
char* delColonFromLabel(const char *label); /* remove the colon from the label if it exists */
ErrCode isMacroNameValid(MacroTable* table , const char* macroName);

#endif
