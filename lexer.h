#ifndef LEXER_H
#define LEXER_H

#include "global.h"
#include "error.h"
#include "tables.h"

#define COLON_LENGTH 1 /* length of the colon character ':' */
#define QUOTE_LENGTH 1 /* length of the quote character '"' */
#define REGISTER_LENGTH 2 /* length of the register name, e.g. r0, r1, ..., r7 */
#define INITIAL_DATA_ITEMS_SIZE 10 /* initial size of the dataItems array for directives */
#define ERROR_OPERAND_AMOUNT -1 /* used for error handling when the number of operands is not valid */
#define NO_OPERANDS 0 /* used for instructions with no operands */
#define ONE_OPERAND 1 /* used for instructions with one operand */
#define TWO_OPERANDS 2 /* used for instructions with two operands */
#define BOTH_REGISTER_OPERAND_BIN_LINES 1 /* the wordLine amount needed (1) if both operands are registers */
#define MATRIX_OPERAND_BIN_LINES 2 /* number of binary lines needed for a matrix operand */
#define NON_MATRIX_OPERAND_BIN_LINES 1 /* number of bits needed for a non-matrix operand */

typedef enum lineType {
    UNSET_LINE = 0, /* used for error handling */
    INSTRUCTION_LINE = 1, 
    DIRECTIVE_LINE, 
    COMMENT_LINE, 
    EMPTY_LINE
} lineType;

typedef enum operandType {
    UNKNOWN_OPERAND = 0, /* used for error handling */
    REGISTER_OPERAND = 1, 
    NUMBER_OPERAND, 
    MATRIX_SYNTAX_OPERAND, /* matrix - syntax check only */
    MATRIX_TABLE_OPERAND, /* matrix - valid matrix in symbol table */
    LABEL_SYNTAX_OPERAND, /* label - unchecked in symbol table */
    LABEL_TABLE_OPERAND /* label - valid in symbol table */
} operandType;

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
            
            operandType operand1Type; /* type of the first operand (e.g. register, immediate, label) */
            operandType operand2Type; /* type of the second operand (e.g. register, immediate, label) */
            char* operand1; /* first operand */
            char* row1; /* if first operand is a matrix element, this will hold the row in the line */
            char* col1; /* if first operand is a matrix element, this will hold the column in the line */
            char* operand2; /* second operand */
            char* row2; /* if second operand is a matrix element, this will hold the row in the line */
            char* col2; /* if second operand is a matrix element, this will hold the column in the line */
       
        } instruction;
    }lineContentUnion; /* union to hold either directive or instruction data */
} parsedLine;


/* for first pass mainly */
parsedLine* createParsedLine(); /* create a new parsedLine structure */
parsedLine* readParsedLine(FILE *fp, ErrCode *errorCode, MacroTable *macroNames, ErrorList *errorList); /* read a line from the file and return a parsedLine structure */
ErrCode getLabelFromLine(parsedLine *pline, char *line, MacroTable *macroNames, ErrorList *errorList); /* get the label from the line if it exists */
ErrCode determineLineType(parsedLine *pLine, char *line, MacroTable *macroNames,ErrorList *errorList); /* determine the type of the line and if it has a label */
ErrCode parseDirectiveLine(parsedLine *pline, char *line, MacroTable *macroNames, ErrorList *errorList);
ErrCode parseDataDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList);
ErrCode parseStrDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList);
ErrCode parseMatDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList);
ErrCode parseEntryExternDirectiveLine(parsedLine *pLine, char *line, MacroTable *macroNames, ErrorList *errorList);

ErrCode parseInstructionLine(parsedLine *pLine, char *line, MacroTable *macroNames, ErrorList *errorList);
ErrCode parseInstructionLineOperand(parsedLine *pLine, char *line, ErrorList *errorList); /* parse the operands of the instruction line */
ErrCode determineOperandType(const char *operand, operandType *opType, char **matLabel, char **row, char **col, MacroTable *macroNames, ErrorList *errorList);
ErrCode parseLabelOperandsValid(parsedLine *pLine, SymbolTable *symbolTable, ErrorList *errorList); /* parse the label operands and check if it is valid */
short int numOfOperandsInInstruction(const char *instructionName); /* return the number of operands in the instruction */

void freeParsedLine(parsedLine *pLine); /* free the memory allocated for the parsedLine structure */
void printParsedLine(parsedLine *pLine);
char* printOpType(operandType opType); /* print the operand type */

/* is X functions prototypes */
/* keyword functions prototypes */
Bool isEndOfLine(const char* str); /* check if the string is an end of line */
Bool isOperationName(const char* arg); /* check if the string is an operation name */
Bool isRegister(const char* arg); /* check if the string is a register name */
Bool isDirective(const char* arg); /* check if the string is a directive */
Bool isMacroDef(const char* arg); /* check if the string is a macro start */
Bool isMacroEnd(const char* arg); /* check if the string is a macro end */
Bool isKeywords(const char* arg); /* check if the string is a keyword */
Bool isValidInteger10bits(int value); /* check if the integer value is valid for the assembler */
Bool isValidInteger8bits(int value); /* check if the integer value is valid for the assembler */


ErrCode isRegisterOperand(const char* operand); /* check if the operand is a valid register */
ErrCode isNumberOperand(const char *operand); /* check if the operand is a valid number */
ErrCode parseMatrixOperand(const char *operandStr, char **name, char **row, char **col); /* check if the operand is a valid matrix operand */
ErrCode isValidLabelSyntax(const char *operand); /* check if the operand is a valid label (doesn't check in the symbol table) */
ErrCode isValidLabelColon(MacroTable *table, const char *label); /* check if the label is valid without the colon */
ErrCode isValidLabelName(MacroTable *table, const char *label);
char* delColonFromLabel(const char *label); /* remove the colon from the label if it exists */
ErrCode isMacroNameValid(MacroTable* table , const char* macroName);

typedef enum opCode {
    mov = 0,
    cmp = 1,
    add = 2,
    sub = 3,
    lea = 4,
    clr = 5,
    not = 6,
    inc = 7,
    dec = 8,
    jmp = 9,
    bne = 10,
    jsr = 11,
    red = 12,
    prn = 13,
    rts = 14,
    stop = 15,
    invalid = -1
} OpCodeNumber;

/* registers */
typedef enum registers{
    r0 = 0, 
    r1,
    r2,
    r3,
    r4,
    r5,
    r6,
    r7,
    NOT_REG = -1
} registersNumber;

registersNumber getRegisterNumber(const char *regName);
OpCodeNumber getOpCodeNumber(const char *opName); /* get the opcode number from the operation name */

#endif
