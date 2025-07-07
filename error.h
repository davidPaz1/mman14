#ifndef ERROR_H
#define ERROR_H

#include "global.h"

#define MAX_ERROR_MSG_LENGTH 150 /* maximum length of an error message */

/* error codes are divided into categories*/
typedef enum ErrCode {
    /* VERY GENERAL error codes 0 - 5 */
    NULL_INITIAL = 0, /* initial state of error code, used to "forget" previous errorCode */
    MALLOC_ERROR = 1, /* memory allocation Ferror */
    EOF_REACHED = 2, /* indicate that the EOF was reached while reading */
    EXTRANEOUS_TEXT = 3, /* extraneous text after the end of the line */

    /* debugging errors 6 - 9 */
    UNKNOWN_ERROR = 6, /* unknown error, should never be used */
    UNEXPECTED_NULL_INPUT = 7, /* unexpected NULL input, should never be used */

    /* util errors 10 - 29 */
    UTIL_SUCCESS = 10,
    END_OF_LINE = 11,
    UNKNOWN_LINE_TYPE = 12, /* unknown line type */
    LINE_TOO_LONG = 13,
    FILE_READ_ERROR = 14,
    FILE_WRITE_ERROR = 15,
    INVALID_FILE_MODE = 16, /* invalid file mode, should never be used */
    FILE_DELETE_ERROR = 17, /* file delete error */

    /* lexer errors 20 - 39 */
    LEXER_SUCCESS = 20, /* scanning was successful */
    EMPTY_LINE_TYPE = 21, /* empty line */
    COMMENT_LINE_TYPE = 22, /* comment line */
    LABEL_INVALID_START_CHAR = 23, /* invalid start character */
    LABEL_INVALID_CHAR = 24, /* invalid character in the line */
    LABEL_TOO_LONG = 25, /* label is too long */
    LABEL_EMPTY = 26, /* label is empty */
    LABEL_TEXT_AFTER_COLON = 27, /* text found after label colon */
    LABEL_SAME_AS_MACRO = 28, /* label is the same as a macro name, should never be used */
    INVALID_DIRECTIVE = 29, /* invalid directive */

    /* tables errors 40 - 59 */
    MACROTABLE_SUCCESS = 40, /* macro operation was successful */
    MACRO_NAME_EXISTS = 41, /* macro name already exists */
    UNMATCHED_MACRO_END = 42, /* macro not found */
    MACRO_NAME_TOO_LONG = 43, /* macro name is too long */
    MACRO_NAME_EMPTY = 44, /* macro name is empty */
    MACRO_NAME_INVALID_START_CHAR = 45, /* macro name starts with an invalid character */
    MACRO_NAME_INVALID_CHAR = 46, /* macro name contains invalid characters */
    MACRO_NAME_KEYWORD = 47, /* macro name is a keyword, should never be used */

    SYMBOLTABLE_SUCCESS = 50, /* symbol table operation was successful */
    SYMBOL_NAME_EXISTS = 51, /* symbol name already exists */

    /* preprocessor errors 60 - 69 */
    PREPROCESSOR_SUCCESS = 60, /* preprocessor executed successfully */
    PREPROCESSOR_FAILURE = 61, /* preprocessor failed */

    /* firstPass errors 70 - 79 */
    FIRSTPASS_SUCCESS = 70, /* first pass was successful */
    FIRSTPASS_FAILURE = 71 /* first pass error, should never be used */

} ErrCode;

typedef struct ErrorNode {
    ErrCode code;
    unsigned int line;              
    struct ErrorNode *next;
} ErrorNode;

typedef struct ErrorList {
    unsigned int count;
    Bool fatalError; /* indicates if there is a fatal error in the list like malloc failure */
    struct ErrorNode* head;
    struct ErrorNode* tail;
} ErrorList;

/* errorcode handling functions prototypes */
char* getErrorMessage(ErrCode error); /* print error message based on error code */
Bool isFatalErr(ErrCode code);
void printErrorMsg(ErrCode code, char *context); /* print error message based on error code */

/* error list handling functions prototypes */
void createErrorList(ErrorList *list); /* initialize the error list */
int addError(ErrorList *list, ErrCode code, unsigned int line, Bool isFatal); /* add error to the list */
void printErrors(ErrorList *list, char *filename, char *stage); /* print all errors in the list */
void freeErrorsList(ErrorList *list); /* free the error list */

#endif
