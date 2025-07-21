#include "lexer.h"
#include "global.h"
#include "error.h"
#include "util.h"
#include "tables.h"

parsedLine* createParsedLine()
{
    parsedLine* pLine = malloc(sizeof(parsedLine)); /* allocate memory for the parsedLine structure */
    if (pLine == NULL)
        return NULL; /* return NULL if memory allocation failed */
    
    pLine->label = NULL; /* initialize the label to NULL */
    pLine->typesOfLine = UNSET_LINE;
    memset(&pLine->lineContentUnion, 0, sizeof(pLine->lineContentUnion)); /* initialize the union to zero */
    return pLine;
}

/* readParsedLine - reads a line from the file and returns a parsedLine structure
 * errorCode:  EOF_REACHED_S , MALLOC_ERROR_F, UTIL_SUCCESS_S
 */
parsedLine* readParsedLine(FILE *fp, ErrCode *errorCode, MacroTable *macroNames, ErrorList *errorList)
{
    parsedLine* pLine;
    char* line;

    line = readLine(fp, errorCode); /* we will delete the parsed parts from this line */
    if (*errorCode != UTIL_SUCCESS_S)  /* if an error occurred while reading the line */
        return NULL;

    pLine = createParsedLine(); /* create a new parsedLine structure */
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

    /* get the label from the line */
    *errorCode = getLabelFromLine(pLine, line, macroNames, errorList);
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
        *errorCode = parseDirectiveLine(pLine, line, macroNames, errorList);
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
    char *labelEnd; /* use to determine if the first token is a label */
    ErrCode errorCode = NULL_INITIAL;
    
    pLine->label = NULL; /* initialize the label to NULL */
    
    token = getFirstToken(line, &errorCode);
    if (errorCode != UTIL_SUCCESS_S) 
        return errorCode;

    labelEnd = strchr(token, ':'); /* check if the string contains ':' only found in labels */
    if (labelEnd == NULL) { /* if the first token is not a label (starting labels have a colon [at the end]) */
        free(token);
        return LEXER_SUCCESS_S;
    }

    errorCode = isValidLabelColon(macroNames, token); /* check if the label is valid */
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

    if (token[0] == '.'){ /* if the line contains a dot, it is unknown directive */
        free(token);
        return INVALID_DIRECTIVE_E;
    }

    free(token); 
    return UNKNOWN_LINE_TYPE_E; /* the line is not an instruction or a directive */
}

