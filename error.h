#ifndef ERROR_H
#define ERROR_H

#include "global.h"

/* error codes are sorted by the files they are mainly needed in,
they also are divided into categories of severity and meaning:
_S = signal or success, _F = fatal - halt immediately, _E = error - will halt at the end of the stage, _N = note - wont halt and wont count as an error.
note all unspecified errors shouldn't be used and if they are we consider them fatal */
typedef enum ErrCode {
    /* VERY GENERAL error codes 0 - 7 */
    NULL_INITIAL = 0, /* initial state of error code, used to "forget" previous errorCode, should never be used */
    MALLOC_ERROR_F = 1, /* memory allocation Ferror */
    EOF_REACHED_S = 2, /* indicate that the EOF was reached while reading */
    EXTRANEOUS_TEXT_E = 3, /* extraneous text after the end of the line */
    MALLOC_ERROR_File_Del_F = 4, /* memory allocation error for the file delete function */
    MALLOC_ERROR_LIST_F = 5, /* memory allocation error for the error list */

    /* debugging errors 8 - 9 */
    UNKNOWN_ERROR = 8, /* unknown error, should never be used */

    /* util errors 10 - 29 */
    UTIL_SUCCESS_S = 10,
    END_OF_LINE_S = 11,
    LINE_TOO_LONG_E = 12,
    INPUT_FILE_UNREADABLE_F = 13,
    FILE_READ_ERROR_F = 14,
    FILE_WRITE_ERROR_F = 15,
    INVALID_FILE_MODE_F = 16, /* invalid file mode */
    FILE_DELETE_ERROR_F = 17, /* file delete error */
    
    /* lexer errors 20 - 49 */
    LEXER_SUCCESS_S = 20, 
    LEXER_FAILURE_S = 21,
    LABEL_EMPTY_LINE_E = 22, /* after the label there is an empty line */
    INVALID_DIRECTIVE_E = 23, /* invalid directive */
    MACRO_AFTER_LABEL_E = 24, /* macro found after label */
    MACRO_DEF_AFTER_LABEL_E = 25,
    MACRO_END_AFTER_LABEL_E = 26,
    UNKNOWN_LINE_TYPE_E = 27, /* unknown line type */
    LABEL_INVALID_START_CHAR_E = 28, /* invalid start character */
    LABEL_INVALID_CHAR_E = 29, /* invalid character in the line */
    LABEL_TOO_LONG_E = 30, /* label is too long */
    LABEL_EMPTY_E = 31, /* label is empty */
    LABEL_TEXT_AFTER_COLON_E = 32, /* text found after label colon */
    LABEL_SAME_AS_MACRO_E = 33, /* label is the same as a macro name */
    LABEL_NAME_IS_KEYWORD_E = 34, /* label is a keyword */
    LABEL_NAME_IS_REGISTER_E = 35, /* label is a register */
    LABEL_ALREADY_EXISTS_E = 36, /* label already exists in the symbol table */
    MACRO_NAME_EXISTS_E = 37, /* macro name already exists */
    MACRO_NAME_TOO_LONG_E = 38, /* macro name is too long */
    MACRO_NAME_EMPTY_E = 39, /* macro name is empty */
    MACRO_INVALID_START_CHAR_E = 40, /* macro name starts with an invalid character */
    MACRO_INVALID_CHAR_E = 41, /* macro name contains invalid characters */
    MACRO_NAME_IS_KEYWORD_E = 42, /* macro name is a keyword */
    MACRO_NAME_IS_REGISTER_E = 43, /* macro name is the same as a register name */
    DIRECTIVE_DATA_MISSING_E = 44, /* directive is missing data items */
    MISSING_COMMA_E = 45, /* missing comma in the directive */
    DATA_INVALID_VALUE_E = 46, /* data item is not a valid number */
    DATA_ITEM_NOT_INTEGER_E = 47, /* data item is not an integer */
    INTEGER_OUT_OF_RANGE10_BITS_E = 48, /* integer value is out of range of a word (10 bits) should be between -512 and 511 */
    STR_MISSING_OPEN_QUOTE_E = 49, /* string directive is missing an opening quotation mark */
    STR_MISSING_CLOSE_QUOTE_E = 50, /* string directive is missing a closing quotation mark */
    STR_INVALID_CHAR_E = 51, /* string directive contains invalid characters */
    MAT_INVALID_ROW_E = 52, /* matrix directive has an invalid row */
    MAT_INVALID_COL_E = 53, /* matrix directive has an invalid column */
    MAT_SIZE_ZERO_NEG_E = 54, /* matrix directive has a size of zero */
    MAT_SIZE_TOO_LARGE_E = 55, /* matrix directive has invalid data */
    MISSING_FIRST_OPERAND_E = 56, /* missing first operand in the instruction */
    ONE_OPERAND_COMMA_E = 57, /* one operand but a comma was found */
    MISSING_SECOND_OPERAND_E = 58, /* missing second operand in the instruction */
    THIRD_OPERAND_DETECTED_E = 59, /* more than two operands found */
    MISSING_NUM_OPERAND_E = 60, /* missing numeric operand in the instruction */
    NUMBER_OPERAND_IS_NOT_INTEGER_E = 61, /* number is not an integer */
    INTEGER_OPERAND_OUT_OF_RANGE8_BITS_E = 62 , /* integer value is out of range of a byte (8 bits) should be between -128 and 127 */
    REGISTER_NO_R_E = 63, /* register does not start with 'r' */
    REGISTER_NO_DIGIT_E = 64, /* register does not have a digit after 'r' */
    REGISTER_OUT_OF_RANGE_E = 65, /* register number is out of range (0 - 7) */
    MAT_EMPTY_ROW_INDEX_E = 66, /* matrix row index is empty */
    MAT_MISSING_FIRST_CLOSING_BRACKET_E = 67, /* matrix is missing the first closing bracket */
    MAT_MISSING_SECOND_BRACKET_E = 68, /* matrix is missing the second bracket */
    MAT_EMPTY_COLUMN_INDEX_E = 69, /* matrix column index is empty */
    MAT_MISSING_SECOND_CLOSING_BRACKET_E = 70, /* matrix is missing the second closing bracket */
    MAT_ROW_NOT_REGISTER_N = 71, /* matrix row is not a register */
    MAT_COL_NOT_REGISTER_N = 72, /* matrix column is not a register */
    OPERAND_IS_KEYWORD_E = 73, /* operand is a keyword */
    OPERAND_IS_MACRO_E = 74, /* operand is a macro */
    OPERAND1_ERROR_N = 75, /* first operand is an error */
    OPERAND2_ERROR_N = 76, /* second operand is an error */
    OPERAND1_UNKNOWN_E = 77, /* first operand is unknown */
    OPERAND2_UNKNOWN_E = 78, /* second operand is unknown */
    OPERAND1_LABEL_DOES_NOT_EXIST_E = 79, /* first operand label does not exist */
    OPERAND2_LABEL_DOES_NOT_EXIST_E = 80, /* second operand label does not exist */
    OPERAND1_NON_MAT_SYMBOL_E = 81, /* first operand is a non-matrix symbol used */
    OPERAND2_NON_MAT_SYMBOL_E = 82, /* second operand is a non-matrix symbol used */
    INSTRUCTION_SRC_OP_CANT_REGISTER_E = 83, /* instruction source operand cannot be a register */
    INSTRUCTION_SRC_OP_CANT_NUM_E = 84, /* instruction source operand cannot be a number */
    INSTRUCTION_DST_OP_CANT_NUM_E = 86, /* instruction destination operand cannot be a number */

    /* tables errors 90 - 99 */
    TABLES_SUCCESS_S = 90, /* macro operation was successful */
    TABLES_FAILURE_S = 91, /* macro operation failed */
    SYMBOL_NAME_EXISTS_E = 92, /* symbol name already exists */

    /* preprocessor errors 100 - 109 */
    PREPROCESSOR_SUCCESS_S = 100, /* preprocessor executed successfully */
    PREPROCESSOR_FAILURE_S = 101, /* preprocessor failed */
    UNMATCHED_MACRO_END_E = 102, /* macro closer not found */

    /* firstPass errors 110 - 119 */
    FIRSTPASS_SUCCESS_S = 110, /* first pass was successful */
    FIRSTPASS_FAILURE_S = 111, /* first pass error */

    /* secondPass errors 120 - 129 */
    SECOND_PASS_SUCCESS_S = 120, /* second pass was successful */
    SECOND_PASS_FAILURE_S = 121, /* second pass error */
    ENTRY_LABEL_DOES_NOT_EXIST_E = 122 /* label does not exist error */

} ErrCode;

