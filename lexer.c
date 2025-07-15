#include "lexer.h"
#include "global.h"
#include "error.h"
#include "util.h"
#include "tables.h"

/* readParsedLine - reads a line from the file and returns a parsedLine structure
 * errorCode:  EOF_REACHED_S , MALLOC_ERROR_F, UTIL_SUCCESS_S
 */
parsedLine* readParsedLine(FILE *fp, ErrCode *errorCode, MacroTable *table, ErrorList *errorList)
{
    parsedLine* pLine;
    char* line;

    line = readLine(fp, errorCode); /* we will delete the parsed parts from this line */
    if (*errorCode != UTIL_SUCCESS_S)  /* if an error occurred while reading the line */
        return NULL;

    pLine = malloc(sizeof(parsedLine));
    if (pLine == NULL) {
        *errorCode = MALLOC_ERROR_F;
        free(line);
        return NULL;
    }

    if (isEndOfLine(line)) { /* if the line is empty */
        free(line);
        pLine->typesOfLine = EMPTY_LINE; /* set the type of line to EMPTY_LINE */
        *errorCode = LEXER_SUCCESS_S; /* return success */
        return pLine;
    }
    if (line[0] == ';') { /* if the line starts with ';', it is a comment */
        free(line);
        pLine->typesOfLine = COMMENT_LINE;
        *errorCode = LEXER_SUCCESS_S; /* return success */
        return pLine; /* return the parsed line with type COMMENT_LINE */
    }

    memset(&pLine->lineContentUnion, 0, sizeof(pLine->lineContentUnion)); /* initialize the union to zero */

    /* get the label from the line */
    *errorCode = getLabelFromLine(pLine, line, table, errorList);
    if (*errorCode != LEXER_SUCCESS_S) { /* if an error occurred while getting the label */
        freeParsedLine(pLine);
        free(line);
        return NULL;
    }

    *errorCode = determineLineType(pLine, line); /* determine the type of the line */
    if (*errorCode != LEXER_SUCCESS_S) {
        freeParsedLine(pLine);
        free(line); 
        return NULL; 
    }

    if (pLine->typesOfLine == COMMENT_LINE || pLine->typesOfLine == EMPTY_LINE){
        *errorCode = LEXER_SUCCESS_S;
        return pLine; /* if the line is a comment or an empty line, return it */
    }

    if (pLine->typesOfLine == DIRECTIVE_LINE) {
        *errorCode = parseDirectiveLine(pLine, line, errorList);
        if (*errorCode != LEXER_SUCCESS_S) { /* if an error occurred while parsing the directive line */
            freeParsedLine(pLine);
            free(line);
            return NULL; /* return NULL if an error occurred */
        }
    } else if (pLine->typesOfLine == INSTRUCTION_LINE) {
        *errorCode = parseInstructionLine(pLine, line, errorList); /* parse the instruction line */
        if (*errorCode != LEXER_SUCCESS_S) { /* if an error occurred while parsing the instruction line */
            freeParsedLine(pLine);
            free(line);
            return NULL; /* return NULL if an error occurred */
        }
    }
    free(line); 
    return pLine;
}

ErrCode getLabelFromLine(parsedLine *pLine, char *line, MacroTable *macroNames, ErrorList *errorList)
{
    char *token;
    ErrCode errorCode = NULL_INITIAL;
    
    token = getFirstToken(line, &errorCode);
    if (errorCode != UTIL_SUCCESS_S) 
        return errorCode;

    if (!isLabel(token)){
        pLine->label = NULL; /* if the first token is not a label, set the label to NULL */
        free(token);
        return LEXER_SUCCESS_S; /* if the first token is not a label, return success */
    }

    errorCode = isValidLabel(macroNames, token);
    if(errorCode != LEXER_SUCCESS_S) {
        addErrorToList(errorList, errorCode); /* add the error to the error list */
        free(token); /* free the allocated memory for the token */
        return LEXER_FAILURE_S;
    }

    pLine->label = token; /* set the label in the parsedLine structure */
    cutnChar(line, strlen(token)); /* cut the label from the line */
    return LEXER_SUCCESS_S;
}

