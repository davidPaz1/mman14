#ifndef ERROR_H
#define ERROR_H

#include "global.h"

#define MAX_ERROR_MSG_LENGTH 150 /* maximum length of an error message */

/* error codes are sorted by the files they are mainly needed in,
they also are divided into categories of severity and meaning:
_S = signal or success, _F = fatal - halt immediately, _E = error - will halt at the end of the stage
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
    INVALID_DIRECTIVE_E = 22, /* invalid directive */
    UNKNOWN_LINE_TYPE_E = 23, /* unknown line type */
    LABEL_INVALID_START_CHAR_E = 24, /* invalid start character */
    LABEL_INVALID_CHAR_E = 25, /* invalid character in the line */
    LABEL_TOO_LONG_E = 26, /* label is too long */
    LABEL_EMPTY_E = 27, /* label is empty */
    LABEL_TEXT_AFTER_COLON_E = 28, /* text found after label colon */
    LABEL_SAME_AS_MACRO_E = 29, /* label is the same as a macro name */
    LABEL_NAME_IS_KEYWORD_E = 30, /* label is a keyword */
    MACRO_NAME_EXISTS_E = 31, /* macro name already exists */
    MACRO_NAME_TOO_LONG_E = 32, /* macro name is too long */
    MACRO_NAME_EMPTY_E = 33, /* macro name is empty */
    MACRO_INVALID_START_CHAR_E = 34, /* macro name starts with an invalid character */
    MACRO_INVALID_CHAR_E = 35, /* macro name contains invalid characters */
    MACRO_NAME_IS_KEYWORD_E = 36, /* macro name is a keyword */
    DIRECTIVE_DATA_MISSING_E = 37, /* directive is missing data items */
    MISSING_COMMA_E = 38, /* missing comma in the directive */
    DATA_INVALID_VALUE_E = 39, /* data item is not a valid number */
    DATA_ITEM_NOT_INTEGER_E = 40, /* data item is not an integer */
    INTEGER_OUT_OF_RANGE_E = 41, /* integer value is out of range */
    STR_MISSING_OPEN_QUOTE_E = 42, /* string directive is missing an opening quotation mark */
    STR_MISSING_CLOSE_QUOTE_E = 43, /* string directive is missing a closing quotation mark */
    STR_INVALID_CHAR_E = 44, /* string directive contains invalid characters */
    STR_QUOTE_IN_MIDDLE_E = 45, /* string directive contains a quote in the middle */
    MAT_INVALID_ROW_E = 46, /* matrix directive has an invalid row */
    MAT_INVALID_COL_E = 47, /* matrix directive has an invalid column */
    MAT_SIZE_ZERO_NEG_E = 48, /* matrix directive has a size of zero */
    MAT_SIZE_TOO_LARGE_E = 49, /* matrix directive has invalid data */

    /* tables errors 80 - 99 */
    TABLES_SUCCESS_S = 70, /* macro operation was successful */
    TABLES_FAILURE_S = 71, /* macro operation failed */
    SYMBOL_NAME_EXISTS_E = 72, /* symbol name already exists */

    /* preprocessor errors 100 - 109 */
    PREPROCESSOR_SUCCESS_S = 100, /* preprocessor executed successfully */
    PREPROCESSOR_FAILURE_S = 101, /* preprocessor failed */
    UNMATCHED_MACRO_END_E = 102, /* macro closer not found */

    /* firstPass errors 110 - 119 */
    FIRSTPASS_SUCCESS_S = 110, /* first pass was successful */
    FIRSTPASS_FAILURE_S = 111, /* first pass error */

    /* secondPass errors 120 - 129 */
    SECOND_PASS_SUCCESS_S = 120, /* second pass was successful */
    SECOND_PASS_FAILURE_S = 121 /* second pass error */

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

/* error list handling functions prototypes */
ErrorList* createErrorList(char *filename); /* initialize the error list */
void addErrorToList(ErrorList *list, ErrCode code); /* add error to the list */
void printErrors(ErrorList *list); /* print all errors in the list */
void freeErrorsList(ErrorList *list); /* free the error list */

#endif
