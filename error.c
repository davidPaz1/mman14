#include "global.h"
#include "error.h"

char* getErrorMessage(ErrCode code)
{
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
        case LABEL_EMPTY_LINE_E:
            return "the line is empty after the label pls remove the label or add some text after it.";
        case INVALID_DIRECTIVE_E:
            return "invalid directive, should be one of the valid directives (.data, .string, .mat, .entry, .extern).";
        case MACRO_AFTER_LABEL_E: 
            return "can not have a macro after a label, please remove the macro from the line.";
        case MACRO_DEF_AFTER_LABEL_E:
            return "macro definition found after label, please move it before the label.";
        case MACRO_END_AFTER_LABEL_E:
            return "macro end found after label, please move it before the label.";
        case UNKNOWN_LINE_TYPE_E:
            return "unknown line type found, pls correct the line to be a valid line type.";
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
        case LABEL_NAME_IS_REGISTER_E:
            return "label name is a register, should not be a register.";
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
        case MACRO_NAME_IS_REGISTER_E:
            return "macro name is a register, pls use a different name.";
        case DIRECTIVE_DATA_MISSING_E:
            return "directive is missing data items, should have at least one data item.";
        case DATA_MISSING_NUMBERS_E:
            return "directive is missing a number, cannot have two commas without a number between them.";
        case DATA_INVALID_VALUE_E:
            return "data item can only contain numbers.";
        case DATA_COMMA_BETWEEN_NUMS_E:
            return "missing comma between the directive arguments, should have a comma between two data items.";
        case DATA_EXTRANEOUS_TEXT_E:
            return "extraneous text found after the data item, should only contain numbers.";
        case DATA_ITEM_NOT_INTEGER_E:
            return "doubles are not allowed, please use integers instead.";
        case INTEGER_OUT_OF_RANGE10_BITS_E:
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
        case ONE_OPERAND_COMMA_E:
            return "instruction has one operand but a comma was found, should not have a comma if there is only one operand.";
        case MISSING_SECOND_OPERAND_E:
            return "missing second operand in the instruction, pls enter the second operand for this instruction.";
        case THIRD_OPERAND_DETECTED_E:
            return "found another comma after the second operand.";
        case MISSING_NUM_OPERAND_E:
            return "missing number after '#'.";
        case NUMBER_OPERAND_IS_NOT_INTEGER_E:
            return "number is not an integer, should be an integer.";
        case INTEGER_OPERAND_OUT_OF_RANGE8_BITS_E:
            return "integer value is out of range of a byte (8 bits) should be between -128 and 127.";
        case REGISTER_NO_R_E:
            return "register does not start with 'r', should start with 'r' followed by a digit.";
        case REGISTER_NO_DIGIT_E:
            return "register does not have a digit after 'r', should be in the format r0, r1, r2, ..., r7.";
        case REGISTER_OUT_OF_RANGE_E:
            return "register number is out of range (0-7), should be between 0 and 7.";
        case OPERAND_EXTRANEOUS_TEXT_E:
            return "operand has extraneous text, should not have any text after the operand name.";
        case MAT_EMPTY_ROW_INDEX_E:
            return "matrix row index is empty, should have a row index after the [ character.";
        case MAT_MISSING_FIRST_CLOSING_BRACKET_E:
            return "matrix is missing the first closing bracket, should end with a ] character after the row index.";
        case MAT_MISSING_SECOND_BRACKET_E:
            return "matrix is missing the second bracket, should have a [ character after the first closing bracket.";
        case MAT_EMPTY_COLUMN_INDEX_E:
            return "matrix column index is empty, should have a column index after the [ character.";
        case MAT_MISSING_SECOND_CLOSING_BRACKET_E:
            return "matrix is missing the second closing bracket, should end with a ] character after the column index.";
        case MAT_ROW_NOT_REGISTER_N:
            return "note the previous error was about the matrix row.";
        case MAT_COL_NOT_REGISTER_N:
            return "note the previous error was about the matrix column.";
        case OPERAND_IS_KEYWORD_E:
            return "operand can not be a keyword (e.g. mov, .data , mcroend).";
        case OPERAND_IS_MACRO_E:
            return "operand can not be a macro, should be a register, number, label or matrix.";
        case OPERAND1_ERROR_N:
            return "the last error was an error in the first operand";
        case OPERAND2_ERROR_N:
            return "the last error was an error in the second operand";
        case OPERAND1_UNKNOWN_E:
            return "first operand is unknown, should be a register, number, label or matrix.";
        case OPERAND2_UNKNOWN_E:
            return "second operand is unknown, should be a register, number, label or matrix.";
        case OPERAND1_LABEL_DOES_NOT_EXIST_E:
            return "first operand is an unknown operand (though it was a label but it is not in the symbol table).";
        case OPERAND2_LABEL_DOES_NOT_EXIST_E:
            return "second operand is an unknown operand (though it was a label but it is not in the symbol table).";
        case OPERAND1_NON_MAT_SYMBOL_E:
            return "first operand has a matrix syntax but a non-matrix symbol used.";
        case OPERAND2_NON_MAT_SYMBOL_E:
            return "second operand has a matrix syntax but a non-matrix symbol used.";
        case INSTRUCTION_SRC_OP_CANT_REGISTER_E:
            return "lea source operand cannot be a register, pls enter a matrix or a label.";
        case INSTRUCTION_SRC_OP_CANT_NUM_E:
            return "lea source operand cannot be a number, pls enter a matrix or a label.";
        case INSTRUCTION_DST_OP_CANT_NUM_E:
            return "instruction destination operand cannot be a number, pls enter a register, matrix or a label.";

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
        case ENTRY_LABEL_DOES_NOT_EXIST_E:
            return "entry label does not exist in Symbol Table, should be defined before use.";

        /* it should never reach here */
        default:
            return "unrecognized error code - shouldn't reach this point.";
    }
}

Bool isFatalErr(ErrCode code)
{
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

Bool isNoteErr(ErrCode code)
{
    switch (code) {
        case OPERAND1_ERROR_N:
        case OPERAND2_ERROR_N:
        case MAT_ROW_NOT_REGISTER_N:
        case MAT_COL_NOT_REGISTER_N:
            return TRUE; /* these are note errors */
        default:
            return FALSE; /* all other errors are not note errors */
    }
}

void printErrorMsg(ErrCode code, const char *stage, unsigned int line)
{
    if (isFatalErr(code))
        fprintf(stderr, "FATAL ERROR: ");
    fprintf(stderr, "code: %d - ", code);
    if (line != 0)
        fprintf(stderr, "line %u ", line);
    if (stage != NULL)
        fprintf(stderr, "at %s stage ", stage);
    
    if (line != 0 || stage != NULL)
        fprintf(stderr, "- ");
        
    fprintf(stderr, "%s\n", getErrorMessage(code));
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

    if (isNoteErr(code)) {
        newNode->line = 0; /* will fix the printing of note error message */
        list->count--; /* do not count note errors */
    }
}

void printErrors(ErrorList *list)
{
    ErrorNode *curr = list->head;
    fprintf(stderr, "there were %d error(s) in file %s during the %s:\n", list->count, list->filename, list->stage);
    while (curr != NULL) {
        if (!isNoteErr(curr->errCode))
            fprintf(stderr, "\n"); /* print a new line for better readability */

        printErrorMsg(curr->errCode, NULL, curr->line); /* print each error message */
        curr = curr->next;
    
    }
}

void freeErrorsList(ErrorList *list)
{
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