ErrCode determineLineType(parsedLine *pLine, char *line) 
{
    char *token;
    ErrCode errorCode = NULL_INITIAL; /* reset error code to initial state */

    pLine->typesOfLine = UNSET_LINE; /* initialize the type of line to UNSET_LINE */

    token = cutFirstToken(line, &errorCode); /* cut the first token from the line */
    if (errorCode == MALLOC_ERROR_F)
        return MALLOC_ERROR_F;
    
    if (isOperationName(token)){   /* if the line is an instruction */
        pLine->typesOfLine = INSTRUCTION_LINE;
        pLine->lineContentUnion.instruction.operationName = token; /* set the operation name */
        return LEXER_SUCCESS_S;
    }

    if (isDirective(token)){
        pLine->typesOfLine = DIRECTIVE_LINE; /* if the line is a directive */
        pLine->lineContentUnion.directive.directiveName = token; /* set the directive name */
        return LEXER_SUCCESS_S;
    }

    if (token[0] == '.') /* if the line contains a dot, it is unknown directive */
        return INVALID_DIRECTIVE_E; 
    
    return UNKNOWN_LINE_TYPE_E; /* the line is not an instruction or a directive */
}

ErrCode parseDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList){

    pLine->lineContentUnion.directive.dataCount = 0;
    pLine->lineContentUnion.directive.dataItems = NULL;
    pLine->lineContentUnion.directive.directiveLabel = NULL;

    if(isEndOfLine(line)){ /* if the directive has no data */
        addErrorToList(errorList, DIRECTIVE_DATA_MISSING_E);
        return LEXER_FAILURE_S;
    }

    if (strcmp(pLine->lineContentUnion.directive.directiveName, ".data") == 0) 
        return parseDataDirectiveLine(pLine, line, errorList);
    
    if (strcmp(pLine->lineContentUnion.directive.directiveName, ".string") == 0) 
        return parseStrDirectiveLine(pLine, line, errorList);
    
    if (strcmp(pLine->lineContentUnion.directive.directiveName, ".mat") == 0) 
      return parseMatDirectiveLine(pLine, line, errorList);

    if (strcmp(pLine->lineContentUnion.directive.directiveName, ".entry") == 0) 
        return parseEntryDirectiveLine(pLine, line, errorList);

    if (strcmp(pLine->lineContentUnion.directive.directiveName, ".extern") == 0) 
        return parseExternDirectiveLine(pLine, line, errorList);

    return INVALID_DIRECTIVE_E; /* should not reach here */
}

ErrCode parseDataDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList)
{
    char *token, *endPtr;
    unsigned int startErrCount = errorList->count; /* save the current error count to check if any errors were added */
    unsigned int dataCount = 0, arrSize = INITIAL_DATA_ITEMS_SIZE; /* arrSize is the initial size of the dataItems array */
    
    int* dataItems = malloc(sizeof(int) * arrSize);
    if (dataItems == NULL) {
        addErrorToList(errorList, MALLOC_ERROR_F);
        return LEXER_FAILURE_S;
    }

    token = strtok(line, ",");

    while (token != NULL) {
        double value;

        while (isspace(*token)) /* skip leading whitespace */
            token++;

        endPtr = token; /* set endPtr to the start of the token */
        value = strtod(token, &endPtr);

        while (isspace(*endPtr)) /* skip trailing whitespace */
            endPtr++;

        if (endPtr == token) /* if no number was found */
            addErrorToList(errorList, DATA_INVALID_VALUE_E);
        else if (*endPtr != '\0') /* if there are non-numeric and non-whitespace characters after the number */
            addErrorToList(errorList, MISSING_COMMA_E);
        else if (value != (int)value) /* if the value is not an integer */
            addErrorToList(errorList, DATA_ITEM_NOT_INTEGER_E);
        else if (!isValidInteger((int)value)) /* check if the integer value is valid for the assembler */
            addErrorToList(errorList, INTEGER_OUT_OF_RANGE_E);        

        if (dataCount >= arrSize) { /* resize array if needed */
            int *temp;
            arrSize *= 2;
            temp = realloc(dataItems, sizeof(int) * arrSize);
            if (temp == NULL) {
                free(dataItems);
                addErrorToList(errorList, MALLOC_ERROR_F);
                return LEXER_FAILURE_S;
            }
            dataItems = temp;
        }

        dataItems[dataCount++] = (int)value;
        token = strtok(NULL, ","); /* get next token */
    } /* end of while */

    if (startErrCount < errorList->count) { /* if new errors were added */
        free(dataItems);
        return LEXER_FAILURE_S;
    }

    pLine->lineContentUnion.directive.dataItems = dataItems;
    pLine->lineContentUnion.directive.dataCount = dataCount;

    return LEXER_SUCCESS_S;
}

