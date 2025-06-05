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

void cutnChar(char *str, int n)
{
    if (n < 0 || n > strlen(str)) {
        str = NULL; /* If n is negative or greater than the string length, return NULL */
        printf(" util Error: n is out of bounds.\n"); /*test123*/
        return;
    }
    memmove(str, str + n, strlen(str) - n + 1); // Move the string left by n characters
}
