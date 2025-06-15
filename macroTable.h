#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

#include "global.h"
#include "error.h"

typedef struct macroBody{ /* linked list of all of the macro lines */
    char* line; /* 1 line from the macro */
    struct macroBody* nextLine; /* pointer to the next line in the body of the macro */
}macroBody;

typedef struct macroNode { /* linked list of all the macros */
    char* macroName; /* macro name */
    unsigned int lineCount; /* number of lines in the macro body */
    macroBody* bodyHead; /* pointer to the first line in the body of the macro */
    macroBody* bodyTail; /* pointer to the last line in the body of the macro */
    struct macroNode* nextMacro; /* pointer to the next macro in the list */
} macroNode;

typedef struct macroTable {
    int macroCount; /* number of macros in the table */
    macroNode* head; /* pointer to the first macro in the list */
} macroTable;


/* function prototypes */
macroTable* createMacroTable(void);
macroNode* createMacroNode(char* macroName);
macroBody* createMacroBody(char* line);
ErrCode addMacro(macroTable* table , char* name);
ErrCode addMacroLine(macroTable* table, char* line);
macroBody* findMacro(macroTable* table, char* macroName);
Bool isMacroNameValid(char* macroName);
Bool isMacroExists(macroTable* table, char* macroName);
void freeMacroTable(macroTable* table);
void freeMacroNode(macroNode* node);
void freeMacroBody(macroBody* body);
#endif