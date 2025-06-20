#include "global.h"
#include "util.h"

char* strDup(char* src) {
    char* dest = malloc(strlen(src) + NULL_TERMINATOR);
    if (dest != NULL) 
        strcpy(dest, src);
    return dest;
}

char *mergeStrings(char *start, char *end)
{
    char *merged;
    if (start == NULL || end == NULL)
        return NULL;
    

    merged = malloc(strlen(start) + strlen(end) + NULL_TERMINATOR); /* enough room for merged string */
    if (merged == NULL) { /* malloc failed */
        return NULL;
    }

    strcpy(merged, start); /* copy the first string */
    strcat(merged, end); /* concatenate the two strings */
    return merged;
}

void cutnChar(char *str, int n)
{
    int spacesCount = 0, cuttingLength = n; /* length to cut from the string */
    while (isspace(str[spacesCount])) 
        spacesCount++;
    
    cuttingLength += spacesCount; /* adjust the length to cut based on leading spaces */

    memmove(str, str + cuttingLength, strlen(str) - cuttingLength + 1); /* Move the string left by n characters */

    cuttingLength = 0; /* reset cutting length to 0 for deleting trailing spaces */
    while (isspace(str[cuttingLength])) 
        cuttingLength++;

    memmove(str, str + cuttingLength, strlen(str) - cuttingLength + 1); /* Move the string left by the number of leading spaces */
}

FILE* openFile(char *filename, char *ending, char *mode, ErrCode *errorCode)
{
    FILE *fp;
    char* fullFileName = mergeStrings(filename, ending); /* merge filename and ending to create full file name */
    *errorCode = NULL_INITIAL; /* reset error code to initial state */

    if (fullFileName == NULL) {
        *errorCode = MALLOC_ERROR;
        return NULL;
    }

    fp = fopen(fullFileName, mode); /* open the file with the given mode */
    if (fp == NULL) {
        if (strcmp(mode, "r") == 0) 
            *errorCode = FILE_READ_ERROR;
        else if (strcmp(mode, "w") == 0) 
            *errorCode = FILE_WRITE_ERROR;
        else 
            *errorCode = INVALID_FILE_MODE;
    }

    if (*errorCode == NULL_INITIAL) /* if no error occurred */
    *errorCode = SCAN_SUCCESS; /* set error code to success */
    
    free(fullFileName); /* free the allocated memory for fullFileName */
    return fp;
}

ErrCode delFile(char *filename, char *ending)
{
    char *fullFileName;

    fullFileName = mergeStrings(filename, ending);
    if (fullFileName == NULL) {
        return MALLOC_ERROR;
    }

    if (remove(fullFileName) != 0) { /* if file delete was unsuccessful */
        free(fullFileName);
        return FILE_DELETE_ERROR;
    }

    free(fullFileName);
    return SCAN_SUCCESS;
}


Bool isOperationName(char* arg) {

    if (strcmp(arg, "mov") == 0 ||
        strcmp(arg, "cmp") == 0 ||
        strcmp(arg, "add") == 0 ||
        strcmp(arg, "sub") == 0 ||
        strcmp(arg, "lea") == 0 ||
        strcmp(arg, "clr") == 0 ||
        strcmp(arg, "not") == 0 ||
        strcmp(arg, "inc") == 0 ||
        strcmp(arg, "dec") == 0 ||
        strcmp(arg, "jmp") == 0 ||
        strcmp(arg, "bne") == 0 ||
        strcmp(arg, "jsr") == 0 ||
        strcmp(arg, "red") == 0 ||
        strcmp(arg, "prn") == 0 ||
        strcmp(arg, "rts") == 0 ||
        strcmp(arg, "stop") == 0)
        return TRUE;
    else 
        return FALSE;
}

Bool isRegister(char* arg) {
    if (strcmp(arg, "r0") == 0 ||
        strcmp(arg, "r1") == 0 ||
        strcmp(arg, "r2") == 0 ||
        strcmp(arg, "r3") == 0 ||
        strcmp(arg, "r4") == 0 ||
        strcmp(arg, "r5") == 0 ||
        strcmp(arg, "r6") == 0 ||
        strcmp(arg, "r7") == 0)
        return TRUE;
    return FALSE;
}

Bool isDirective(char* arg) {
    if (strcmp(arg, ".data") == 0 ||
        strcmp(arg, ".string") == 0 ||
        strcmp(arg, ".entry") == 0 ||
        strcmp(arg, ".extern") == 0)
        return TRUE;
    return FALSE;
}

Bool isMacroStart(char* arg) {
    if (strcmp(arg, "mcro") == 0)
        return TRUE;
    return FALSE;
}

Bool isMacroEnd(char* arg) {
    if (strcmp(arg, "mcroend") == 0)
        return TRUE;
    return FALSE;
}

Bool isKeywords(char *arg)
{
    if (isOperationName(arg) ||
        isRegister(arg) ||
        isDirective(arg) ||
        isMacroStart(arg) ||
        isMacroEnd(arg))
        return TRUE;
    return FALSE;
}