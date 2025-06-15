#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

#include "global.h"
#include "error.h"

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


/* function prototypes */
macroTable* createMacroTable(void);
macroNode* createMacroNode(char* macroName, MacroBody* body);
MacroBody* createMacroBody(char* line);
Bool isMacroNameValid(char* macroName);
Bool isMacroExists(macroTable* table, char* macroName);
ErrCode addMacro(macroTable* table, char* name, MacroBody* body);
MacroBody* findMacro(macroTable* table, char* macroName);
void freeMacroTable(macroTable* table);
void freeMacroNode(macroNode* node);
void freeMacroBody(MacroBody* body);
#endif