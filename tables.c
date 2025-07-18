#include "tables.h"
#include "global.h"
#include "error.h"
#include "lexer.h" /* for isMacroNameValid */
#include "util.h" /* for strDup */

/* MacroTable functions that operate on the MacroTable struct (linked list)*/
MacroTable* createMacroTable() {
    
    MacroTable* newTable = malloc(sizeof(MacroTable)); /* allocate memory for the table */
    if (newTable == NULL) {
        return NULL; /* exit if memory allocation fails */
    }

    newTable->macroHead = NULL; /* initialize the head of the list to NULL */
    return newTable; /* return the new table */
}

MacroNode* createMacroNode(const char* macroName, ErrCode* errorCode) {
    MacroNode* newMacro; /* returned macro */
    *errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    newMacro = malloc(sizeof(MacroNode));
    if (newMacro == NULL) {
        *errorCode = MALLOC_ERROR_F; /* set error code to MALLOC_ERROR */
        return NULL; /* exit if memory allocation fails */
    }

    newMacro->macroName = strDup(macroName);
    if (newMacro->macroName == NULL) {
        *errorCode = MALLOC_ERROR_F; /* set error code to MALLOC_ERROR */
        free(newMacro);
        return NULL; /* exit if memory allocation fails */
    }

    *errorCode = TABLES_SUCCESS_S; /* set error code to MACROTABLE_SUCCESS */
    newMacro->bodyHead = NULL; /* set the body head to the first line in the body */
    newMacro->bodyTail = NULL; /* set the body tail to the last line in the body */
    newMacro->nextMacro = NULL; /* initialize the next macro pointer to NULL */
    return newMacro;
}

MacroBody* createMacroBody(const char* line, ErrCode* errorCode) {
    MacroBody* newBody = malloc(sizeof(MacroBody));  /* returned body */
    *errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    if (newBody == NULL) {
        *errorCode = MALLOC_ERROR_F;
        return NULL; /* exit if memory allocation fails */
    }

    newBody->line = strDup(line); /* duplicate the line to avoid aliasing */
    if (newBody->line == NULL) {
        *errorCode = MALLOC_ERROR_F;
        free(newBody);
        return NULL;
    }
    newBody->nextLine = NULL;
    *errorCode = TABLES_SUCCESS_S; /* set error code to MACROTABLE_SUCCESS */
    return newBody;
}

/* add a new macro to the table */
ErrCode addMacro(MacroTable* macroTable, const char* name) {
    MacroNode* newMacro; /* new macro to be added */
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    errorCode = isMacroNameValid(macroTable, name); /* check if the macro name is valid */
    if (errorCode != TABLES_SUCCESS_S) /* if the macro name is not valid */
        return errorCode; /* exit if the macro name is not valid */
    

    newMacro = createMacroNode(name, &errorCode);
    if (errorCode != TABLES_SUCCESS_S) 
        return errorCode; /* exit if the macro node creation failed */
    

    newMacro->nextMacro = macroTable->macroHead; /* insert at the beginning of the list */
    macroTable->macroHead = newMacro; /* update the head of the list */
    return TABLES_SUCCESS_S; /* return success */
}

ErrCode addMacroLine(MacroTable* macroTable, const char* line) {
    MacroBody* newLine; /* new line to be added */
    MacroNode* headNode; /* the macro to which the line will be added */
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    headNode = macroTable->macroHead; /* get the head of the macro list */
    
    newLine = createMacroBody(line, &errorCode); /* create a new body for the line */
    if (errorCode != TABLES_SUCCESS_S) 
        return errorCode; /* exit if memory allocation fails */
    

    if (headNode->bodyHead == NULL) { /* if this is the first line in the macro */
        headNode->bodyHead = newLine; /* set the head of the body */
        headNode->bodyTail = newLine; /* set the tail of the body */
    } else {
        headNode->bodyTail->nextLine = newLine; /* link the new line to the end of the body */
        headNode->bodyTail = newLine; /* update the tail of the body */
    }

    return TABLES_SUCCESS_S; /* return success */
}

MacroBody* findMacro(MacroTable* macroTable, const char* macroName) {
    MacroNode* current; /* used to iterate through the macro list */

    current = macroTable->macroHead;
    while (current != NULL) {
        if (strcmp(current->macroName, macroName) == 0) 
            return current->bodyHead; /* return the body of the found macro */
        current = current->nextMacro;
    }

    return NULL; /* Macro not found */
}

/** isMacroExists - checks if a macro with the given name exists in the table.
 * Returns TRUE if the macro exists, otherwise returns FALSE.
 */
Bool isMacroExists(MacroTable* macroTable, const char* macroName) {
    return findMacro(macroTable, macroName) != NULL; /* check if the macro exists by trying to find it */
}


void freeMacroTable(MacroTable* macroTable) {
    if (macroTable == NULL) /* check if the table is NULL */
        return; /* exit if the table is NULL */
    freeMacroNode(macroTable->macroHead); /* free the first macro node */
    free(macroTable);
}