typedef struct ErrorNode {
    ErrCode errCode;
    unsigned int line;
    struct ErrorNode *next;
} ErrorNode;

typedef struct ErrorList {
    unsigned int count; /* the number of errors in the list */
    unsigned int currentLine; /* the current line number we are processing (will save us from passing it to every function) */
    char* stage; /* the stage that the error(s) occurred in, e.g. "preprocessor", "first pass" */
    char* filename; /* the name of the file where the error(s) occurred */
    Bool fatalError; /* indicates if there is a fatal error in the list like malloc failure */
    struct ErrorNode* head;
    struct ErrorNode* tail;
} ErrorList;

/* errorcode handling functions prototypes */
char* getErrorMessage(ErrCode error); /* print error message based on error code */
void printErrorMsg(ErrCode code, const char *stage, unsigned int line); /* print error message based on error code */
Bool isFatalErr(ErrCode code);
Bool isNoteErr(ErrCode code); /* check if the error is a note error */

/* error list handling functions prototypes */
ErrorList* createErrorList(char *filename); /* initialize the error list */
void addErrorToList(ErrorList *list, ErrCode code); /* add error to the list */
void printErrors(ErrorList *list); /* print all errors in the list */
void freeErrorsList(ErrorList *list); /* free the error list */

#endif