ErrCode parseStrDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList)
{
    ErrCode errorCode = NULL_INITIAL;
    unsigned int startErrCount = errorList->count; /* save the current error count to check if any errors were added */
    unsigned int i, arrSize = INITIAL_DATA_ITEMS_SIZE;
    char *startQuote, *endQuote;
    int* dataItems = malloc(sizeof(int) * INITIAL_DATA_ITEMS_SIZE);
    if (dataItems == NULL) {
        addErrorToList(errorList, MALLOC_ERROR_F);
        return LEXER_FAILURE_S;
    }

    if (errorCode == MALLOC_ERROR_F) { /* if memory allocation failed */
        free(dataItems);
        addErrorToList(errorList, errorCode);
        return LEXER_FAILURE_S;
    }
    
    startQuote = strchr(line, '"'); /* find the first quote */
    endQuote = strrchr(line, '"'); /* find the last quote */

    if (startQuote == NULL){ /* if didn't find any quote */
        addErrorToList(errorList, STR_MISSING_OPEN_QUOTE_E);
        addErrorToList(errorList, STR_MISSING_CLOSE_QUOTE_E);
    }
    else if (startQuote != line) /* if the first character is not '"' */
        addErrorToList(errorList, STR_MISSING_OPEN_QUOTE_E);
    else if (endQuote != line + strlen(line) - 1) /* if the last character is not '"' */
        addErrorToList(errorList, STR_MISSING_CLOSE_QUOTE_E);

    for (i = 1; i < strlen(line) - 1; i++)
    {
        if (line[i] == '"') /* if the string contains a quote in the middle */
            addErrorToList(errorList, STR_QUOTE_IN_MIDDLE_E);
        if(!isAscii(line[i])) /* if the first character is not a valid ASCII character */
            addErrorToList(errorList, STR_INVALID_CHAR_E);
        
        if (i - 1 >= arrSize) { /* resize array if needed */
            int *temp;
            arrSize *= 2;
            temp = realloc(dataItems, sizeof(int) * arrSize);
            if (temp == NULL) {
                free(dataItems);
                addErrorToList(errorList, MALLOC_ERROR_F);
                return LEXER_FAILURE_S;
            }
            dataItems = temp;
        }
        
        dataItems[i - 1] = (int)line[i]; /* store the character in the dataItems array */
    }

    if (i - 1 >= arrSize) { /* resize array if needed */
            int *temp;
            arrSize += 1; /* add one more space for the null terminator */
            temp = realloc(dataItems, sizeof(int) * arrSize);
            if (temp == NULL) {
                free(dataItems);
                addErrorToList(errorList, MALLOC_ERROR_F);
                return LEXER_FAILURE_S;
            }
            dataItems = temp;
    }
    dataItems[i - 1] = '\0'; /* add null terminator to the end of the string */
    if (startErrCount < errorList->count){ /* if new errors were added */ 
        free(dataItems);
        return LEXER_FAILURE_S;
    } 
    
    pLine->lineContentUnion.directive.dataItems = dataItems; /* set the data items in the parsed line */
    pLine->lineContentUnion.directive.dataCount = i; /* set the data count in the parsed line */
    return LEXER_SUCCESS_S;
}