ErrCode parseDirectiveLine(parsedLine *pLine, char *line, MacroTable *macroNames, ErrorList *errorList){

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

    if (strcmp(pLine->lineContentUnion.directive.directiveName, ".entry") == 0 ||
        strcmp(pLine->lineContentUnion.directive.directiveName, ".extern") == 0) 
        return parseEntryExternDirectiveLine(pLine, line, macroNames, errorList);

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
        else if (!isEndOfLine(endPtr)) /* if there are non-numeric and non-whitespace characters after the number */
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
    unsigned int startErrCount = errorList->count; /* save the current error count to check if any errors were added */
    unsigned int i, arrSize = INITIAL_DATA_ITEMS_SIZE;
    char *startQuote, *endQuote;
    int* dataItems = malloc(sizeof(int) * INITIAL_DATA_ITEMS_SIZE);
    if (dataItems == NULL) {
        addErrorToList(errorList, MALLOC_ERROR_F);
        return LEXER_FAILURE_S;
    }

    printf("Parsing string directive: %s\n", line); /* debug print */

    if (isEndOfLine(line)) { /* if the directive has no data */
        addErrorToList(errorList, DIRECTIVE_DATA_MISSING_E);
        return LEXER_FAILURE_S;
    }


    startQuote = strchr(line, '"'); /* find the first quote */
    endQuote = strrchr(line, '"'); /* find the last quote */

    if (startQuote == NULL){ /* if didn't find any quote */
        addErrorToList(errorList, STR_MISSING_OPEN_QUOTE_E);
        addErrorToList(errorList, STR_MISSING_CLOSE_QUOTE_E);
    }
    else if (startQuote != line) /* if the first character is not '"' (line already skipped whitespace) */
        addErrorToList(errorList, STR_MISSING_OPEN_QUOTE_E);
    else if (endQuote == startQuote) /* if didn't find a closing quote */
        addErrorToList(errorList, STR_MISSING_CLOSE_QUOTE_E);

    for (i = 1; i < strlen(line) - 1; i++)
    {
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

    /* if found a closing quote and there is extraneous text after the string */
    if (endQuote != NULL && endQuote != startQuote && !isEndOfLine(endQuote + QUOTE_LENGTH))
        addErrorToList(errorList, EXTRANEOUS_TEXT_E);

    if (startErrCount < errorList->count){ /* if new errors were added we return failure */
        free(dataItems);
        return LEXER_FAILURE_S;
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

ErrCode parseEntryExternDirectiveLine(parsedLine *pLine, char *line, MacroTable *macroNames, ErrorList *errorList)
{
    ErrCode errorCode = NULL_INITIAL;

    char* token = cutFirstToken(line, &errorCode); /* get the label from the line */
    if (errorCode != UTIL_SUCCESS_S) {
        addErrorToList(errorList, errorCode);
        return LEXER_FAILURE_S;
    }

    errorCode = isValidLabelName(macroNames, token);
    if (errorCode != LEXER_SUCCESS_S) {  /* if the label is not valid */
        addErrorToList(errorList, errorCode);
        free(token);
        return LEXER_FAILURE_S;
    }

    if (!isEndOfLine(line)) { /* if there is extraneous text after the label */
        addErrorToList(errorList, EXTRANEOUS_TEXT_E); /* if the directive has no label */
        free(token);
        return LEXER_FAILURE_S;
    }

    pLine->lineContentUnion.directive.directiveLabel = token; /* set the directive label */
    pLine->lineContentUnion.directive.dataCount = 0; /* set the data count to 0 for extern directive */
    pLine->lineContentUnion.directive.dataItems = NULL; /* set the data items to NULL for extern directive */

    if (pLine->label != NULL){ /* if the line has a label */
        free(pLine->label); /* free the label because it is not needed for entry/extern directives */
        pLine->label = NULL;
    }

    return LEXER_SUCCESS_S;
}

ErrCode parseInstructionLine(parsedLine *pLine, char *line, ErrorList *errorList)
{
    char *operand1 = NULL, *operand2 = NULL;
    char* commaExists = NULL; /* used to find the first comma in the line for error checking */
    unsigned int operandLen = 0;
    pLine->lineContentUnion.instruction.operandCount = numOfOperandsInInstruction(pLine->lineContentUnion.instruction.operationName);

    int len = strlen(line);

    if (pLine->lineContentUnion.instruction.operandCount == NO_OPERANDS) { /* if the instruction has no operands */
        if (!isEndOfLine(line)) { /* if there is extraneous text after the instruction */
            addErrorToList(errorList, EXTRANEOUS_TEXT_E);
            return LEXER_FAILURE_S;
        }
    }

    commaExists = strchr(line, ','); /* find the first comma in the line */

    operand1 = strtok(line, ","); /* get the first operand */
    if (operand1 == NULL) { /* if the first operand is missing */
        addErrorToList(errorList, MISSING_FIRST_OPERAND_E);
        return LEXER_FAILURE_S;
    }

    operandLen = strlen(operand1); /* to check */

    operand1 = trimmedDup(operand1); /* making the first operand a dynamic string */
    if (operand1 == NULL) { /* if memory allocation failed */
        addErrorToList(errorList, MALLOC_ERROR_F);
        return LEXER_FAILURE_S;
    }

    pLine->lineContentUnion.instruction.operand1 = operand1; /* set the first operand so we can check for more errors */
    if (pLine->lineContentUnion.instruction.operandCount == ONE_OPERAND) { /* if the instruction has one operand */
        if (commaExists != NULL) { /* if there was a comma after the first operand */
            addErrorToList(errorList, ONE_OPERAND_COMMA_E); /* if the instruction has no second operand */
            return LEXER_FAILURE_S;
        }
        return LEXER_SUCCESS_S; /* return success if the instruction has one operand */
    }

    operand2 = strtok(NULL, ","); /* get the second operand */
    if (operand2 == NULL) { /* if the second operand is missing */
        addErrorToList(errorList, MISSING_SECOND_OPERAND_E);
        return LEXER_FAILURE_S;
    }

    operand2 = trimmedDup(operand2); /* making the second operand a dynamic string */
    if (operand2 == NULL) { /* if memory allocation failed */
        addErrorToList(errorList, MALLOC_ERROR_F);
        return LEXER_FAILURE_S;
    }

    pLine->lineContentUnion.instruction.operand2 = operand2; /* set the second operand */
    if (pLine->lineContentUnion.instruction.operandCount == TWO_OPERANDS)
        return LEXER_SUCCESS_S;


    return LEXER_FAILURE_S; /* should never reach here but if it does, return failure */
}

ErrCode determineOperandType(parsedLine *pLine, MacroTable *macroNames, SymbolTable *symbolTable, ErrorList *errorList)
{
    ErrCode errorCode = NULL_INITIAL;

    /* logic goes here*/
    if (FALSE)
        errorCode = errorCode; 

    return LEXER_FAILURE_S;
}

short int numOfWordsInInstruction(parsedLine *pLine)
{
    return 0; /* Not implemented yet */
}

short int numOfOperandsInInstruction(const char *instructionName)
{
    if (strcmp(instructionName, "mov") == 0 ||
        strcmp(instructionName, "cmp") == 0 ||
        strcmp(instructionName, "add") == 0 ||
        strcmp(instructionName, "sub") == 0 ||
        strcmp(instructionName, "lea") == 0)
        return TWO_OPERANDS; /* mov, cmp, add, sub, lea have 2 operands */
    else if (strcmp(instructionName, "clr") == 0 ||
             strcmp(instructionName, "not") == 0 ||
             strcmp(instructionName, "inc") == 0 ||
             strcmp(instructionName, "dec") == 0 ||
             strcmp(instructionName, "jmp") == 0 ||
             strcmp(instructionName, "bne") == 0 ||
             strcmp(instructionName, "jsr") == 0 ||
             strcmp(instructionName, "red") == 0 ||
             strcmp(instructionName, "prn") == 0)
        return ONE_OPERAND; /* clr, not, inc, dec, jmp, bne, jsr, red, prn have 1 operand */
    else if (strcmp(instructionName, "rts") == 0 ||
             strcmp(instructionName, "stop") == 0)
        return NO_OPERANDS; /* rts and stop have no operands */

    /* if the instruction name is not recognized, return -1 */
    return ERROR_OPERAND_AMOUNT; /* should never happen in parseInstructionLine */
}

void freeParsedLine(parsedLine *pLine)
{
    if (!pLine)
        return;

    if (pLine->label != NULL)
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

ErrCode isRegisterOperand(const char* arg) {
    if (arg[0] == 'r')
        return LEXER_FAILURE_S;
    if (arg[1] < '0' && arg[1] > '7') 
        return invalid;
        
    if(!isEndOfLine(arg + REGISTER_LENGTH)) /* check if the operand is a valid register */
        return EXTRANEOUS_TEXT_E;

    return FALSE;
}

Bool isDirective(const char* arg)
{
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

Bool isValidInteger(int value)
{
    if (value >= MIN_10BIT && value <= MAX_10BIT) /* check if the integer value can fit in 10 bits (-512 to 511) */
        return TRUE;
    return FALSE;
}

/* Check if the operand is a valid label (doesnt check in the symbol table!) */
ErrCode isLabelOperand(const char *operand) {
    if (!isalpha(operand[0])) /* check if the first character is a letter */
        return LEXER_FAILURE_S;

    while (*operand != '\0') { /* iterate through the operand until the end */
        if (!isalnum(*operand)) /* check if the character is alphanumeric */
            return LEXER_FAILURE_S;
        operand++;
    }

    return LEXER_SUCCESS_S;
}

ErrCode isNumberOperand(const char *operand) {
    if (operand[0] != '#') /* check if the operand starts with '#' */
        return LEXER_FAILURE_S;
    operand++; /* move the pointer to the next character */

    if (*operand == '-' || *operand == '+') /* check if the operand starts with a sign */
        operand++; 

    if (!isdigit(*operand)) /* check if the first character after the sign is a digit */
        return MISSING_NUM_OPERAND_E;

    /* iterate through the operand until the end or until a non-digit character is found */
    while (*operand != '\0'){
        if(isalnum(*operand)) /* check if the character is a digit */
            operand++;
        else if (isspace(*operand) && isEndOfLine(operand))
            return LEXER_SUCCESS_S; /* if there are no characters after the number, return success */                
        else
            return EXTRANEOUS_TEXT_E; /* if there are non-digit characters after the number, return error */
    }

    return LEXER_SUCCESS_S;
}


ErrCode isValidLabelColon(MacroTable *table, const char *label)
{
    char* newLabel;
    ErrCode errorCode;

    if (label[strlen(label) - 1] != ':') /* check if the last character is ':' */
        return LABEL_TEXT_AFTER_COLON_E; /* colon is in the middle of the label */
    
    newLabel = delColonFromLabel(label); /* remove the colon from the label for further checks */
    if (newLabel == NULL) /* check if the label is NULL after removing the colon */
        return MALLOC_ERROR_F;
    
    errorCode = isValidLabelName(table, newLabel); /* check if the label name is valid */
    free(newLabel); /* free the allocated memory for the new label */

    if (errorCode != LEXER_SUCCESS_S)  /* if the label name is not valid */
        return errorCode; /* return the error that the new label caused */
    
    return LEXER_SUCCESS_S;
}

ErrCode isValidLabelName(MacroTable *macroNames, const char *label)
{
    int i, len = strlen(label); /* start from 1 to skip the first character */

    if (len == 0) /* check if the label is empty */
        return LABEL_EMPTY_E; /* label is too short */

    if (len > (MAX_LABEL_LENGTH)) /* check if the label length is valid */
        return LABEL_TOO_LONG_E; /* label is too long */

    if (!isalpha(label[0])) /* check if the first character is a valid letter */
        return LABEL_INVALID_START_CHAR_E; /* label must start with a letter */

    for (i = 1; i < len; i++) {
        if (!isalnum(label[i])) /* check if the character is letter or digit */
            return LABEL_INVALID_CHAR_E; /* if char isnt alphanumeric or ':' its invalid */
    }

    if(isKeywords(label)) /* check if the label is a keyword */
        return LABEL_NAME_IS_KEYWORD_E; /* label cannot be a keyword */
    if (isMacroExists(macroNames, label)) /* check if the label already exists in the macro table */
        return LABEL_SAME_AS_MACRO_E; /* label cannot be a macro name */

    return LEXER_SUCCESS_S; 
}

char* delColonFromLabel(const char *label)
{
    int len = strlen(label);
    char *newLabel;

    newLabel = malloc(len - COLON_LENGTH + NULL_TERMINATOR);
    if (newLabel == NULL)
        return NULL;

    strncpy(newLabel, label, len - COLON_LENGTH);
    newLabel[len - COLON_LENGTH] = '\0';

    return newLabel;
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
