#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

#include "global.h"

typedef struct MacroBody{ /* linked list of macro lines */
    char* line; /* 1 line from the macro */
    struct MacroBody* nextLine; /* pointer to the next line in the body of the macro */
}MacroBody;

typedef struct macroTable { /* linked list of all the macros */
    char* macroName; /* macro name */
    MacroBody* body; /* body of the macro */
    struct macroTable* nextMacro; /* pointer to the next macro in the list */
} macroTable;

#endif