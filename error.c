#include "global.h"
#include "error.h"

char* getErrorMessage(ErrCode code) {
    switch (code) {

        /* VERY GENERAL error codes 0 - 5 */
        case NULL_INITIAL: 
            return "initial state shouldn't be reached.";
        case MALLOC_ERROR_F: 
            return "memory allocation failed.";
        case EXTRANEOUS_TEXT_E:
            return "extraneous text after the end of the line";
        case MALLOC_ERROR_File_Del_F:
            return "memory allocation error while deleting the file.";
        case MALLOC_ERROR_LIST_F:
            return "memory allocation error while adding an error to the list";

        /* debugging errors 8 - 9 */
        case UNKNOWN_ERROR:
            return "unknown error, should never be used.";

        /* util errors 10 - 29 */

        case LINE_TOO_LONG_E:
                return "line length in file is longer than allowed (80).";
        case INPUT_FILE_UNREADABLE_F: 
            return "could not read input .as file, make sure it exists and is readable.";
        case FILE_READ_ERROR_F: 
            return "could not open file to read, make sure it exists and is readable.";
        case FILE_WRITE_ERROR_F: 
            return "file write error.";
        case INVALID_FILE_MODE_F: 
            return "file mode error, should never be used.";
        case FILE_DELETE_ERROR_F: 
            return "could not delete file pls ignore the half built files.";

        /* lexer errors 20 - 39 */
        case INVALID_DIRECTIVE_E:
            return "invalid directive, should be one of the valid directives (.data, .string, .mat, .entry, .extern).";

        case LABEL_INVALID_START_CHAR_E: 
            return "label starts with an invalid character, should start with a letter.";
        case LABEL_INVALID_CHAR_E: 
            return "label contains invalid characters, should only contain letters and digits.";
        case LABEL_TOO_LONG_E: 
            return "label is too long, should be less than 30 characters.";
        case LABEL_EMPTY_E:
            return "label does not have a name, should not be empty.";
        case LABEL_TEXT_AFTER_COLON_E:
            return "text found after label colon, should not have any text after the colon.";
        case LABEL_SAME_AS_MACRO_E:
            return "label and a macro name cannot be the same.";
        case LABEL_NAME_IS_KEYWORD_E:
            return "label name is a keyword, should not be a keyword.";
        case LABEL_ALREADY_EXISTS_E:
            return "label already exists in symbol table.";
        case MACRO_NAME_EXISTS_E:
            return "macro name already exists.";
        case MACRO_NAME_TOO_LONG_E:
            return "macro name is too long.";
        case MACRO_NAME_EMPTY_E:
            return "macro name is empty.";
        case MACRO_INVALID_START_CHAR_E:
            return "macro name starts with an invalid character, should start with a letter.";
        case MACRO_INVALID_CHAR_E:
            return "macro name contains invalid characters.";
        case MACRO_NAME_IS_KEYWORD_E:
            return "macro name is a set keyword by the assembly language.";
        case DIRECTIVE_DATA_MISSING_E:
            return "directive is missing data items, should have at least one data item.";
        case MISSING_COMMA_E:
            return "missing comma between the directive arguments, should have a comma between two data items.";
        case DATA_INVALID_VALUE_E:
            return "data item can only contain numbers.";
        case DATA_ITEM_NOT_INTEGER_E:
            return "doubles are not allowed, please use integers instead.";
        case INTEGER_OUT_OF_RANGE_E:
            return "integer value is out of range of a word (10 bits) should be between -512 and 511.";
        case STR_MISSING_OPEN_QUOTE_E:
            return "string directive is missing an opening quotation mark, should start with a \" character.";
        case STR_MISSING_CLOSE_QUOTE_E:
            return "string directive is missing a closing quotation mark, should end with a \" character.";
        case STR_INVALID_CHAR_E:
            return "string directive contains invalid characters, should only contain ASCII characters.";
        case MAT_INVALID_ROW_E:
            return "matrix has an invalid row, should be in the format .mat[row][col] where row and col are positive integers.";
        case MAT_INVALID_COL_E:
            return "matrix has an invalid column, should be in the format .mat[row][col] where row and col are positive integers.";
        case MAT_SIZE_ZERO_NEG_E:
            return "matrix has a size of zero or negative, should have positive row and column.";
        case MAT_SIZE_TOO_LARGE_E:
            return "matrix input is larger than the size entered, please increase the matrix size or reduce the input.";
        case MISSING_FIRST_OPERAND_E:
            return "missing first operand in the instruction, pls enter the operand.";
        case MISSING_SECOND_OPERAND_E:
            return "missing second operand in the instruction, pls enter the second operand for this instruction.";
        case MISSING_NUM_OPERAND_E:
            return "missing number after '#'.";
        case ONE_OPERAND_COMMA_E:
            return "instruction has one operand there was a comma found, should not have a comma if there is only one operand.";
        case REGISTER_OUT_OF_RANGE_E:
            return "register number is out of range (0-7), should be between 0 and 7.";
        case MAT_INVALID_NAME_START:
            return "matrix name starts with an invalid character, should start with a letter.";
        case MAT_MISSING_FIRST_BRACKET:
            return "matrix is missing the first bracket, should start with a [ character.";
        case MAT_EMPTY_ROW_INDEX:
            return "matrix row index is empty, should have a row index after the [ character.";
        case MAT_MISSING_FIRST_CLOSING_BRACKET:
            return "matrix is missing the first closing bracket, should end with a ] character after the row index.";
        case MAT_MISSING_SECOND_BRACKET:
            return "matrix is missing the second bracket, should have a [ character after the first closing bracket.";
        case MAT_EMPTY_COLUMN_INDEX:
            return "matrix column index is empty, should have a column index after the [ character.";
        case MAT_MISSING_SECOND_CLOSING_BRACKET:
            return "matrix is missing the second closing bracket, should end with a ] character after the column index.";
        
        /* tables errors 50 - 69 */
        case SYMBOL_NAME_EXISTS_E:
            return "symbol name already exists.";

        /* preprocessor errors 70 - 79 */
        case UNMATCHED_MACRO_END_E:
            return "had \"macroend\" without having an opening macro definition.";

        /* firstPass errors 60 - 69 */
        case FIRSTPASS_FAILURE_S:
            return "first pass failed.";

        /* secondPass errors 80 - 89 */
        case SECOND_PASS_SUCCESS_S:
            return "second pass was successful.";
        case SECOND_PASS_FAILURE_S:
            return "second pass failed.";

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
        case MALLOC_ERROR_File_Del_F:
        case MALLOC_ERROR_LIST_F:
            return TRUE;
        default:
            return FALSE; /* all other errors are not fatal */
    }        
}

