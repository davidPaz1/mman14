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
    memmove(str, str + n, strlen(str) - n + 1); /* Move the string left by n characters */
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