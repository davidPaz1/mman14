#ifndef ERROR_H
#define ERROR_H

#include "global.h"

#define MAX_ERROR_MSG_LENGTH 150 /* maximum length of an error message */

/* error codes are divided into categories*/
typedef enum ErrCode {
    /* VERY GENERAL error codes 0 - 5 */
    NULL_INITIAL = 0, /* initial state of error code, used to "forget" previous errorCode */
    MALLOC_ERROR = 1, /* memory allocation error */
    EOF_REACHED = 2, /* indicate that the EOF was reached while reading */
    EXTRANEOUS_TEXT = 3, /* extraneous text after the end of the line */

    /* debugging errors 6 - 9 */
    UNKNOWN_ERROR = 6, /* unknown error, should never be used */
    UNEXPECTED_NULL_INPUT = 7, /* unexpected NULL input, should never be used */

    /* scan errors 10 - 19 */
    SCAN_SUCCESS = 10,
    END_OF_LINE = 11,
    TOKEN_IS_LABEL = 12, /* label found in the line */
    LINE_TOO_LONG = 13,
    FILE_READ_ERROR = 14,
    FILE_WRITE_ERROR = 15,
    INVALID_FILE_MODE = 16, /* invalid file mode, should never be used */
    FILE_DELETE_ERROR = 17, /* file delete error */

    /* preprocessor errors 20 - 29 */
    PREPROCESSOR_SUCCESS = 20, /* preprocessor executed successfully */
    PREPROCESSOR_FAILURE = 21, /* preprocessor error, should never be used */

    /* macro errors 30 - 39 */
    MACROTABLE_SUCCESS = 30, /* macro operation was successful */
    MACRO_NAME_EXISTS = 31, /* macro name already exists */
    MACRO_NOT_DEF = 32, /* macro not found */
    MACRO_NAME_TOO_LONG = 33, /* macro name is too long */
    MACRO_NAME_EMPTY = 34, /* macro name is empty */
    MACRO_NAME_INVALID_CHAR = 35, /* macro name contains invalid characters */
    MACRO_NAME_KEYWORD = 36 /* macro name is a keyword, should never be used */

    /* ??? 40 - 49 */


} ErrCode;

/* errorcode handling functions prototypes */
char* getErrorMessage(ErrCode error); /* print error message based on error code */
void printErrorMsg(ErrCode code, char *context); /* print error message based on error code */

#endif
