#include "global.h"
#include "util.h"

char* strDup(char* src) {
    char* dest = malloc(strlen(src) + NULL_TERMINATOR);
    if (dest != NULL) 
        strcpy(dest, src);
    return dest;
}

Bool isOperationName(char* str) {

    if (strcmp(str, "mov") == 0 ||
        strcmp(str, "cmp") == 0 ||
        strcmp(str, "add") == 0 ||
        strcmp(str, "sub") == 0 ||
        strcmp(str, "lea") == 0 ||
        strcmp(str, "clr") == 0 ||
        strcmp(str, "not") == 0 ||
        strcmp(str, "inc") == 0 ||
        strcmp(str, "dec") == 0 ||
        strcmp(str, "jmp") == 0 ||
        strcmp(str, "bne") == 0 ||
        strcmp(str, "jsr") == 0 ||
        strcmp(str, "red") == 0 ||
        strcmp(str, "prn") == 0 ||
        strcmp(str, "rts") == 0 ||
        strcmp(str, "stop") == 0)
        return TRUE;
    else 
        return FALSE;
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
    memmove(str, str + n, strlen(str) - n + 1); /* Move the string left by n characters */
}
