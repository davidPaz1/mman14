#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

#include "global.h"
#include "error.h"

typedef struct macroBody{ /* linked list of all of the macro lines */
    char* line; /* 1 line from the macro */
    struct macroBody* nextLine; /* pointer to the next line in the body of the macro */
}macroBody;

typedef struct macroNode {  /* linked list of all the macros */
    char* macroName; /* macro name */
    macroBody* bodyHead; /* pointer to the first line in the body of the macro */
    macroBody* bodyTail; /* pointer to the last line to add new lines */
    struct macroNode* nextMacro; /* pointer to the next macro in the list */
} macroNode;

typedef struct macroTable { /* head of linked list of all the macros */
    macroNode* macroHead; /* pointer to the first macro in the list */
} macroTable;


/* function prototypes */

/* "public" functions */
macroTable* createMacroTable(ErrCode *errorCode);
ErrCode addMacro(macroTable* table , char* name);
ErrCode addMacroLine(macroTable* table, char* line);
macroBody* findMacro(macroTable* table, char* macroName);
void freeMacroTable(macroTable* table);

/* "private" functions */
macroNode* createMacroNode(char* macroName, ErrCode* errorCode);
macroBody* createMacroBody(char* line, ErrCode* errorCode);
ErrCode isMacroNameValid(macroTable* table, char* macroName);
Bool isMacroExists(macroTable* table, char* macroName);
void freeMacroNode(macroNode* node);
void freeMacroBody(macroBody* body);

#endif