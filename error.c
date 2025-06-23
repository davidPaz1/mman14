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

        /* scan errors 10 - 29 */
        case UNKNOWN_LINE_TYPE: /* 12 */
            return "unknown line type found, should be one of the four types (instruction, directive, comment, empty).";
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
        case LABEL_INVALID_START_CHAR: /* 18 */
            return "label starts with an invalid character, should start with a letter.";
        case LABEL_INVALID_CHAR: /* 19 */
            return "label contains invalid characters, should only contain letters and digits.";
        case LABEL_TOO_LONG: /* 21 */
            return "label is too long, should be less than 30 characters.";
        case LABEL_EMPTY: /* 22 */
            return "label does not have a name, should not be empty.";
        case LABEL_TEXT_AFTER_COLON: /* 23 */
            return "text found after label colon, should not have any text after the colon.";
        case LABEL_SAME_AS_MACRO: /* 24 */
            return "label and a macro name cannot be the same.";
        case INVALID_DIRECTIVE: /* 25 */
            return "invalid directive, should be one of the valid directives (.data, .string, .entry, .extern).";

        /* macroTable errors 30 - 39 */
        case MACRO_NAME_EXISTS: /* 31 */
            return "macro name already exists.";
        case UNMATCHED_MACRO_END: /* 32 */
            return "had \"macroend\" without having an opening macro definition.";
        case MACRO_NAME_TOO_LONG: /* 33 */
            return "macro name is too long.";
        case MACRO_NAME_EMPTY: /* 34 */
            return "macro name is empty.";
        case MACRO_NAME_INVALID_START_CHAR: /* 35 */
            return "macro name starts with an invalid character, should start with a letter.";
        case MACRO_NAME_INVALID_CHAR: /* 36 */
            return "macro name contains invalid characters.";
        case MACRO_NAME_KEYWORD: /* 37 */   
            return "macro name is a set keyword by the assembly language.";

        /* symbolTable errors 40 - 49 */
        case SYMBOLTABLE_SUCCESS: /* 40 */
            return "symbol table operation was successful.";
        case SYMBOL_NAME_EXISTS: /* 41 */
            return "symbol name already exists.";

        
        /* firstPass errors 60 - 69 */
        case FIRSTPASS_FAILURE: /* 61 */
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
