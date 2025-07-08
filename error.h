#ifndef ERROR_H
#define ERROR_H

#include "global.h"

#define MAX_ERROR_MSG_LENGTH 150 /* maximum length of an error message */

/* error codes are sorted by the files they are mainly needed in,
they also are divided into categories of severity and meaning:
_S = signal or success, _F = fatal - halt immediately, _E = error - will halt at the end of the stage
note all unspecified errors shouldn't be used and if they are we consider them fatal */
typedef enum ErrCode {
    /* VERY GENERAL error codes 0 - 5 */
    NULL_INITIAL = 0, /* initial state of error code, used to "forget" previous errorCode, should never be used */
    MALLOC_ERROR_F = 1, /* memory allocation Ferror */
    EOF_REACHED_S = 2, /* indicate that the EOF was reached while reading */
    EXTRANEOUS_TEXT_E = 3, /* extraneous text after the end of the line */

    /* error.c codes 4 - 7 */
    ERROR_C_FILE_SUCCESS_S = 4, /* success error file */
    ERROR_C_FILE_FAILURE_S = 5, /* failure error file */
    SEEN_FATAL_ERROR_S = 6, /* indicates that a fatal error was seen */
    MALLOC_ERROR_LIST_F = 7, /* memory allocation error for the error list */

    /* debugging errors 8 - 9 */
    UNKNOWN_ERROR = 8, /* unknown error, should never be used */
    UNEXPECTED_NULL_INPUT_F = 9, /* unexpected NULL input, should never be used */

    /* util errors 10 - 29 */
    UTIL_SUCCESS_S = 10,
    END_OF_LINE_S = 11,
    LINE_TOO_LONG_E = 12,
    INPUT_FILE_UNREADABLE_F = 13,
    FILE_READ_ERROR_F = 14,
    FILE_WRITE_ERROR_F = 15,
    INVALID_FILE_MODE_F = 16, /* invalid file mode */
    FILE_DELETE_ERROR_F = 17, /* file delete error */
    
    /* lexer errors 20 - 39 */
    LEXER_SUCCESS_S = 20, /* scanning was successful */
    COMMENT_LINE_TYPE_S = 21, /* comment line */
    EMPTY_LINE_TYPE_S = 22, /* empty line */
    UNKNOWN_LINE_TYPE_E = 23, /* unknown line type */
    LABEL_INVALID_START_CHAR_E = 24, /* invalid start character */
    LABEL_INVALID_CHAR_E = 25, /* invalid character in the line */
    LABEL_TOO_LONG_E = 26, /* label is too long */
    LABEL_EMPTY_E = 27, /* label is empty */
    LABEL_TEXT_AFTER_COLON_E = 28, /* text found after label colon */
    LABEL_SAME_AS_MACRO_E = 29, /* label is the same as a macro name, should never be used */
    LABEL_NAME_IS_KEYWORD_E = 30, /* label is a keyword, should never be used */
    INVALID_DIRECTIVE_E = 31, /* invalid directive */

    /* tables errors 40 - 69 */
    MACROTABLE_SUCCESS_S = 40, /* macro operation was successful */
    MACRO_NAME_EXISTS_E = 41, /* macro name already exists */
    UNMATCHED_MACRO_END_E = 42, /* macro not found */
    MACRO_NAME_TOO_LONG_E = 43, /* macro name is too long */
    MACRO_NAME_EMPTY_E = 44, /* macro name is empty */
    MACRO_INVALID_START_CHAR_E = 45, /* macro name starts with an invalid character */
    MACRO_INVALID_CHAR_E = 46, /* macro name contains invalid characters */
    MACRO_NAME_IS_KEYWORD_E = 47, /* macro name is a keyword, should never be used */

    SYMBOLTABLE_SUCCESS_S = 50, /* symbol table operation was successful */
    SYMBOL_NAME_EXISTS_E = 51, /* symbol name already exists */

    /* preprocessor errors 70 - 79 */
    PREPROCESSOR_SUCCESS_S = 70, /* preprocessor executed successfully */
    PREPROCESSOR_FAILURE_S = 71, /* preprocessor failed */

    /* firstPass errors 80 - 89 */
    FIRSTPASS_SUCCESS_S = 80, /* first pass was successful */
    FIRSTPASS_FAILURE_S = 81 /* first pass error, should never be used */

} ErrCode;

typedef struct ErrorNode {
    ErrCode errCode;
    unsigned int line;              
    struct ErrorNode *next;
} ErrorNode;

typedef struct ErrorList {
    unsigned int count; /* the number of errors in the list */
    char* stage; /* the stage that the error(s) occurred in, e.g. "preprocessor", "first pass" */
    char* filename; /* the name of the file where the error(s) occurred */
    Bool fatalError; /* indicates if there is a fatal error in the list like malloc failure */
    struct ErrorNode* head;
    struct ErrorNode* tail;
} ErrorList;

/* errorcode handling functions prototypes */
char* getErrorMessage(ErrCode error); /* print error message based on error code */
void printErrorMsg(ErrCode code, char *stage, unsigned int line); /* print error message based on error code */
Bool isFatalErr(ErrCode code);

/* error list handling functions prototypes */
ErrorList* createErrorList(char *filename); /* initialize the error list */
void addErrorToList(ErrorList *list, ErrCode code, unsigned int line); /* add error to the list */
void printErrors(ErrorList *list, unsigned int incCount); /* print all errors in the list */
void freeErrorsList(ErrorList *list); /* free the error list */

#endif
