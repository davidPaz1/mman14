#include "global.h"
#include "error.h"

void printErrorMsg(ErrCode error) {
    printf("Error code: %d - ", error);
    switch (error) {
        case NULL_INITIAL:
            printf("initial state shouldn't be reached.\n");
            break;
        case UNKNOWN_ERROR:
            printf("Unknown error occurred.\n");
            break;
        case MALLOC_ERROR:
            printf("Memory allocation failed.\n");
            break;
        case LINE_TOO_LONG:
            printf("Error: Line too long.\n");
            break;
        case EOF_REACHED:
            printf("End of file reached.\n");
            break;
        case FILE_READ_ERROR:
            printf("File read error.\n");
            break;
        case FILE_WRITE_ERROR:
            printf("File write error.\n");
            break;
        case MACRO_NAME_EXISTS:
            printf("Macro name already exists.\n");
            break;
        case MACRO_NOT_FOUND:
            printf("Macro not found.\n");
            break;
        default:
            if (error == SCAN_SUCCESS || 
                error == PREPROCESSOR_SUCCESS || 
                error == MACROTABLE_SUCCESS) {
                /* We consider these success codes - no error */
                // Optionally print nothing or:
                printf("Operation successful. but still, you shouldn't reach this point\n");
            } else {
                printf("Unrecognized error code: %d - shouldn't reach this point\n", error);
            }
            break;
    }
}
