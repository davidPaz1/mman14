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
    newTable->macroCount = 0; /* initialize the macro count to 0 */
    newTable->head = NULL; /* initialize the head of the list to NULL */
    *errorCode = MACROTABLE_SUCCESS; /* set error code to MACROTABLE_SUCCESS */
    return newTable; /* return the new table */
}

macroNode* createMacroNode(char* macroName, ErrCode* errorCode) {
    macroNode* newMacro; /* returned macro */
    *errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    if (macroName == NULL) { /*test123*/
        fprintf(stderr, "invalid macro name\n");
        *errorCode = UNKNOWN_ERROR;
        return NULL; /* exit if the name is NULL */
    }

    if (!isMacroNameValid(macroName)) {
        fprintf(stderr, "Invalid macro name: %s\n", macroName);
        *errorCode = MACRO_NAME_INVALID;
        return NULL; /* exit if the macro name is not valid */
    }

    newMacro = malloc(sizeof(macroNode));
    if (newMacro == NULL) {
        *errorCode = MALLOC_ERROR; /* set error code to MALLOC_ERROR */
        return NULL; /* exit if memory allocation fails */
    }

    newMacro->macroName = strDup(macroName);
    if (newMacro->macroName == NULL) { /*test123*/
        *errorCode = MALLOC_ERROR; /* set error code to MALLOC_ERROR */
        free(newMacro);
        return NULL; /* exit if memory allocation fails */
    }

    newMacro->bodyHead = NULL; /* set the body head to the first line in the body */
    newMacro->bodyTail = NULL; /* set the body tail to the last line in the body */
    newMacro->lineCount = 0; /* set the line count */
    newMacro->nextMacro = NULL; /* initialize the next macro pointer to NULL */
    return newMacro;
}

macroBody* createMacroBody(char* line) {
    macroBody* newBody; /* returned body */
    if (line == NULL) { /*test123*/
        fprintf(stderr, "invalid macro line\n");
        return NULL; /* exit if the line is NULL */
    }
    
    newBody = malloc(sizeof(macroBody));
    if (newBody == NULL) {
        fprintf(stderr, "Memory allocation failed for macro body\n");
        return NULL; /* exit if memory allocation fails */
    }

    newBody->line = strDup(line); /* duplicate the line to avoid aliasing */
    if (newBody->line == NULL) {
        fprintf(stderr, "Memory allocation failed for macro body line\n");
        free(newBody);
        return NULL;
    }
    newBody->nextLine = NULL;

    return newBody;
}

/* add a new macro to the table */
ErrCode addMacro(macroTable* table , char* name) {
    macroNode* newMacro; /* new macro to be added */
    ErrCode errorCode = NULL_INITIAL; /* initialize error code to NULL_INITIAL */

    if (table == NULL || name == NULL) { /*test123*/
        fprintf(stderr, "invalid macro table or line\n");
        return UNKNOWN_ERROR; /* exit if the table or line is NULL */
    }

    newMacro = createMacroNode(name, &errorCode);
    if (newMacro == NULL) {
        return errorCode; /* exit if memory allocation fails */
    }

    newMacro->nextMacro = table->head; /* insert at the beginning of the list */
    table->head = newMacro; /* update the head of the list */
    table->macroCount++; /* increment the macro count */
    return MACROTABLE_SUCCESS; /* return success */
}

/* add to the last entered macro the new line*/
ErrCode addMacroLine(macroTable* table, char* line) {
    macroBody* newLine; /* new line to be added */
    macroNode* macrohead; /* the macro to which the line will be added */
    if (table == NULL || line == NULL) { /*test123*/
        fprintf(stderr, "invalid macro table or line\n");
        return UNKNOWN_ERROR; /* exit if the table or line is NULL */
    }
    macrohead = table->head; /* get the head of the macro list */
    
    newLine = createMacroBody(line); /* create a new body for the line */
    if (newLine == NULL) {
        return MALLOC_ERROR; /* exit if memory allocation fails */
    }

    if (macrohead->bodyHead == NULL) { /* if this is the first line in the macro */
        macrohead->bodyHead = newLine; /* set the head of the body */
        macrohead->bodyTail = newLine; /* set the tail of the body */
    } else {
        macrohead->bodyTail->nextLine = newLine; /* link the new line to the end of the body */
        macrohead->bodyTail = newLine; /* update the tail of the body */
    }

    macrohead->lineCount++; /* increment the line count */
    return MACROTABLE_SUCCESS; /* return success */
}

