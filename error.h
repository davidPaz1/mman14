#ifndef ERROR_H
#define ERROR_H

#include "global.h"

/* error codes are divided into categories*/
typedef enum ErrCode {
    /* VERY GENERAL error codes 0 - 2 */
    NULL_INITIAL = 0, /* initial state of error code, used to "forget" previous errorCode */
    UNKNOWN_ERROR = 1, /* unknown error, should never be used */
    MALLOC_ERROR = 2, /* memory allocation error */

    /* util errors 2 - 9 */

    /* scan errors 10 - 19 */
    SCAN_SUCCESS = 10, 
    LINE_TOO_LONG = 11,
    EOF_REACHED = 12,
    FILE_READ_ERROR = 13,
    FILE_WRITE_ERROR = 14,

    /* preprocessor errors 20 - 29 */
    PREPROCESSOR_SUCCESS = 20, /* preprocessor executed successfully */

    /* macroTable errors 30 - 39 */
    MACROTABLE_SUCCESS = 30, /* macro operation was successful */
    MACRO_NAME_EXISTS = 31, /* macro name already exists */
    MACRO_NOT_FOUND = 32, /* macro not found */

    /* ??? 40 - 49 */
} ErrCode;

/* errorcode handling functions prototypes */
void printErrorMsg(ErrCode error); /* print error message based on error code */
#endif
