#include "global.h"

char* strDup(const char* src) {
    char* dest = malloc(strlen(src) + NULL_TERMINATOR);
    if (dest != NULL) {
        strcpy(dest, src);
    }
    return dest;
}