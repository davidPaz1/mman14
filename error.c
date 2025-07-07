#include "global.h"
#include "error.h"

char* getErrorMessage(ErrCode code) {
    switch (code) {

        /* VERY GENERAL error codes 0 - 5 */
        case NULL_INITIAL: /* 0 */
            return "initial state shouldn't be reached.";
        case MALLOC_ERROR_F: /* 1 */
                return "memory allocation failed.";
        case EXTRANEOUS_TEXT_E: /* 3 */
            return "extraneous text after the end of the line, should never be used.";

        /* debugging errors 6 - 9 */
        case UNKNOWN_ERROR: /* 6 */
            return "unknown error occurred. Should never be used.";
        case UNEXPECTED_NULL_INPUT_F: /* 7 */
            return "unexpected NULL input, should never be used.";

        /* util errors 10 - 29 */

        case LINE_TOO_LONG_E: /* 12 */
                return "line length in file is longer than allowed (80).";
        case INPUT_FILE_UNREADABLE_F: /* 13 */
            return "could not read input file, make sure it exists and is readable.";
        case FILE_READ_ERROR_F: /* 14 */
            return "could not read from file, make sure it exists and is readable.";
        case FILE_WRITE_ERROR_F: /* 15 */
            return "file write error.";
        case INVALID_FILE_MODE_F: /* 16 */
            return "file mode error, should never be used.";
        case FILE_DELETE_ERROR_F: /* 17 */
            return "could not delete file pls ignore the half built files.";

        /* lexer errors 20 - 39 */
        case UNKNOWN_LINE_TYPE_E: /* 23 */
            return "unknown line type found, should be one of the four types (instruction, directive, comment, empty).";
        case LABEL_INVALID_START_CHAR_E: /* 24 */
            return "label starts with an invalid character, should start with a letter.";
        case LABEL_INVALID_CHAR_E: /* 25 */
            return "label contains invalid characters, should only contain letters and digits.";
        case LABEL_TOO_LONG_E: /* 26 */
            return "label is too long, should be less than 30 characters.";
        case LABEL_EMPTY_E: /* 27 */
            return "label does not have a name, should not be empty.";
        case LABEL_TEXT_AFTER_COLON_E: /* 28 */
            return "text found after label colon, should not have any text after the colon.";
        case LABEL_SAME_AS_MACRO_E: /* 29 */
            return "label and a macro name cannot be the same.";
        case LABEL_NAME_IS_KEYWORD_E: /* 30 */
            return "label name is a keyword, should not be a keyword.";
        case INVALID_DIRECTIVE_E: /* 31 */
            return "invalid directive, should be one of the valid directives (.data, .string, .entry, .extern).";

        /* tables errors 40 - 59 */
        case MACRO_NAME_EXISTS_E: /* 41 */
            return "macro name already exists.";
        case UNMATCHED_MACRO_END_E: /* 42 */
            return "had \"macroend\" without having an opening macro definition.";
        case MACRO_NAME_TOO_LONG_E: /* 43 */
            return "macro name is too long.";
        case MACRO_NAME_EMPTY_E: /* 44 */
            return "macro name is empty.";
        case MACRO_INVALID_START_CHAR_E: /* 45 */
            return "macro name starts with an invalid character, should start with a letter.";
        case MACRO_INVALID_CHAR_E: /* 46 */
            return "macro name contains invalid characters.";
        case MACRO_NAME_IS_KEYWORD_E: /* 47 */
            return "macro name is a set keyword by the assembly language.";

        case SYMBOLTABLE_SUCCESS_S: /* 50 */
            return "symbol table operation was successful.";
        case SYMBOL_NAME_EXISTS_E: /* 51 */
            return "symbol name already exists.";


        /* firstPass errors 60 - 69 */
        case FIRSTPASS_FAILURE_S: /* 71 */
            return "first pass failed.";

        /* it should never reach here */
        default:
            return "unrecognized error code - shouldn't reach this point.";
    }
}

Bool isFatalErr(ErrCode code) {
    switch (code) {
        /* These are clearly fatal errors */
        case UNKNOWN_ERROR: /* is for debugging purposes, should not be used in production */
        case NULL_INITIAL: /* is for debugging purposes, should not be used in production */
        case MALLOC_ERROR_F:
        case INPUT_FILE_UNREADABLE_F:
        case FILE_READ_ERROR_F:
        case FILE_WRITE_ERROR_F:
        case FILE_DELETE_ERROR_F:
        case INVALID_FILE_MODE_F:
        case UNEXPECTED_NULL_INPUT_F:
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
        fprintf(stderr, "Error %s\n", getErrorMessage(MALLOC_ERROR_F));
}

void createErrorList(ErrorList *list, char *filename)
{
    list->count = 0;
    list->fatalError = FALSE; /* initialize fatal error flag to FALSE */
    list->filename = filename;
    list->stage = NULL; /* initialize stage to NULL */
    list->head = NULL;
    list->tail = NULL;
}

int addError(ErrorList *list, ErrCode code, unsigned int line, Bool isFatal)
{
    ErrorNode *newNode = malloc(sizeof(ErrorNode));
    if (newNode == NULL) {
        list->fatalError = TRUE; /* set fatal error flag if memory allocation fails */
        list->count++; /* increment the count of errors */
        printErrors(list, 1); /* print errors with unknown filename */
        printErrorMsg(MALLOC_ERROR_F, "Failed to allocate memory for error node");
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

void printErrors(ErrorList *list, unsigned int incCount) {
    ErrorNode *curr = list->head;
    list->count += incCount; /* increment the count of errors by incCount (will be used mainly for malloc failures) */
    fprintf(stderr, "there were %d errors in file %s during the %s:\n", list->count, list->filename, list->stage);
    while (curr != NULL) {
        fprintf(stderr, "line %u: %s\n", curr->line, getErrorMessage(curr->code));
        curr = curr->next;
    }
}

void freeErrorsList(ErrorList *list) {
    ErrorNode* curr;
    if (list == NULL)
        return;
    
    curr = list->head;
    while (curr != NULL) {
        ErrorNode *temp = curr;
        curr = curr->next;
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
    list->stage = NULL; /* we don't need to free the stage string because it is a constant string */
    list->filename = NULL; /* we don't need to free the filename string because it is a argv[i] string */
    free(list);
}