ErrCode parseMatDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList)
{
    ErrCode errorCode = NULL_INITIAL;
    unsigned int startErrCount = errorList->count; /* save the current error count to check if any errors were added */
    unsigned int dataCount = 0, i = 0; 
    int row = 0, col = 0;
    char *token, *endPtr;
    int* dataItems = NULL;
    
    token = cutFirstToken(line, &errorCode); /* should be "[row]"*/
    if (errorCode != UTIL_SUCCESS_S) {
        addErrorToList(errorList, errorCode);
        return LEXER_FAILURE_S;
    }

    /* check if start and end characters are correct and parse the row size */
    if (token[0] != '[' || sscanf(token+1, "%d", &row) != 1 || token[strlen(token) - 1] != ']')
        addErrorToList(errorList, MAT_INVALID_ROW_E);

    free(token);
    token = cutFirstToken(line, &errorCode); /* should be "[col]" */
    if (errorCode != UTIL_SUCCESS_S) {
        if (errorCode == END_OF_LINE_S)
            errorCode = DIRECTIVE_DATA_MISSING_E;
        addErrorToList(errorList, errorCode);
        return LEXER_FAILURE_S;
    }

    /* check if start and end characters are correct and parse the column size */ 
    if (token[0] != '[' || sscanf(token+1, "%d", &col) != 1 || token[strlen(token) - 1] != ']') 
        addErrorToList(errorList, MAT_INVALID_COL_E);
    
    free(token);
    if (startErrCount < errorList->count) /* if new errors were added */
        return LEXER_FAILURE_S;

    if (row <= 0 || col <= 0) { /* check if row or col are zero or negative */
        addErrorToList(errorList, MAT_SIZE_ZERO_NEG_E);
        return LEXER_FAILURE_S;
    }

    dataCount = row * col; /* calculate the number of data items in the matrix */
    dataItems = calloc(dataCount, sizeof(int)); /* allocate memory for the data items (zero-initialized) */
    if (dataItems == NULL) {
        addErrorToList(errorList, MALLOC_ERROR_F);
        return LEXER_FAILURE_S;
    }

    i = 0;
    token = strtok(line, ",");

    while (token != NULL) {
        double value;

        while (isspace(*token)) /* skip leading whitespace */
            token++;

        if (dataCount <= i) { /* if we have more items than the matrix size */
            addErrorToList(errorList, MAT_SIZE_TOO_LARGE_E); /* if there are more items than matrix size */
            free(dataItems);
            return LEXER_FAILURE_S;    
        }

        endPtr = token; /* set endPtr to the start of the token */
        value = strtod(token, &endPtr);

        while (isspace(*endPtr)) /* skip trailing whitespace */
            endPtr++;

        if (endPtr == token) /* if no number was found */
            addErrorToList(errorList, DATA_INVALID_VALUE_E);
        else if (*endPtr != '\0') /* if there are non-numeric and non-whitespace characters after the number */
            addErrorToList(errorList, MISSING_COMMA_E);
        else if (value != (int)value) /* if the value is not an integer */
            addErrorToList(errorList, DATA_ITEM_NOT_INTEGER_E);
        else if (!isValidInteger((int)value)) /* check if the integer value is valid for the assembler */
            addErrorToList(errorList, INTEGER_OUT_OF_RANGE_E);        

        

        dataItems[i++] = (int)value;
        token = strtok(NULL, ","); /* get next token */
    }

    free(token);
    if (startErrCount < errorList->count) /* if new errors were added */
        return LEXER_FAILURE_S;
    

    pLine->lineContentUnion.directive.dataItems = dataItems; /* set the data items in the parsed line */
    pLine->lineContentUnion.directive.dataCount = dataCount; /* set the data count in the parsed line */

    return LEXER_SUCCESS_S;
}

ErrCode parseEntryDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList)
{
    char *token;
    ErrCode errorCode = NULL_INITIAL;

    if (FALSE){
        (void)errorCode;
        (void)token;
    }

    return LEXER_SUCCESS_S; 
}

ErrCode parseExternDirectiveLine(parsedLine *pLine, char *line, ErrorList *errorList)
{
    char *token;
    ErrCode errorCode = NULL_INITIAL;

    if (FALSE){
        (void)errorCode;
        (void)token;
    }

    return LEXER_SUCCESS_S;
}

ErrCode parseInstructionLine(parsedLine *pLine, char *line, ErrorList *errorList){
    char *token;
    ErrCode errorCode = NULL_INITIAL;

    if (FALSE){
        (void)errorCode;
        (void)token;
    }

    return LEXER_SUCCESS_S;
}

void freeParsedLine(parsedLine *pLine) {
    if (!pLine)
        return;

    free(pLine->label);
    
    switch (pLine->typesOfLine) {
        case DIRECTIVE_LINE:
            if (pLine->lineContentUnion.directive.directiveName != NULL)
                free(pLine->lineContentUnion.directive.directiveName);
            if (pLine->lineContentUnion.directive.dataItems != NULL)
                free(pLine->lineContentUnion.directive.dataItems);
            if (pLine->lineContentUnion.directive.directiveLabel != NULL)
                free(pLine->lineContentUnion.directive.directiveLabel);
            break;
        case INSTRUCTION_LINE:
            if (pLine->lineContentUnion.instruction.operationName != NULL)
                free(pLine->lineContentUnion.instruction.operationName);
            if (pLine->lineContentUnion.instruction.operand1 != NULL)
                free(pLine->lineContentUnion.instruction.operand1);
            if (pLine->lineContentUnion.instruction.operand2 != NULL)
                free(pLine->lineContentUnion.instruction.operand2);
            break;

        default: /* For COMMENT_LINE and EMPTY_LINE, no additional memory to free */
            break;
    }
    free(pLine);
}