void freeMacroNode(MacroNode* node) {    
    
    MacroNode* current = node; /* used to iterate through the macro nodes */
    if (node == NULL)
        return; /* exit if the node is NULL */
    
    while (current != NULL) { /* iterate through the macro nodes and free them */
        MacroNode* next = current->nextMacro;
        free(current->macroName); /* free the macro name */
        freeMacroBody(current->bodyHead); /* free the body of the macro */
        free(current); /* free the macro node itself */
        current = next;
    }
    
}

void freeMacroBody(MacroBody* body) {
    MacroBody* current = body; /* used to iterate through the body lines */
    if (body == NULL)
        return;
    
    while (current != NULL) { /* iterate through the body lines and free them */
        MacroBody* next = current->nextLine;
        free(current->line);
        free(current);
        current = next;
    }
}

/* SymbolTable public functions */

SymbolTable* createSymbolTable()
{
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (table == NULL)
        return NULL;
    
    table->head = NULL;
    table->haveEntry = FALSE;
    table->haveExtern = FALSE;
    table->count = 0;
    return table;
}

ErrCode addSymbol(SymbolTable* table, const char* name, unsigned int address, const char* firstToken)
{
    SymbolNode* newSymbol; /* new symbol to be added */

    newSymbol = createSymbolNode(name, address, firstToken);
    if (newSymbol == NULL) 
        return MALLOC_ERROR_F;
    

    if (table->head == NULL) 
        table->head = newSymbol;
    else {
        newSymbol->next = table->head; 
        table->head = newSymbol; /* insert at the beginning of the list */
    }

    if (newSymbol->isEntry == TRUE)
        table->haveEntry = TRUE;
    if (newSymbol->type == EXTERN_SYMBOL)
        table->haveExtern = TRUE;

    table->count++;
    return TABLES_SUCCESS_S;
}

SymbolNode* findSymbol(SymbolTable *table, const char *name)
{
    SymbolNode *current; /* used to iterate through the symbol nodes */

    current = table->head;
    while (current != NULL) {
        if (strcmp(current->symbolName, name) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

Bool isSymbolExists(SymbolTable *table, const char *name)
{
    return findSymbol(table, name) != NULL; /* check if the symbol exists by trying to find it */
}

void addToAddress(SymbolTable *table, unsigned int addAddress, const char *typeToAdd)
{
    SymbolNode *current = table->head; /* used to iterate through the symbol nodes */

    while (current != NULL) {
        /* check if the type to add matches the current symbol type */
        if (strcmp(typeToAdd, "data") == 0 && (current->type == DATA_SYMBOL || current->type == EXTERN_SYMBOL))
            current->address += addAddress;
        else if (strcmp(typeToAdd, "code") == 0 && current->type == CODE_SYMBOL)
            current->address += addAddress;

        current = current->next;
    }
}

void freeSymbolTable(SymbolTable* table){
    SymbolNode* current;
    if (table == NULL) /* check if the table is NULL */
    return;

    current = table->head; /* used to iterate through the symbol nodes */
    while (current != NULL) { /* iterate through the symbol nodes and free them */
        SymbolNode* next = current->next; /* save the next node */
        freeSymbolNode(current);
        current = next; /* move to the next node */
    }
    free(table); /* free the table itself */
}

/* SymbolTable private functions */

SymbolNode* createSymbolNode(const char* name, unsigned int address, const char* firstToken){
    SymbolNode* newNode = (SymbolNode*)malloc(sizeof(SymbolNode));
    if (newNode == NULL) 
        return NULL; /* exit if memory allocation fails */
    
    newNode->symbolName = strDup(name);
    if (newNode->symbolName == NULL) {
        free(newNode);
        return NULL;
    }
    newNode->address = address;
    newNode->next = NULL;

    newNode->isEntry = FALSE; /* initialize the entry symbol flag to FALSE */
    newNode->isMat = FALSE; /* initialize the mat symbol flag to FALSE */

    if (strcmp(firstToken, ".entry") == 0)
        newNode->isEntry = TRUE;
    else if (strcmp(firstToken, ".mat") == 0)
        newNode->isMat = TRUE;
    
    if (isOperationName(firstToken))
        newNode->type = CODE_SYMBOL; /* if the token is an operation name, set the type to CODE_SYMBOL */
    else if (strcmp(firstToken, ".extern") == 0)
        newNode->type = EXTERN_SYMBOL;
    else if (isDirective(firstToken)) /* is token .data or .string */
        newNode->type = DATA_SYMBOL;
    else
        newNode->type = UNDEFINED_SYMBOL; /* if the token is not recognized, set the type to UNDEFINED_SYMBOL */
    return newNode;
}

void freeSymbolNode(SymbolNode *node)
{
    if (node == NULL)
        return;

    free(node->symbolName);
    free(node);
}
