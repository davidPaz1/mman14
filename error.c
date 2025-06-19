#include "global.h"
#include "error.h"

char* getErrorMessage(ErrCode code) {
    switch (code) {
        case NULL_INITIAL: /* 0 */
            return "initial state shouldn't be reached.";
        case MALLOC_ERROR: /* 1 */
                return "memory allocation failed.";
        case UNKNOWN_ERROR: /* 3 */
            return "unknown error occurred. Should never be used."; 
        case UNEXPECTED_NULL_INPUT: /* 4 */
            return "unexpected NULL input, should never be used.";
        
        case LINE_TOO_LONG: /* 12 */
            return "line length in file is longer than allowed (80).";
        case FILE_READ_ERROR: /* 13 */
            return "file read error.";
        case FILE_WRITE_ERROR: /* 14 */
            return "file write error.";
        case INVALID_FILE_MODE: /* 15 */
            return "file mode error, should never be used.";
        case FILE_DELETE_ERROR: /* 16 */
            return "file delete error.";

        case PREPROCESSOR_FAILURE: /* 21 */
            return "preprocessor failed."; /* shuld delete this probably? */
        
        case MACRO_NAME_EXISTS: /* 31 */
            return "macro name already exists.";
        case MACRO_NOT_DEF: /* 32 */
            return "tried to spread an undefined macro.";
        case MACRO_NAME_TOO_LONG: /* 33 */
            return "macro name is too long.";
        case MACRO_NAME_EMPTY: /* 34 */
            return "macro name is empty.";
        case MACRO_NAME_INVALID_CHAR: /* 35 */
            return "macro name contains invalid characters.";
        case MACRO_NAME_KEYWORD: /* 36 */
            return "macro name is a set keyword by the assembley language.";
        
        case SCAN_SUCCESS: /* 10 */
        case PREPROCESSOR_SUCCESS: /* 20 */
        case MACROTABLE_SUCCESS: /* 30 */
            return "operation successful, but you shouldn't reach this point.";
        default:
            return "unrecognized error code - shouldn't reach this point.";
    }
}

void printErrorMsg(ErrCode code, char *context)
{
    fprintf(stderr, "Ecode: %d - ", code);
    if (context[0] == '\0' || context == NULL) /* if context is NULL or empty */
        fprintf(stderr, "%s\n", getErrorMessage(code));
    else
        fprintf(stderr, "Error %s because %s\n", context, getErrorMessage(code));
    if(context == NULL) /* if context is NULL it is due to malloc failure */
        fprintf(stderr, "Error %s\n", getErrorMessage(MALLOC_ERROR));
}
