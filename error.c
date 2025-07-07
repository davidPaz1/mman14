#include "global.h"
#include "error.h"

char* getErrorMessage(ErrCode code) {
    switch (code) {

        /* VERY GENERAL error codes 0 - 5 */
        case NULL_INITIAL: /* 0 */
            return "initial state shouldn't be reached.";
        case MALLOC_ERROR: /* 1 */
                return "memory allocation failed.";
        case EXTRANEOUS_TEXT: /* 3 */
            return "extraneous text after the end of the line, should never be used.";

        /* debugging errors 6 - 9 */
        case UNKNOWN_ERROR: /* 6 */
            return "unknown error occurred. Should never be used.";
        case UNEXPECTED_NULL_INPUT: /* 7 */
            return "unexpected NULL input, should never be used.";

        /* util errors 10 - 29 */
        case UNKNOWN_LINE_TYPE: /* 12 */
            return "unknown line type found, should be one of the four types (instruction, directive, comment, empty).";
        case LINE_TOO_LONG: /* 13 */
            return "line length in file is longer than allowed (80).";
        case FILE_READ_ERROR: /* 14 */
            return "file read error.";
        case FILE_WRITE_ERROR: /* 15 */
            return "file write error.";
        case INVALID_FILE_MODE: /* 16 */
            return "file mode error, should never be used.";
        case FILE_DELETE_ERROR: /* 17 */
            return "file delete error.";

        /* lexer errors 20 - 39 */
        case LABEL_INVALID_START_CHAR: /* 23 */
            return "label starts with an invalid character, should start with a letter.";
        case LABEL_INVALID_CHAR: /* 24 */
            return "label contains invalid characters, should only contain letters and digits.";
        case LABEL_TOO_LONG: /* 25 */
            return "label is too long, should be less than 30 characters.";
        case LABEL_EMPTY: /* 26 */
            return "label does not have a name, should not be empty.";
        case LABEL_TEXT_AFTER_COLON: /* 27 */
            return "text found after label colon, should not have any text after the colon.";
        case LABEL_SAME_AS_MACRO: /* 28 */
            return "label and a macro name cannot be the same.";
        case INVALID_DIRECTIVE: /* 29 */
            return "invalid directive, should be one of the valid directives (.data, .string, .entry, .extern).";

        /* tables errors 40 - 59 */
        case MACRO_NAME_EXISTS: /* 41 */
            return "macro name already exists.";
        case UNMATCHED_MACRO_END: /* 42 */
            return "had \"macroend\" without having an opening macro definition.";
        case MACRO_NAME_TOO_LONG: /* 43 */
            return "macro name is too long.";
        case MACRO_NAME_EMPTY: /* 44 */
            return "macro name is empty.";
        case MACRO_NAME_INVALID_START_CHAR: /* 45 */
            return "macro name starts with an invalid character, should start with a letter.";
        case MACRO_NAME_INVALID_CHAR: /* 46 */
            return "macro name contains invalid characters.";
        case MACRO_NAME_KEYWORD: /* 47 */
            return "macro name is a set keyword by the assembly language.";

        case SYMBOLTABLE_SUCCESS: /* 50 */
            return "symbol table operation was successful.";
        case SYMBOL_NAME_EXISTS: /* 51 */
            return "symbol name already exists.";


        /* firstPass errors 60 - 69 */
        case FIRSTPASS_FAILURE: /* 71 */
            return "first pass failed.";

        /* it should never reach here */
        default:
            return "unrecognized error code - shouldn't reach this point.";
    }
}

Bool isFatalErr(ErrCode code) {
    switch (code) {
        /* These are clearly fatal errors */
        case MALLOC_ERROR:
        case FILE_READ_ERROR:
        case FILE_WRITE_ERROR:
        case FILE_DELETE_ERROR:
        case UNKNOWN_ERROR:
        case UNEXPECTED_NULL_INPUT:
        case INVALID_FILE_MODE:
        case MACRO_NAME_KEYWORD:
        case LABEL_SAME_AS_MACRO:
        case FIRSTPASS_FAILURE:
        case PREPROCESSOR_FAILURE:
            return TRUE;
        default:
            return FALSE; /* all other errors are not fatal */
    }        
}

void printErrorMsg(ErrCode code, char *context)
{
    fprintf(stderr, "Ecode: %d - ", code);
    if (context[0] == '\0' || context == NULL) /* if context is NULL or empty */
        fprintf(stderr, "%s\n", getErrorMessage(code));
    else
        fprintf(stderr, "Error %s because %s\n", context, getErrorMessage(code));
    if(context == NULL) /* if context is NULL it is due to malloc failure */
        fprintf(stderr, "Error %s\n", getErrorMessage(MALLOC_ERROR));
}

void createErrorList(ErrorList *list)
{
    list->count = 0;
    list->head = NULL;
    list->tail = NULL;
}

int addError(ErrorList *list, ErrCode code, unsigned int line, Bool isFatal)
{

    ErrorNode *newNode = malloc(sizeof(ErrorNode));
    if (newNode == NULL) {
        printErrorMsg(MALLOC_ERROR, "Failed to allocate memory for error node");
        return -1; /* return -1 if memory allocation fails */
    }

    newNode->code = code;
    newNode->line = line;
    newNode->next = NULL;

    if (list->head == NULL) { /* if the list is empty */
        list->head = newNode;
        list->tail = newNode;
    } else { /* if the list is not empty */
        list->tail->next = newNode; /* link the new node to the end of the list */
        list->tail = newNode; /* update the tail to the new node */
    }
    
    list->count++; /* increment the count of errors */
    if (isFatal) 
        list->fatalError = TRUE; /* set fatal error flag if isFatal is TRUE */

    return 0; /* return 0 on success */
}

void printErrors(ErrorList *list, char *filename, char *stage) {
    ErrorNode *curr = list->head;
    fprintf(stderr, "there were %d errors in file %s during the %s:\n", list->count, filename, stage);
    while (curr != NULL) {
        fprintf(stderr, "line %u: %s\n", curr->line, getErrorMessage(curr->code));
        curr = curr->next;
    }
}

void freeErrorsList(ErrorList *list) {
    if (list == NULL)
        return;
    
    ErrorNode *curr = list->head;
    while (curr != NULL) {
        ErrorNode *temp = curr;
        curr = curr->next;
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    free(list);
}
