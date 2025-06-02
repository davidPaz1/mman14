#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

#include "global.h"

typedef struct MacroBody{ /* linked list of all of the macro lines */
    char* line; /* 1 line from the macro */
    struct MacroBody* nextLine; /* pointer to the next line in the body of the macro */
}MacroBody;

typedef struct macroNode { /* linked list of all the macros */
    char* macroName; /* macro name */
    unsigned int lineCount; /* number of lines in the macro body */
    MacroBody* bodyHead; /* pointer to the first line in the body of the macro */
    MacroBody* bodyTail; /* pointer to the last line in the body of the macro */
    struct macroNode* nextMacro; /* pointer to the next macro in the list */
} macroNode;

typedef struct macroTable {
    int macroCount; /* number of macros in the table */
    macroNode* head; /* pointer to the first macro in the list */
} macroTable;

typedef enum MacroErrorCode {
    MACRO_NO_ERROR = 0, /* no error */
    MACRO_SUCCESS, /* macro operation was successful */
    MACRO_NAME_EXISTS, /* macro name already exists */
    MACRO_NOT_FOUND, /* macro not found */
    MACRO_MALLOC_ERROR /* memory allocation error */
} MacroErrorCode;

/* function prototypes */
macroTable* createMacroTable (char* macroName, char* line);
macroNode* createMacroNode (char* macroName, char* line);
MacroBody* createMacroBody (char* line);
Bool isMacroNameValid (char* macroName);
Bool isMacroExists (macroTable* table, char* macroName);
MacroErrorCode addMacro(macroTable* table, char* name, char* line);
MacroBody* findMacro (macroTable* table, char* macroName);
void freeMacroTable (macroTable* table);
void freeMacroNode (macroNode* node);
void freeMacroBody (MacroBody* body);
#endif