#include "tables.h"
#include "global.h"
#include "error.h"
#include "lexer.h" /* for isMacroNameValid */
#include "util.h" /* for strDup */

/* MacroTable functions that operate on the MacroTable struct (linked list)*/
macroTable* createMacroTable(ErrCode* errorCode) {
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
    *errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

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

ErrCode addMacroLine(macroTable* table, char* line) {
    macroBody* newLine; /* new line to be added */
    macroNode* headNode; /* the macro to which the line will be added */
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    if (table == NULL || line == NULL) { /*test123*/
        fprintf(stderr, "invalid macro table or line\n");
        return UNEXPECTED_NULL_INPUT; /* exit if the table or line is NULL */
    }
    headNode = table->macroHead; /* get the head of the macro list */
    
    newLine = createMacroBody(line, &errorCode); /* create a new body for the line */
    if (errorCode != MACROTABLE_SUCCESS) 
        return errorCode; /* exit if memory allocation fails */
    

    if (headNode->bodyHead == NULL) { /* if this is the first line in the macro */
        headNode->bodyHead = newLine; /* set the head of the body */
        headNode->bodyTail = newLine; /* set the tail of the body */
    } else {
        headNode->bodyTail->nextLine = newLine; /* link the new line to the end of the body */
        headNode->bodyTail = newLine; /* update the tail of the body */
    }

    return MACROTABLE_SUCCESS; /* return success */
}

macroBody* findMacro(macroTable* table, char* macroName, ErrCode* errorCode) {
    macroNode* current; /* used to iterate through the macro list */
    *errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    current = table->macroHead;
    while (current != NULL) {
        if (strcmp(current->macroName, macroName) == 0) {
            *errorCode = MACROTABLE_SUCCESS; /* set error code to success */
            return current->bodyHead; /* return the body of the found macro */
        }
        current = current->nextMacro;
    }

    *errorCode = UNKNOWN_ERROR; /* it should not reach here because findMacro is called only after isMacroExists */
    return NULL; /* Macro not found */
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
    if (table == NULL) /* check if the table is NULL */
        return; /* exit if the table is NULL */
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