macroBody* findMacro(macroTable* table, char* macroName) {
    macroNode* current; /* used to iterate through the macro list */
    if (table == NULL || macroName == NULL) {
        fprintf(stderr, "invalid macro table or macro name\n");
        return NULL; /* exit if the table or macro name is NULL */
    }
    
    current = table->head;
    while (current != NULL) {
        if (strcmp(current->macroName, macroName) == 0) {
            return current->bodyHead; /* return the body of the found macro */
        }
        current = current->nextMacro;
    }
    
    return NULL; /* Macro not found */
}

Bool isMacroNameValid(char* macroName) {
    int i; /* index for iterating through the macro name */
    if (strlen(macroName) == 0 || strlen(macroName) > MAX_MACRO_SIZE) {
        fprintf(stderr, "Macro name must be between 1 and 30 characters long\n");
        return FALSE; /* exit if the macro name is not valid */
    }
    
    for (i = 0; i < strlen(macroName); i++) {
        if (!isalnum(macroName[i]) && macroName[i] != '_') {
            fprintf(stderr, "Macro name can only contain alphanumeric characters and underscores\n");
            return FALSE; /* exit if the macro name contains invalid characters */
        }
    }
    
    if (isKeywords(macroName)) { /*test123*/
        fprintf(stderr, "Macro name cannot be a keyword\n");
        return FALSE; /* exit if the macro name is a keyword */
    }

    if (isMacroExists(NULL, macroName)) { /*test123*/
        fprintf(stderr, "Macro name already exists\n");
        return FALSE; /* exit if the macro name already exists */
    }

    return TRUE; /* Macro name is valid */
}

Bool isMacroExists(macroTable* table, char* macroName) {
    macroNode* current; /* used to iterate through the macro list */
    if (table == NULL || macroName == NULL) { /*test123*/
        fprintf(stderr, "invalid macro table or macro name\n");
        return FALSE; /* exit if the table or macro name is NULL */
    }
    
    current = table->head;
    while (current != NULL) {
        if (strcmp(current->macroName, macroName) == 0) {
            return TRUE; /* Macro exists */
        }
        current = current->nextMacro;
    }
    
    return FALSE; /* Macro does not exist */
}

void freeMacroTable(macroTable* table) {
    macroNode* current; /* used to iterate through the macro nodes */
    if (table == NULL) { /*test123*/
        fprintf(stderr, "invalid macro table\n");
        return;
    }

    current = table->head;
    while (current != NULL) { /* iterate through the macro nodes and free them */
        macroNode* next = current->nextMacro;
        freeMacroNode(current);
        current = next;
    }
    
    free(table);
}

void freeMacroNode(macroNode* node) {
    if (node == NULL) { /*test123*/
        fprintf(stderr, "invalid macro node\n");
        return;
    }
    
    free(node->macroName); /* free the macro name */
    freeMacroBody(node->bodyHead); /* free the body of the macro */
    free(node); /* free the macro node itself */
}

void freeMacroBody(macroBody* body) {
    macroBody* current; /* used to iterate through the body lines */
    if (body == NULL) { /*test123*/
        fprintf(stderr, "invalid macro body\n");
        return;
    }
    current = body;
    while (current != NULL) { /* iterate through the body lines and free them */
        macroBody* next = current->nextLine;
        free(current->line);
        free(current);
        current = next;
    }
}