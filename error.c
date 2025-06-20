#include "global.h"
#include "error.h"

char* getErrorMessage(ErrCode code) {
    switch (code) {

        /* VERY GENERAL error codes 0 - 5 */
        case NULL_INITIAL: /* 0 */
            return "initial state shouldn't be reached.";
        case MALLOC_ERROR: /* 1 */
                return "memory allocation failed.";
        case EXTRANEOUS_TEXT: /* 3 */
            return "extraneous text after the end of the line, should never be used.";

        /* debugging errors 6 - 9 */
        case UNKNOWN_ERROR: /* 6 */
            return "unknown error occurred. Should never be used.";
        case UNEXPECTED_NULL_INPUT: /* 7 */
            return "unexpected NULL input, should never be used.";

        /* scan errors 10 - 19 */
        case LINE_TOO_LONG: /* 13 */
            return "line length in file is longer than allowed (80).";
        case FILE_READ_ERROR: /* 14 */
            return "file read error.";
        case FILE_WRITE_ERROR: /* 15 */
            return "file write error.";
        case INVALID_FILE_MODE: /* 16 */
            return "file mode error, should never be used.";
        case FILE_DELETE_ERROR: /* 17 */
            return "file delete error.";

        /* macroTable errors 30 - 39 */
        case MACRO_NAME_EXISTS: /* 31 */
            return "macro name already exists.";
        case UNMATCHED_MACRO_END: /* 32 */
            return "had \"macroend\" without having an opening macro definition.";
        case MACRO_NAME_TOO_LONG: /* 33 */
            return "macro name is too long.";
        case MACRO_NAME_EMPTY: /* 34 */
            return "macro name is empty.";
        case MACRO_NAME_INVALID_CHAR: /* 35 */
            return "macro name contains invalid characters.";
        case MACRO_NAME_KEYWORD: /* 36 */
            return "macro name is a set keyword by the assembley language.";

        /* firstPass errors 40 - 49 */
        case FIRSTPASS_FAILURE: /* 41 */
            return "first pass failed.";

        /* it should never reach here */
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