void printParsedLine(parsedLine *pLine)
{
    if (pLine == NULL)
        return;

    printf("\nParsed Line:\n");
    if (pLine->label != NULL)
        printf("%s", pLine->label);
    
    switch (pLine->typesOfLine) {
        case INSTRUCTION_LINE:
            printf(" %s ", pLine->lineContentUnion.instruction.operationName);
            if (pLine->lineContentUnion.instruction.operand1 != NULL)
                printf(" %s", pLine->lineContentUnion.instruction.operand1);
            if (pLine->lineContentUnion.instruction.operand2 != NULL)
                printf(" , %s", pLine->lineContentUnion.instruction.operand2);
            break;
        case DIRECTIVE_LINE:
            if (strcmp(pLine->lineContentUnion.directive.directiveName, ".string") == 0)
            {
                printf(" %s \"", pLine->lineContentUnion.directive.directiveName);
                if (pLine->lineContentUnion.directive.dataItems != NULL) {
                    unsigned int i;
                    for (i = 0; i < pLine->lineContentUnion.directive.dataCount; i++) {
                        printf("%c", pLine->lineContentUnion.directive.dataItems[i]);
                    }
                }
                printf("\"");
            } else if (strcmp(pLine->lineContentUnion.directive.directiveName, ".data") == 0 ||
                        strcmp(pLine->lineContentUnion.directive.directiveName, ".mat") == 0 ||
                        strcmp(pLine->lineContentUnion.directive.directiveName, ".string") == 0) 
            {
                printf(" %s", pLine->lineContentUnion.directive.directiveName);
                if (pLine->lineContentUnion.directive.dataCount > 0) {
                    unsigned int i;
                    for (i = 0; i < pLine->lineContentUnion.directive.dataCount; i++) {
                        printf(" %d", pLine->lineContentUnion.directive.dataItems[i]);
                        if (i < pLine->lineContentUnion.directive.dataCount - 1)
                            printf(",");
                    }
                }
            }
            else /* .entry or .extern */
            {
                printf(" %s", pLine->lineContentUnion.directive.directiveName);
                if (pLine->lineContentUnion.directive.directiveLabel != NULL)
                    printf(" %s", pLine->lineContentUnion.directive.directiveLabel);
            }
            break;
        case COMMENT_LINE:
            printf("Comment Line");
            break;
        case EMPTY_LINE:
            printf("Empty Line");
            break;
        case UNSET_LINE:
            printf("Unset Line");
            break;
        default:
            printf("Unknown Line Type");
            break;
    }
    printf("\n\n");
}

/* is X functions: */

/* Check if the string is end of line (contains only whitespace or is empty) */
Bool isEndOfLine(const char *str)
{
    int i = 0;
    while (isspace(str[i])) /* skip leading whitespace */
        i++;
    
    if (str[i] == '\0') /* if the string is empty or contains only whitespace */
        return TRUE; 
    return FALSE; /* if there are non-whitespace characters, it is not end of line */
}

Bool isOperationName(const char* arg) {

    if (strcmp(arg, "mov") == 0 ||
        strcmp(arg, "cmp") == 0 ||
        strcmp(arg, "add") == 0 ||
        strcmp(arg, "sub") == 0 ||
        strcmp(arg, "lea") == 0 ||
        strcmp(arg, "clr") == 0 ||
        strcmp(arg, "not") == 0 ||
        strcmp(arg, "inc") == 0 ||
        strcmp(arg, "dec") == 0 ||
        strcmp(arg, "jmp") == 0 ||
        strcmp(arg, "bne") == 0 ||
        strcmp(arg, "jsr") == 0 ||
        strcmp(arg, "red") == 0 ||
        strcmp(arg, "prn") == 0 ||
        strcmp(arg, "rts") == 0 ||
        strcmp(arg, "stop") == 0)
        return TRUE;
    else 
        return FALSE;
}

Bool isRegister(const char* arg) {
    if (strcmp(arg, "r0") == 0 ||
        strcmp(arg, "r1") == 0 ||
        strcmp(arg, "r2") == 0 ||
        strcmp(arg, "r3") == 0 ||
        strcmp(arg, "r4") == 0 ||
        strcmp(arg, "r5") == 0 ||
        strcmp(arg, "r6") == 0 ||
        strcmp(arg, "r7") == 0)
        return TRUE;
    return FALSE;
}