void printErrorMsg(ErrCode Ecode, const char *stage, unsigned int line)
{
    fprintf(stderr, "Ecode: %d - ", Ecode);
    if (line != 0)
        fprintf(stderr, "line %u ", line);
    if (stage != NULL)
        fprintf(stderr, "at %s stage ", stage);
    
    if (line != 0 || stage != NULL)
        fprintf(stderr, "- ");
        
    fprintf(stderr, "%s\n", getErrorMessage(Ecode));
}

ErrorList* createErrorList(char *filename)
{
    ErrorList* newList = malloc(sizeof(ErrorList));
    if (newList == NULL) 
        return NULL;
    

    newList->count = 0;
    newList->currentLine = 0; /* initialize current line number to 0 */
    newList->fatalError = FALSE; /* initialize fatal error flag to FALSE */
    newList->filename = filename;
    newList->stage = NULL; /* initialize stage to NULL */
    newList->head = NULL;
    newList->tail = NULL;
    return newList;
}

void addErrorToList(ErrorList *list, ErrCode code)
{
    ErrorNode *newNode = malloc(sizeof(ErrorNode));
    list->count++; /* increment the count of errors by 1 for the new error being added */
    if (newNode == NULL) {
        list->fatalError = TRUE; /* set fatal error flag if memory allocation fails */
        list->count++; /* increment the count of errors by 1 for the malloc failure */
        printErrorMsg(code, NULL, list->currentLine); /* print the error message */
        printErrorMsg(MALLOC_ERROR_LIST_F, NULL, list->currentLine); /* print the error message */
        return;
    }

    newNode->errCode = code;
    newNode->line = list->currentLine; /* set the line number of the error */
    newNode->next = NULL;

    if (list->head == NULL) { /* if the list is empty */
        list->head = newNode;
        list->tail = newNode;
    } else { /* if the list is not empty */
        list->tail->next = newNode; /* link the new node to the end of the list */
        list->tail = newNode; /* update the tail to the new node */
    }

    if (isFatalErr(code)) /* if the error is fatal */
        list->fatalError = TRUE; /* set fatal error flag if isFatal is TRUE */
}

void printErrors(ErrorList *list) {
    ErrorNode *curr = list->head;
    fprintf(stderr, "there were %d error(s) in file %s during the %s:\n", list->count, list->filename, list->stage);
    while (curr != NULL) {
        printErrorMsg(curr->errCode, NULL, curr->line); /* print each error message */
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
