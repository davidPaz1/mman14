#include "global.h"
#include "macroTable.h"
#include "util.h"

/* MacroTable functions that operate on the MacroTable struct (linked list)*/
macroTable* createMacroTable(char* macroName , char* line) {
    macroTable* newTable; /* returned table */
    if (macroName == NULL || line == NULL) { /*test123*/
        fprintf(stderr, "invalid macro name or line\n");
        return NULL; /* exit if the name or line is NULL */
    }
    
    newTable = malloc(sizeof(macroTable));
    if (newTable == NULL) { /*testMalloc*/
        fprintf(stderr, "Memory allocation failed for macro table\n");
        return NULL; /* exit if memory allocation fails */
    }
    
    newTable->head = createMacroNode(macroName, line); /* create the first macro node */

    if (newTable->head == NULL) {
        free(newTable);
        return NULL; /* exit if memory allocation fails */
    }
    newTable->macroCount = 1; /* initialize the macro count to 1 */
    return newTable; /* return the new macro table */
}

macroNode* createMacroNode(char* macroName , char* line){
    macroNode* newMacro; /* returned macro */
    MacroBody* newBody; /* body of the macro */
    if (macroName == NULL || line == NULL) { /*test123*/
        fprintf(stderr, "invalid macro name or line\n");
        return NULL; /* exit if the name or line is NULL */
    }
    
    newMacro = malloc(sizeof(macroNode));
    if (newMacro == NULL) { /*testMalloc*/
        fprintf(stderr, "Memory allocation failed for macro node\n");
        return NULL; /* exit if memory allocation fails */
    }
    
    newMacro->macroName = strDup(macroName);
    if (newMacro->macroName == NULL) { /*test123*/
        fprintf(stderr, "Memory allocation failed for macro name\n");
        free(newMacro);
        return NULL; /* exit if memory allocation fails */
    }
    
    newBody = createMacroBody(line);
    if (newBody == NULL) {
        free(newMacro->macroName);
        free(newMacro);
        return NULL; /* exit if memory allocation fails */
    }
    
    newMacro->bodyHead = newBody;
    newMacro->bodyTail = newBody;
    newMacro->lineCount = 1;

    return newMacro;
}

MacroBody* createMacroBody(char* line) {
    MacroBody* newBody; /* returned body */
    if (line == NULL) { /*test123*/
        fprintf(stderr, "invalid macro line\n");
        return NULL; /* exit if the line is NULL */
    }
    
    newBody = malloc(sizeof(MacroBody));
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

MacroErrorCode addMacro(macroTable* table , char* name, char* line) {
    macroNode* newMacro; /* new macro node to be added */
    if (table == NULL || name == NULL || line == NULL) { /*test123*/
        fprintf(stderr, "invalid macro table, name or line\n");
        return MACRO_MALLOC_ERROR; /* exit if the table, name or line is NULL */
    }

    
    if (isMacroNameValid(name)) { /* if the macro already exists */
        return MACRO_NAME_EXISTS;
    }
    
    /* if the macro does not exist, create a new macro node */
    newMacro = createMacroNode(name, line); 
    if (newMacro == NULL) {
        return MACRO_MALLOC_ERROR; /* exit if memory allocation fails */
    }
    
    newMacro->nextMacro = table->head; /* insert at the beginning of the list */
    table->head = newMacro; /* update the head of the list */
    table->macroCount++; /* increment the macro count */
    return MACRO_SUCCESS; /* return success */
}

MacroBody* findMacro(macroTable* table, char* macroName) {
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
    if (strlen(macroName) == 0 || strlen(macroName) > 31) { /*test123*/
        fprintf(stderr, "Macro name must be between 1 and 31 characters long\n");
        return FALSE; /* exit if the macro name is not valid */
    }
    
    for (i = 0; i < strlen(macroName); i++) {
        if (!isalnum(macroName[i]) && macroName[i] != '_') {
            fprintf(stderr, "Macro name can only contain alphanumeric characters and underscores\n");
            return FALSE; /* exit if the macro name contains invalid characters */
        }
    }
    
    if (isOperationName(macroName)) { /*test123*/
        fprintf(stderr, "Macro name cannot be an operation name\n");
        return FALSE; /* exit if the macro name is an operation name */
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

void freeMacroBody(MacroBody* body) {
    MacroBody* current; /* used to iterate through the body lines */
    if (body == NULL) { /*test123*/
        fprintf(stderr, "invalid macro body\n");
        return;
    }
    current = body;
    while (current != NULL) { /* iterate through the body lines and free them */
        MacroBody* next = current->nextLine;
        free(current->line);
        free(current);
        current = next;
    }
}