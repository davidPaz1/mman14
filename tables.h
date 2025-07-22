#ifndef TABLES_H
#define TABLES_H

#include "global.h"
#include "error.h"

/* MacroTable definitions */

typedef struct MacroBody{ /* linked list of all of the macro lines */
    char* line; /* 1 line from the macro */
    struct MacroBody* nextLine; /* pointer to the next line in the body of the macro */
}MacroBody;

typedef struct MacroNode {  /* linked list of all the macros */
    char* macroName; /* macro name */
    MacroBody* bodyHead; /* pointer to the first line in the body of the macro */
    MacroBody* bodyTail; /* pointer to the last line to add new lines */
    struct MacroNode* nextMacro; /* pointer to the next macro in the list */
} MacroNode;

typedef struct MacroTable { /* head of linked list of all the macros */
    MacroNode* macroHead; /* pointer to the first macro in the list */
} MacroTable;

/* "public" macro functions */
MacroTable* createMacroTable();
ErrCode addMacro(MacroTable* macroTable , const char* name);
ErrCode addMacroLine(MacroTable* macroTable, const char* line);
MacroBody* findMacro(MacroTable* macroTable, const char* macroName);
void freeMacroTable(MacroTable* macroTable);

/* "private" macro functions */
MacroNode* createMacroNode(const char* macroName, ErrCode* errorCode);
MacroBody* createMacroBody(const char* line, ErrCode* errorCode);
Bool isMacroExists(MacroTable* macroTable, const char* macroName);
void freeMacroNode(MacroNode* node);
void freeMacroBody(MacroBody* body);


/* SymbolTable definitions */

typedef enum Symbol_Type{
    UNDEFINED_SYMBOL = -1, /* symbol is not defined */
    DATA_SYMBOL = 1,
    CODE_SYMBOL,
    EXTERN_SYMBOL
} Symbol_Type;

typedef struct SymbolNode {
    char* symbolName; /* name of the symbol */
    unsigned int address; /* address of the symbol */
    unsigned int isEntry : 1; /* flags to indicate if the symbol is an entry symbol */
    unsigned int isMat : 1; /* flags to indicate if the symbol is a mat symbol */
    Symbol_Type type; /* type of the symbol */
    struct SymbolNode* next; /* pointer to the next symbol in the list */
} SymbolNode;

typedef struct SymbolTable {
    SymbolNode* head; /* pointer to the first symbol in the list */
    unsigned int haveEntry : 1; /* flag to indicate if there is an entry symbol */
    unsigned int haveExtern : 1; /* flag to indicate if there is an extern symbol */
    unsigned int count; /* number of symbols in the table */
} SymbolTable;

/* "public" symbol functions */
SymbolTable* createSymbolTable();
ErrCode addSymbol(SymbolTable* table, const char* name, unsigned int address, const char* firstToken);
SymbolNode* findSymbol(SymbolTable* table, const char* name);
Bool isSymbolExists(SymbolTable* table, const char* name);
void addToAddress(SymbolTable* table, unsigned int addAddress, const char* typeToAdd);
void freeSymbolTable(SymbolTable* table);

/* "private" symbol functions */
SymbolNode* createSymbolNode(const char* name, unsigned int address, const char* firstToken);
void freeSymbolNode(SymbolNode* node);


#endif