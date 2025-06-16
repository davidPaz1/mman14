#include "global.h"
#include "error.h"

void printErrorMsg(ErrCode error) {
    fprintf(stderr, "Error code: %d - ", error);
    switch (error) {
        case NULL_INITIAL:
            fprintf(stderr, "initial state shouldn't be reached.\n");
            break;
        case UNKNOWN_ERROR:
            fprintf(stderr, "Unknown error occurred.\n");
            break;
        case MALLOC_ERROR:
            fprintf(stderr, "Memory allocation failed.\n");
            break;
        case LINE_TOO_LONG:
            fprintf(stderr, "Error: Line too long.\n");
            break;
        case FILE_READ_ERROR:
            fprintf(stderr, "File read error.\n");
            break;
        case FILE_WRITE_ERROR:
            fprintf(stderr, "File write error.\n");
            break;
        case MACRO_NAME_EXISTS:
            fprintf(stderr, "Macro name already exists.\n");
            break;
        case MACRO_NOT_FOUND:
            fprintf(stderr, "Macro not found.\n");
            break;
        default:
            if (error == SCAN_SUCCESS || 
                error == PREPROCESSOR_SUCCESS || 
                error == MACROTABLE_SUCCESS) {
                /* We consider these success codes - no error */
                /* Optionally print nothing or: */
                fprintf(stderr, "Operation successful. but still, you shouldn't reach this point\n");
            } else {
                fprintf(stderr, "Unrecognized error code: %d - shouldn't reach this point\n", error);
            }
            break;
    }
}
