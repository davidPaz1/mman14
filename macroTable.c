#include "global.h"
#include "error.h"
#include "macroTable.h"
#include "util.h"

/* MacroTable functions that operate on the MacroTable struct (linked list)*/
macroTable* createMacroTable(ErrCode *errorCode) {
    macroTable* newTable; /* returned table */
    *errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    newTable = malloc(sizeof(macroTable)); /* allocate memory for the table */
    if (newTable == NULL) {
        *errorCode = MALLOC_ERROR; /* set error code to MALLOC_ERROR */
        return NULL; /* exit if memory allocation fails */
    }
    newTable->macroHead = NULL; /* initialize the head of the list to NULL */
    *errorCode = MACROTABLE_SUCCESS; /* set error code to MACROTABLE_SUCCESS */
    return newTable; /* return the new table */
}

macroNode* createMacroNode(char* macroName, ErrCode* errorCode) {
    macroNode* newMacro; /* returned macro */
    *errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    newMacro = malloc(sizeof(macroNode));
    if (newMacro == NULL) {
        *errorCode = MALLOC_ERROR; /* set error code to MALLOC_ERROR */
        return NULL; /* exit if memory allocation fails */
    }

    newMacro->macroName = strDup(macroName);
    if (newMacro->macroName == NULL) {
        *errorCode = MALLOC_ERROR; /* set error code to MALLOC_ERROR */
        free(newMacro);
        return NULL; /* exit if memory allocation fails */
    }

    *errorCode = MACROTABLE_SUCCESS; /* set error code to MACROTABLE_SUCCESS */
    newMacro->bodyHead = NULL; /* set the body head to the first line in the body */
    newMacro->bodyTail = NULL; /* set the body tail to the last line in the body */
    newMacro->nextMacro = NULL; /* initialize the next macro pointer to NULL */
    return newMacro;
}

macroBody* createMacroBody(char* line, ErrCode* errorCode) {
    macroBody* newBody = malloc(sizeof(macroBody));  /* returned body */

    if (newBody == NULL) {
        *errorCode = MALLOC_ERROR;
        return NULL; /* exit if memory allocation fails */
    }

    newBody->line = strDup(line); /* duplicate the line to avoid aliasing */
    if (newBody->line == NULL) {
        *errorCode = MALLOC_ERROR;
        free(newBody);
        return NULL;
    }
    newBody->nextLine = NULL;
    *errorCode = MACROTABLE_SUCCESS; /* set error code to MACROTABLE_SUCCESS */
    return newBody;
}

/* add a new macro to the table */
ErrCode addMacro(macroTable* table , char* name) {
    macroNode* newMacro; /* new macro to be added */
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    if (table == NULL || name == NULL) { /*test123*/
        fprintf(stderr, "invalid macro table or line\n");
        return UNEXPECTED_NULL_INPUT; /* exit if the table or line is NULL */
    }

    errorCode = isMacroNameValid(table, name); /* check if the macro name is valid */
    if (errorCode != MACROTABLE_SUCCESS) /* if the macro name is not valid */
        return errorCode; /* exit if the macro name is not valid */
    

    newMacro = createMacroNode(name, &errorCode);
    if (errorCode != MACROTABLE_SUCCESS) 
        return errorCode; /* exit if the macro node creation failed */
    

    newMacro->nextMacro = table->macroHead; /* insert at the beginning of the list */
    table->macroHead = newMacro; /* update the head of the list */
    return MACROTABLE_SUCCESS; /* return success */
}

/* add to the last entered macro the new line*/
ErrCode addMacroLine(macroTable* table, char* line) {
    macroBody* newLine; /* new line to be added */
    macroNode* macrohead; /* the macro to which the line will be added */
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    if (table == NULL || line == NULL) { /*test123*/
        fprintf(stderr, "invalid macro table or line\n");
        return UNEXPECTED_NULL_INPUT; /* exit if the table or line is NULL */
    }
    macrohead = table->macroHead; /* get the head of the macro list */
    
    newLine = createMacroBody(line, &errorCode); /* create a new body for the line */
    if (errorCode != MACROTABLE_SUCCESS) 
        return errorCode; /* exit if memory allocation fails */
    

    if (macrohead->bodyHead == NULL) { /* if this is the first line in the macro */
        macrohead->bodyHead = newLine; /* set the head of the body */
        macrohead->bodyTail = newLine; /* set the tail of the body */
    } else {
        macrohead->bodyTail->nextLine = newLine; /* link the new line to the end of the body */
        macrohead->bodyTail = newLine; /* update the tail of the body */
    }

    return MACROTABLE_SUCCESS; /* return success */
}

macroBody* findMacro(macroTable* table, char* macroName) {
    macroNode* current; /* used to iterate through the macro list */
    if (table == NULL || macroName == NULL) {
        fprintf(stderr, "invalid macro table or macro name\n");
        return NULL; /* exit if the table or macro name is NULL */
    }
    
    current = table->macroHead;
    while (current != NULL) {
        if (strcmp(current->macroName, macroName) == 0) {
            return current->bodyHead; /* return the body of the found macro */
        }
        current = current->nextMacro;
    }
    
    return NULL; /* Macro not found */
}

ErrCode isMacroNameValid(macroTable* table ,char* macroName) {
    int i; /* index for iterating through the macro name */
    int len = strlen(macroName); /* length of the macro name */

    if (len == 0)
        return MACRO_NAME_EMPTY; /* exit if the macro name is empty */
    if(len > MAX_MACRO_SIZE) 
        return MACRO_NAME_TOO_LONG; /* exit if the macro name is too long */
    
    for (i = 0; i < len; i++) {
        if (!isalnum(macroName[i]) && macroName[i] != '_')
            return MACRO_NAME_INVALID_CHAR; /* exit if the macro name contains invalid characters */
    }
    
    if (isKeywords(macroName))
        return MACRO_NAME_KEYWORD; /* exit if the macro name is a keyword */
    
    if (isMacroExists(table, macroName))
        return MACRO_NAME_EXISTS; /* exit if the macro name already exists */
    
    return MACROTABLE_SUCCESS; /* Macro name is valid */
}

Bool isMacroExists(macroTable* table, char* macroName) {

    macroNode* current = table->macroHead; /* used to iterate through the macro list */
    while (current != NULL) {
        if (strcmp(current->macroName, macroName) == 0) /* check if the macro name matches */
            return TRUE; /* Macro exists */
        current = current->nextMacro;
    }
    
    return FALSE; /* Macro does not exist */
}

void freeMacroTable(macroTable* table) {
    freeMacroNode(table->macroHead); /* free the first macro node */
    free(table);
}

void freeMacroNode(macroNode* node) {    
    
    macroNode* current = node; /* used to iterate through the macro nodes */
    if (node == NULL)
        return; /* exit if the node is NULL */
    
    while (current != NULL) { /* iterate through the macro nodes and free them */
        macroNode* next = current->nextMacro;
        free(current->macroName); /* free the macro name */
        freeMacroBody(current->bodyHead); /* free the body of the macro */
        free(current); /* free the macro node itself */
        current = next;
    }
    
}

void freeMacroBody(macroBody* body) {
    macroBody* current = body; /* used to iterate through the body lines */
    if (body == NULL)
        return;
    
    while (current != NULL) { /* iterate through the body lines and free them */
        macroBody* next = current->nextLine;
        free(current->line);
        free(current);
        current = next;
    }
}