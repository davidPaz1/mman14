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