Bool isDirective(const char* arg) {

    if (strcmp(arg, ".data") == 0 ||
        strcmp(arg, ".string") == 0 ||
        strcmp(arg, ".mat") == 0 ||
        strcmp(arg, ".entry") == 0 ||
        strcmp(arg, ".extern") == 0)
        return TRUE;
        
    return FALSE;
}

Bool isMacroStart(const char* arg) {
    if (strcmp(arg, "mcro") == 0)
        return TRUE;
    return FALSE;
}

Bool isMacroEnd(const char* arg) {
    if (strcmp(arg, "mcroend") == 0)
        return TRUE;
    return FALSE;
}

Bool isKeywords(const char *arg)
{
    if (isOperationName(arg) ||
        isRegister(arg) ||
        isDirective(arg) ||
        isMacroStart(arg) ||
        isMacroEnd(arg))
        return TRUE;
    return FALSE;
}

Bool isLabel(const char *str)
{
    char *labelEnd;
    
    if (str == NULL || str[0] == '\0') /* check if the string is NULL or empty */
        return FALSE;

    labelEnd = strchr(str, ':'); /* check if the string contains ':' only found in labels */
    if (labelEnd == NULL) 
        return FALSE; /* if ':' is not found, it is not a label */
    return TRUE; /* if ':' is found, it is a label */
}

Bool isValidInteger(int value)
{
    if (value >= MIN_10BIT && value <= MAX_10BIT) /* check if the integer value can fit in 10 bits (-512 to 511) */
        return TRUE;
    return FALSE;
}

ErrCode isValidLabel(MacroTable *table, const char *label)
{
    int i, len = strlen(label); /* start from 1 to skip the first character */

    if (len == COLON_LENGTH) /* check if the label is empty (only contains ':') */
        return LABEL_EMPTY_E; /* label is too short */

    if (len > (MAX_LABEL_LENGTH + COLON_LENGTH)) /* check if the label length is valid */
        return LABEL_TOO_LONG_E; /* label is too long */

    if (!isalpha(label[0])) /* check if the first character is a valid letter */
        return LABEL_INVALID_START_CHAR_E; /* label must start with a letter */

    if(isKeywords(label)) /* check if the label is a keyword */
        return LABEL_NAME_IS_KEYWORD_E; /* label cannot be a keyword */

    for (i = 1; i < len - 1; i++) {
        if (!isalnum(label[i])){ /* check if the character is letter or digit */

            if (label[i] == ':') /* if the character is ':' */
                return LABEL_TEXT_AFTER_COLON_E; /* label cannot have text after the colon */

            return LABEL_INVALID_CHAR_E; /* if char isnt alphanumeric or ':' its invalid */
        }
    }

    if(isMacroExists(table, label)) /* check if the label is a macro name */
        return LABEL_SAME_AS_MACRO_E; /* label cannot be a macro name */

    /* note: we do not check the last character cas it is ':' (we only call isValidLabel() if there is
     a colon and if we didn't find any other ':' in the range 1 to len-1 it must be in len) */
    return LEXER_SUCCESS_S; /* valid label */
}

ErrCode isMacroNameValid(MacroTable* table , const char* macroName) {
    int i; /* index for iterating through the macro name */
    int len = strlen(macroName); /* length of the macro name */

    if (len == 0)
        return MACRO_NAME_EMPTY_E; /* exit if the macro name is empty */
    if(len > MAX_MACRO_LENGTH) 
        return MACRO_NAME_TOO_LONG_E; /* exit if the macro name is too long */
    
    if (!isalpha(macroName[0]))
        return MACRO_INVALID_START_CHAR_E; /* exit if the first character is not a letter */

    for (i = 1; i < len; i++) {
        if (!isalnum(macroName[i]) && macroName[i] != '_')
            return MACRO_INVALID_CHAR_E; /* exit if the macro name contains invalid characters */
    }
    
    if (isKeywords(macroName))
        return MACRO_NAME_IS_KEYWORD_E; /* exit if the macro name is a keyword */
    
    if (isMacroExists(table, macroName))
        return MACRO_NAME_EXISTS_E; /* exit if the macro name already exists */
    
    return TABLES_SUCCESS_S; /* Macro name is valid */
}
