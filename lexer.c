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
 * errorCode:  EOF_REACHED_S, LEXER_SUCCESS_S, LEXER_FAILURE_S
 */
parsedLine* readParsedLine(FILE *fp, ErrCode *errorCode, MacroTable *macroNames, ErrorList *errorList)
{
    parsedLine* pLine;
    char* line;

    line = readLine(fp, errorCode); /* we will delete the parsed parts from this line */
    if (*errorCode == EOF_REACHED_S) 
        return NULL; /* end of file reached, return NULL */
    
    if (*errorCode != UTIL_SUCCESS_S){
        addErrorToList(errorList, *errorCode); /* add the error to the error list */
        return NULL;
    }

    pLine = createParsedLine(); /* create a new parsedLine structure */
    if (pLine == NULL) {
        addErrorToList(errorList, MALLOC_ERROR_F); /* add the error to the error list */
        free(line);
        *errorCode = LEXER_FAILURE_S;
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
    if (*errorCode == LEXER_FAILURE_S) { /* if getting the label from the line failed */
        freeParsedLine(pLine);
        free(line);
        return NULL;
    }

    *errorCode = determineLineType(pLine, line, macroNames ,errorList); /* determine the type of the line */    
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
        if (*errorCode == LEXER_FAILURE_S) { /* if an error occurred while parsing the directive line */
            freeParsedLine(pLine);
            free(line);
            return NULL; /* return NULL if an error occurred */
        }
    } else if (pLine->typesOfLine == INSTRUCTION_LINE) {
        *errorCode = parseInstructionLine(pLine, line, macroNames, errorList); /* parse the instruction line */
        if (*errorCode != LEXER_SUCCESS_S) { /* if an error occurred while parsing the instruction line */
            freeParsedLine(pLine);
            free(line);
            return NULL; /* return NULL if an error occurred */
        }
    }

    *errorCode = LEXER_SUCCESS_S; 
    free(line); 
    return pLine;
}

/* gets the label from the line and sets it in the parsedLine structure
 * errorCode:  LEXER_SUCCESS_S, LEXER_FAILURE_S
 */
ErrCode getLabelFromLine(parsedLine *pLine, char *line, MacroTable *macroNames, ErrorList *errorList)
{
    char *token;
    char *labelEnd; /* use to determine if the first token is a label */
    ErrCode errorCode = NULL_INITIAL;
    
    pLine->label = NULL; /* initialize the label to NULL */
    
    token = getFirstToken(line, &errorCode);
    if (errorCode != UTIL_SUCCESS_S){
        addErrorToList(errorList, errorCode); /* add the error to the error list */
        return LEXER_FAILURE_S;
    }

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

/* determines the type of the line and sets it in the parsedLine structure
 * errorCode:  LEXER_SUCCESS_S, LEXER_FAILURE_S
 */
ErrCode determineLineType(parsedLine *pLine, char *line, MacroTable *macroNames, ErrorList *errorList) 
{
    char *token;
    ErrCode errorCode = NULL_INITIAL; /* reset error code to initial state */

    pLine->typesOfLine = UNSET_LINE; /* initialize the type of line to UNSET_LINE */
    if (isEndOfLine(line)) { /* if the line is empty */
        addErrorToList(errorList, LABEL_EMPTY_LINE_E);
        return LEXER_FAILURE_S;
    }

    token = cutFirstToken(line, &errorCode); /* cut the first token from the line */
    if (errorCode == MALLOC_ERROR_F){
        addErrorToList(errorList, errorCode); /* add the error to the error list */
        return LEXER_FAILURE_S; /* return failure if memory allocation failed */
    }
    
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
        addErrorToList(errorList, INVALID_DIRECTIVE_E);
        return LEXER_FAILURE_S;
    }

    if (isMacroExists(macroNames, token)) { /* if the token is a macro name */
        free(token);
        addErrorToList(errorList, MACRO_AFTER_LABEL_E); /* add the error to the error list */
        return LEXER_FAILURE_S;
    }

    if (isMacroDef(token)) { /* if the token is a macro definition */
        free(token);
        addErrorToList(errorList, MACRO_DEF_AFTER_LABEL_E); /* add the error to the error list */
        return LEXER_FAILURE_S;
    }

    if (isMacroEnd(token)) { /* if the token is a macro end */
        free(token);
        addErrorToList(errorList, MACRO_END_AFTER_LABEL_E); /* add the error to the error list */
        return LEXER_FAILURE_S;
    }


    free(token);
    addErrorToList(errorList, UNKNOWN_LINE_TYPE_E);
    return LEXER_FAILURE_S; /* the line is not an instruction or a directive */
}

/* parses a directive line and sets directive structure
 * errorCode:  LEXER_SUCCESS_S, LEXER_FAILURE_S
 */
ErrCode parseDirectiveLine(parsedLine *pLine, char *line, MacroTable *macroNames, ErrorList *errorList)
{
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

    return INVALID_DIRECTIVE_E; /* cannot reach here because all directives are handled */
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
        else if (!isValidInteger10bits((int)value)) /* check if the integer value is valid for the assembler */
            addErrorToList(errorList, INTEGER_OUT_OF_RANGE10_BITS_E);        

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
        else if (!isValidInteger10bits((int)value)) /* check if the integer value is valid for the assembler */
            addErrorToList(errorList, INTEGER_OUT_OF_RANGE10_BITS_E);        

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

/* parses an instruction line and sets the instruction structure
 * errorCode:  LEXER_SUCCESS_S, LEXER_FAILURE_S
 */
ErrCode parseInstructionLine(parsedLine *pLine, char *line, MacroTable *macroNames, ErrorList *errorList)
{
    ErrCode errorCode = NULL_INITIAL;
    char *matLabel = NULL, *row = NULL, *col = NULL; /* pointers to hold the matrix label, row and column when parsing */
    unsigned int wordCount = FIRST_INSTRUCTION_WORD; /* the amount of binary lines the instruction will take */
    operandType opType1 = UNKNOWN_OPERAND, opType2 = UNKNOWN_OPERAND;
    Bool errorOccurred = FALSE; /* flag to indicate if an error occurred while parsing */

    /* get the number of operands in the instruction */
    pLine->lineContentUnion.instruction.operandCount = numOfOperandsInInstruction(pLine->lineContentUnion.instruction.operationName);

    errorCode = parseInstructionLineOperand(pLine, line, errorList); /* parse the operands of the instruction line */
    if (errorCode != LEXER_SUCCESS_S)
        errorOccurred = TRUE; /* set the error flag to true */
    
    if (errorCode == LEXER_FAILURE_S) /* an error that prevents further parsing */
        return errorCode;
    
    if (pLine->lineContentUnion.instruction.operandCount == NO_OPERANDS) { /* if the instruction has no operands */
        pLine->lineContentUnion.instruction.wordCount = wordCount; /* if it has no operands, the word count is the first instruction word */
        return LEXER_SUCCESS_S; /* return success if the instruction has no operands */
    }

    /* the instruction has at least one operand - lets check the first operand type */
    errorCode = determineOperandType(pLine->lineContentUnion.instruction.operand1, &opType1, &matLabel, &row, &col, macroNames, errorList);
    if (errorCode == LEXER_FAILURE_S){ /* if the operand type is not valid */
        errorOccurred = TRUE; /* set the error flag to true */
        addErrorToList(errorList, OPERAND1_ERROR_N); /* add the error to the error list */
    } 
    else if (opType1 == UNKNOWN_OPERAND) { /* if the operand type is unknown */
        errorOccurred = TRUE; /* set the error flag to true */
        addErrorToList(errorList, OPERAND1_UNKNOWN_E); /* add the error to the error list */
    }
    else if (opType1 == MATRIX_SYNTAX_OPERAND) { /* if the operand type is a matrix */
        wordCount += MATRIX_OPERAND_BIN_LINES; /* if the first operand is a matrix, it will take (2) binary lines */
        free(pLine->lineContentUnion.instruction.operand1); /* free the raw first operand */
        pLine->lineContentUnion.instruction.operand1 = matLabel; /* set the first operand to the label of the matrix */
        pLine->lineContentUnion.instruction.row1 = row; /* set the row of the first operand */
        pLine->lineContentUnion.instruction.col1 = col; /* set the column of the first operand */
    }
    else
        wordCount += NON_MATRIX_OPERAND_BIN_LINES; /* if the first operand is a non-matrix, it will take (1) binary lines */

    pLine->lineContentUnion.instruction.operand1Type = opType1;

    /* if the instruction has only one operand */
    if (pLine->lineContentUnion.instruction.operandCount == ONE_OPERAND || pLine->lineContentUnion.instruction.operand2 == NULL) {
        errorCode = isOperandTypesCompatible(pLine, errorList); /* check if the operand types are compatible */
        if (errorOccurred || errorCode == LEXER_FAILURE_S) /* if an error occurred while parsing operand */
            return LEXER_FAILURE_S;
        pLine->lineContentUnion.instruction.wordCount = wordCount; 
        return LEXER_SUCCESS_S;
    }
    
    /* the instruction has two operands, we need to check the second operand */
    errorCode = determineOperandType(pLine->lineContentUnion.instruction.operand2, &opType2, &matLabel, &row, &col, macroNames, errorList);
    if (errorCode == LEXER_FAILURE_S){ /* if the operand type is not valid */
        errorOccurred = TRUE;
        addErrorToList(errorList, OPERAND2_ERROR_N);
    } 
    else if (opType2 == UNKNOWN_OPERAND) {
        errorOccurred = TRUE;
        addErrorToList(errorList, OPERAND2_UNKNOWN_E);
    }
    else if (opType2 == MATRIX_SYNTAX_OPERAND) { /* if the operand type is a matrix */
        wordCount += MATRIX_OPERAND_BIN_LINES; /* if the first operand is a matrix, it will take (2) binary lines */
        free(pLine->lineContentUnion.instruction.operand2); /* free the raw second operand */
        pLine->lineContentUnion.instruction.operand2 = matLabel; /* set the second operand to the label of the matrix */
        pLine->lineContentUnion.instruction.row2 = row; /* set the row of the second operand */
        pLine->lineContentUnion.instruction.col2 = col; /* set the column of the second operand */
    }
    else if (opType1 == REGISTER_OPERAND && opType2 == REGISTER_OPERAND)  /* if both operands are registers */
        wordCount = BOTH_REGISTER_OPERAND_BIN_LINES + FIRST_INSTRUCTION_WORD; /* if both operands are registers, they will take (1) binary lines */
    else 
        wordCount += NON_MATRIX_OPERAND_BIN_LINES; /* if the first operand is a non-matrix, it will take (1) binary lines */
    
    pLine->lineContentUnion.instruction.operand2Type = opType2; /* set the type of the second operand */
    pLine->lineContentUnion.instruction.wordCount = wordCount; /* set the word count of the instruction */
    
    errorCode = areOperandsTypesCompatible(pLine , errorList); /* check if the operand types are compatible */    
    if (errorOccurred || errorCode == LEXER_FAILURE_S) /* if an error occurred while parsing the one of the operands */
        return LEXER_FAILURE_S;
    
    
    return LEXER_SUCCESS_S;
}

/* Parses an instruction line and sets the the raw unprocessed operands in the parsedLine structure
 * return:  LEXER_SUCCESS_S, LEXER_FAILURE_S, ONE_OPERAND_COMMA_E , MISSING_SECOND_OPERAND_E, THIRD_OPERAND_DETECTED_E
 * return LEXER_FAILURE_S if we cant continue parsing the instruction line
 */
ErrCode parseInstructionLineOperand(parsedLine *pLine, char *line, ErrorList *errorList)
{
    char *operand1 = NULL, *operand2 = NULL;
    char* commaExists = NULL; /* used to find the first comma in the line for error checking */

    if (pLine->lineContentUnion.instruction.operandCount == NO_OPERANDS) { /* if the instruction has no operands */
        if (!isEndOfLine(line)) { /* if there is extraneous text after the instruction */
            addErrorToList(errorList, EXTRANEOUS_TEXT_E);
            return LEXER_FAILURE_S;
        }
        return LEXER_SUCCESS_S;
    }

    commaExists = strchr(line, ','); /* find the first comma in the line */

    operand1 = strtok(line, ","); /* get the first operand */
    if (operand1 == NULL) { /* if the first operand is missing */
        addErrorToList(errorList, MISSING_FIRST_OPERAND_E);
        return LEXER_FAILURE_S;
    }

    operand1 = trimmedDup(operand1); /* making the first operand a dynamic string */
    if (operand1 == NULL) { /* if memory allocation failed */
        addErrorToList(errorList, MALLOC_ERROR_F);
        return LEXER_FAILURE_S;
    }

    pLine->lineContentUnion.instruction.operand1 = operand1; /* set the first operand so we can check for more errors */
    if (pLine->lineContentUnion.instruction.operandCount == ONE_OPERAND) { /* if the instruction has one operand */
        if (commaExists != NULL) { /* if there was a comma after the first operand */
            addErrorToList(errorList, ONE_OPERAND_COMMA_E); /* if the instruction has no second operand */
            return ONE_OPERAND_COMMA_E;
        }
        return LEXER_SUCCESS_S;
    }

    operand2 = strtok(NULL, ","); /* get the second operand */
    if (operand2 == NULL || isEndOfLine(operand2)) { /* if the second operand is missing */
        addErrorToList(errorList, MISSING_SECOND_OPERAND_E);
        return MISSING_SECOND_OPERAND_E;
    }

    operand2 = trimmedDup(operand2); /* making the second operand a dynamic string */
    if (operand2 == NULL) { /* if memory allocation failed */
        addErrorToList(errorList, MALLOC_ERROR_F);
        return LEXER_FAILURE_S;
    }

    pLine->lineContentUnion.instruction.operand2 = operand2; /* set the second operand */

    if (strtok(NULL, ",") == NULL) /* if there are no more commas after the second operand */
        return LEXER_SUCCESS_S;
    
    addErrorToList(errorList, THIRD_OPERAND_DETECTED_E); /* if there are more than two operands */
    return THIRD_OPERAND_DETECTED_E;
}

/* Determines the type of the operand and sets it in the parsedLine structure
 * unknown lines are assumed to be labels
 * errorCode:  LEXER_SUCCESS_S, LEXER_FAILURE_S
 */
ErrCode determineOperandType(const char *operand, operandType *opType, char **matLabel, char **row, char **col, MacroTable *macroNames, ErrorList *errorList)
{
    ErrCode errorCode = NULL_INITIAL;
    *opType = UNKNOWN_OPERAND;

    if (isKeywords(operand)) { /* if the operand is a keyword */
        addErrorToList(errorList, OPERAND_IS_KEYWORD_E); /* add an error to the error list */
        return LEXER_FAILURE_S; /* return failure if the operand is a keyword */
    }

    if (isMacroExists(macroNames, operand)) { /* if the operand is a macro */
        addErrorToList(errorList, OPERAND_IS_MACRO_E); /* add an error to the error list */
        return LEXER_FAILURE_S; /* return failure if the operand is a macro */
    }

    errorCode = isRegisterOperand(operand); /* check if the first operand is a register */
    if (errorCode == LEXER_SUCCESS_S) { /* if the operand is a register */
        *opType = REGISTER_OPERAND; /* set the operand type to REGISTER_OPERAND */
        return LEXER_SUCCESS_S; /* return success */
    }
    else if (errorCode == EXTRANEOUS_TEXT_E) { /* if op is an register with an error */
        addErrorToList(errorList, errorCode); /* add the error to the error list */
        return LEXER_FAILURE_S; /* return failure if the operand is not a valid register */
    }
    /* all other errors does not guarantee user tried to use a register */

    /* so the operand is not a register, lets check if it is a number */

    errorCode = isNumberOperand(operand);
    if (errorCode == LEXER_SUCCESS_S) { /* if the operand is a number */
        *opType = NUMBER_OPERAND; /* set the operand type to NUMBER_OPERAND */
        return LEXER_SUCCESS_S; /* return success */
    }
    else if (errorCode != LEXER_FAILURE_S) { /* if op is a number with an error */
        addErrorToList(errorList, errorCode); /* add the error to the error list */
        return LEXER_FAILURE_S;
    }

    /* so the operand is not a register or a number, lets check if it is a matrix */

    errorCode = parseMatrixOperand(operand, matLabel, row, col);
    if (errorCode == LEXER_SUCCESS_S) { /* if the operand is a matrix */
        Bool errorOccurred = FALSE; /* flag to check if there are errors with the row or column */

        errorCode = isRegisterOperand(*row);
        if (errorCode != LEXER_SUCCESS_S) { /*  if the row is not a valid register */
            addErrorToList(errorList, errorCode); /* add the error to the error list */
            addErrorToList(errorList, MAT_ROW_NOT_REGISTER_N);
            errorOccurred = TRUE;
        }

        errorCode = isRegisterOperand(*col); 
        if (errorCode != LEXER_SUCCESS_S){ /*  if the column is not a valid register */
            addErrorToList(errorList, errorCode); /* add the error to the error list */
            addErrorToList(errorList, MAT_COL_NOT_REGISTER_N);
            errorOccurred = TRUE;
        }

        if (errorOccurred) /* if one of the row or column is not a valid register */
            return LEXER_FAILURE_S; /* return failure if the operand is not a valid matrix */

        /* if both row and column are valid registers, set the operand type to matrix */
        *opType = MATRIX_SYNTAX_OPERAND; /* set the operand type to MATRIX_SYNTAX_OPERAND */
        return LEXER_SUCCESS_S;
    }
    else if (errorCode != LEXER_FAILURE_S) { /* if op is a matrix with an error */
        addErrorToList(errorList, errorCode); /* add the error to the error list */
        return LEXER_FAILURE_S; /* return failure if the operand is not a valid matrix */
    }

    /* so the operand is not a register, number or matrix, so for now we treat it as a label */
    errorCode = isValidLabelSyntax(operand); 
    if (errorCode != LEXER_SUCCESS_S) /* if the label syntax is not valid */
        *opType = UNKNOWN_OPERAND; /* set the operand type to UNKNOWN_OPERAND */
    else
        *opType = LABEL_SYNTAX_OPERAND; /* set the operand type to label valid (for now) */
    return LEXER_SUCCESS_S;
}

/* checks one operand instructions to see if the operand type is compatible for the instruction
 * like inc #1 is invalid
 * return: LEXER_SUCCESS_S if the operand types are compatible, LEXER_FAILURE_S otherwise
 */
ErrCode isOperandTypesCompatible(parsedLine *pLine, ErrorList *errorList)
{
    char *operationName = pLine->lineContentUnion.instruction.operationName; /* instruction name */
    operandType op1 = pLine->lineContentUnion.instruction.operand1Type; /* first operand type */
    Bool errorOccurred = FALSE; /* flag to check if an error occurred while checking the operand types */

    if (op1 == NUMBER_OPERAND && strcmp(operationName, "prn") != 0) { /* if the instruction is not prn and the destination operand is a number */
        errorOccurred = TRUE;
        addErrorToList(errorList, INSTRUCTION_DST_OP_CANT_NUM_E); /* destination operand cannot be a number */
    }

    if(errorOccurred)
        return LEXER_FAILURE_S;
    return LEXER_SUCCESS_S; /* return success if the operand types are compatible */
}

/* checks two operands instructions to see if the operands type is compatible for the instruction
 * like mov #1 , r1 is invalid
 * return: LEXER_SUCCESS_S if the operand types are compatible, LEXER_FAILURE_S not
 */
ErrCode areOperandsTypesCompatible(parsedLine *pLine, ErrorList *errorList)
{
    char *operationName = pLine->lineContentUnion.instruction.operationName; /* instruction name */
    operandType op1 = pLine->lineContentUnion.instruction.operand1Type; /* first operand type */
    operandType op2 = pLine->lineContentUnion.instruction.operand2Type;
    Bool errorOccurred = FALSE; /* flag to check if an error occurred while checking the operand types */

    if (strcmp(operationName, "lea") == 0 && (op1 == NUMBER_OPERAND || op1 == REGISTER_OPERAND) ) { /* if the instruction is lea and the src operand is a number */
        ErrCode errorCode = op1 == NUMBER_OPERAND ? INSTRUCTION_SRC_OP_CANT_NUM_E : INSTRUCTION_SRC_OP_CANT_REGISTER_E; /* set the error code for the src operand */
        errorOccurred = TRUE;
        addErrorToList(errorList, errorCode); /* src operand cannot be a number or a register */
    }

    /* all other instructions allow destination operand to be whatever */
    /* so we move on to the src operand */

    if (op2 == NUMBER_OPERAND && strcmp(operationName, "cmp") != 0) { /* if the instruction is not cmp and the destination operand is a number */
        errorOccurred = TRUE;
        addErrorToList(errorList, INSTRUCTION_DST_OP_CANT_NUM_E); /* destination operand cannot be a number */
    }

    if(errorOccurred)
        return LEXER_FAILURE_S;
    return LEXER_SUCCESS_S;
}

ErrCode parseLabelOperandsValid(parsedLine *pLine, SymbolTable *symbolTable, ErrorList *errorList)
{
    Bool errorOccurred = FALSE; /* flag to check if an error occurred */
    operandType op1 = pLine->lineContentUnion.instruction.operand1Type; /* first operand */
    operandType op2 = pLine->lineContentUnion.instruction.operand2Type; /* second operand */
    if (pLine == NULL || symbolTable == NULL)
        return LEXER_FAILURE_S;

    /* if the first operand is a label or matrix element, we need to check if it exists in the symbol table */
    if (op1 == LABEL_SYNTAX_OPERAND || op1 == MATRIX_SYNTAX_OPERAND) {
        SymbolNode *refSymbol1 = findSymbol(symbolTable, pLine->lineContentUnion.instruction.operand1); /* find the symbol in the symbol table */
        if (refSymbol1 == NULL) { /* if the label does not exist in the symbol table */
            addErrorToList(errorList, OPERAND1_LABEL_DOES_NOT_EXIST_E); /* add an error to the error list */
            errorOccurred = TRUE; /* set the error flag to true */
        }
        else if (op1 == LABEL_SYNTAX_OPERAND) /* if the first operand is a label */
            pLine->lineContentUnion.instruction.operand1Type = LABEL_TABLE_OPERAND; /* change the operand type to LABEL_TABLE_OPERAND */   
        else if (!refSymbol1->isMat) { /* if the first operand is not a matrix element */
            addErrorToList(errorList, OPERAND1_NON_MAT_SYMBOL_E); /* add an error to the error list */
            errorOccurred = TRUE; /* set the error flag to true */
        }
        else /* if the first operand is a matrix element */
            pLine->lineContentUnion.instruction.operand1Type = MATRIX_TABLE_OPERAND;
    }

    /* if the second operand is a label or matrix element, we need to check if it exists in the symbol table */
    if (op2 == LABEL_SYNTAX_OPERAND || op2 == MATRIX_SYNTAX_OPERAND) {
        SymbolNode *refSymbol2 = findSymbol(symbolTable, pLine->lineContentUnion.instruction.operand2); /* find the symbol in the symbol table */
        if (refSymbol2 == NULL) { /* if the label does not exist in the symbol table */
            addErrorToList(errorList, OPERAND2_LABEL_DOES_NOT_EXIST_E); /* add an error to the error list */
            errorOccurred = TRUE; /* set the error flag to true */
        }
        else if (op2 == LABEL_SYNTAX_OPERAND) /* if the first operand is a label */
            pLine->lineContentUnion.instruction.operand2Type = LABEL_TABLE_OPERAND; /* change the operand type to LABEL_TABLE_OPERAND */   
        else if (!refSymbol2->isMat) { /* if the first operand is not a matrix element */
            addErrorToList(errorList, OPERAND2_NON_MAT_SYMBOL_E); /* add an error to the error list */
            errorOccurred = TRUE; /* set the error flag to true */
        }
        else /* if the first operand is a matrix element */
            pLine->lineContentUnion.instruction.operand2Type = MATRIX_TABLE_OPERAND;
    }

    if (errorOccurred) /* if an error occurred while parsing the operands */
        return LEXER_FAILURE_S;

    return LEXER_SUCCESS_S;
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
            if (pLine->lineContentUnion.instruction.row1 != NULL)
                free(pLine->lineContentUnion.instruction.row1);
            if (pLine->lineContentUnion.instruction.col1 != NULL)
                free(pLine->lineContentUnion.instruction.col1);
            if (pLine->lineContentUnion.instruction.operand2 != NULL)
                free(pLine->lineContentUnion.instruction.operand2);
            if (pLine->lineContentUnion.instruction.row2 != NULL)
                free(pLine->lineContentUnion.instruction.row2);
            if (pLine->lineContentUnion.instruction.col2 != NULL)
                free(pLine->lineContentUnion.instruction.col2);
            break;

        default: /* For COMMENT_LINE and EMPTY_LINE, no additional memory to free */
            break;
    }
    free(pLine);
}

void printParsedLine(parsedLine *pLine)
{
    if (pLine == NULL){
        printf("Error: Parsed line is NULL\n");
        return;
    }

    printf("\nParsed Line:\n");

    switch (pLine->typesOfLine) {
        case INSTRUCTION_LINE:
            printf("Instruction Line:\n");
            printf("operand number: %d  , ", pLine->lineContentUnion.instruction.operandCount);
            printf("word count: %d\n", pLine->lineContentUnion.instruction.wordCount);
            printf("operand1 is: >%s<, operand2 is: >%s<\n", printOpType(pLine->lineContentUnion.instruction.operand1Type), printOpType(pLine->lineContentUnion.instruction.operand2Type));

            if (pLine->label != NULL)
                printf("%s", pLine->label);
            
            printf(" %s ", pLine->lineContentUnion.instruction.operationName);
            if (pLine->lineContentUnion.instruction.operand1 != NULL)
                printf(">>%s<<", pLine->lineContentUnion.instruction.operand1);
            if (pLine->lineContentUnion.instruction.row1 != NULL)
                printf(" [>>%s<<]", pLine->lineContentUnion.instruction.row1);
            if (pLine->lineContentUnion.instruction.col1 != NULL)
                printf("[>>%s<<]", pLine->lineContentUnion.instruction.col1);
            if (pLine->lineContentUnion.instruction.operand2 != NULL)
                printf(", >>%s<<", pLine->lineContentUnion.instruction.operand2);
            if (pLine->lineContentUnion.instruction.row2 != NULL)
                printf(" [>>%s<<]", pLine->lineContentUnion.instruction.row2);
            if (pLine->lineContentUnion.instruction.col2 != NULL)
                printf("[>>%s<<]", pLine->lineContentUnion.instruction.col2);
            break;
        case DIRECTIVE_LINE:
            printf("Directive Line:\n");

            if (pLine->label != NULL)
            printf("%s", pLine->label);

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

char* printOpType(operandType opType)
{
    switch (opType) {
        case REGISTER_OPERAND:
            return "Register";
        case NUMBER_OPERAND:
            return "Number";
        case MATRIX_SYNTAX_OPERAND:
            return "Matrix Syntax";
        case MATRIX_TABLE_OPERAND:
            return "Matrix Table";
        case LABEL_SYNTAX_OPERAND:
            return "Label Syntax";
        case LABEL_TABLE_OPERAND:
            return "Label Table";
        default:
            return "Unknown Operand Type";
    }
}

/* is X functions: */

/* check if the string is end of line (contains only whitespace or is empty) */
Bool isEndOfLine(const char *str){
    int i = 0;
    while (isspace(str[i])) /* skip leading whitespace */
        i++;

    return (str[i] == '\0'); /* return TRUE if the string is empty or contains only whitespace */
}

Bool isOperationName(const char* arg){
    return (strcmp(arg, "mov") == 0 ||
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
        strcmp(arg, "stop") == 0);
}

Bool isRegister(const char* arg){
    return (strcmp(arg, "r0") == 0 ||
        strcmp(arg, "r1") == 0 ||
        strcmp(arg, "r2") == 0 ||
        strcmp(arg, "r3") == 0 ||
        strcmp(arg, "r4") == 0 ||
        strcmp(arg, "r5") == 0 ||
        strcmp(arg, "r6") == 0 ||
        strcmp(arg, "r7") == 0);
}

Bool isDirective(const char* arg){
    return (strcmp(arg, ".data") == 0 ||
        strcmp(arg, ".string") == 0 ||
        strcmp(arg, ".mat") == 0 ||
        strcmp(arg, ".entry") == 0 ||
        strcmp(arg, ".extern") == 0);
}

Bool isMacroDef(const char* arg){
    return (strcmp(arg, "mcro") == 0);
}

Bool isMacroEnd(const char* arg){
    return (strcmp(arg, "mcroend") == 0);
}

Bool isKeywords(const char *arg){
    return (isOperationName(arg) ||
        isDirective(arg) ||
        isMacroDef(arg) ||
        isMacroEnd(arg));
}

/* check if the integer value can fit in 10 bits (-512 to 511) */
Bool isValidInteger10bits(int value){
    return (value >= MIN_10BIT_INT && value <= MAX_10BIT_INT); 
}

Bool isValidInteger8bits(int value){
    return (value >= MIN_8BIT_INT && value <= MAX_8BIT_INT);
}

/* Check if the label is a valid label Syntax
 * returns: LEXER_SUCCESS_S, LEXER_FAILURE_S, LABEL_TEXT_AFTER_COLON_E, MALLOC_ERROR_F
 */
ErrCode isValidLabelSyntax(const char *operand)
{
    if (strlen(operand) > MAX_LABEL_LENGTH) /* check if the operand is empty */
        return LABEL_TOO_LONG_E;
    
    if (!isalpha(operand[0])) /* check if the first character is a letter */
        return LABEL_INVALID_START_CHAR_E;

    while (*operand != '\0') { /* iterate through the operand until the end */
        if (!isalnum(*operand)) /* check if the character is alphanumeric */
            return LABEL_INVALID_CHAR_E;
        operand++;
    }

    return LEXER_SUCCESS_S;
}

/* Check if the operand is a valid register
 * returns: LEXER_SUCCESS_S, EXTRANEOUS_TEXT_E, LEXER_FAILURE_S
 */
ErrCode isRegisterOperand(const char* operand)
{
    if (operand[0] != 'r') /* check if the first character is not 'r' */
        return REGISTER_NO_R_E;

     /* if the second character isn't a digit */
    if (!isdigit(operand[1])) 
        return REGISTER_NO_DIGIT_E;

    /* check if the register number is bigger then 7 */
    if (isdigit(operand[2]) || operand[1] == '8' || operand[1] == '9')
        return REGISTER_OUT_OF_RANGE_E;

    if(!isEndOfLine(operand + REGISTER_LENGTH)) /* check if the operand is a valid register */
        return EXTRANEOUS_TEXT_E;

    return LEXER_SUCCESS_S;
}

/* Check if the operand is a number
 * returns: LEXER_SUCCESS_S, MISSING_NUM_OPERAND_E, NUMBER_OPERAND_IS_NOT_INTEGER_E, EXTRANEOUS_TEXT_E, INTEGER_OPERAND_OUT_OF_RANGE8_BITS_E, LEXER_FAILURE_S
 */
ErrCode isNumberOperand(const char *operand)
{
    double value;
    char *endPtr;

    if (operand[0] != '#') 
        return LEXER_FAILURE_S;
    operand++;  /* skip '#' */

    if (isEndOfLine(operand))
        return MISSING_NUM_OPERAND_E;

    value = strtod(operand, &endPtr);
    if (operand == endPtr)  /* no digits were found */
        return MISSING_NUM_OPERAND_E;

    if (value != (int)value)  /* check if the value is an integer */
        return NUMBER_OPERAND_IS_NOT_INTEGER_E;

    if (!isEndOfLine(endPtr))  /* extraneous characters */
        return EXTRANEOUS_TEXT_E;

    if (!isValidInteger8bits((int)value))  /* check if the value is within 8-bit range */
        return INTEGER_OPERAND_OUT_OF_RANGE8_BITS_E;

    return LEXER_SUCCESS_S;
}

ErrCode parseMatrixOperand(const char *operandStr, char **name, char **row, char **col)
{
    const char *p = operandStr;
    const char *start;
    unsigned int len;

    /* clear the former address of the pointers */
    *name = NULL;
    *row = NULL;
    *col = NULL;

    while (isspace(*p)) p++;

    if (!isalpha(*p) && *p != '_')
        return LEXER_FAILURE_S;

    start = p;
    while (isalnum(*p) || *p == '_') p++;
    len = p - start;
    *name = strnDup(start, len);

    while (isspace(*p)) p++;
    if (*p != '[') {
        freeStrings(*name, *row, *col);
        return LEXER_FAILURE_S;
    }
    p++; /* skip the '[' */

    while (isspace(*p)) p++;
    if (*p == '\0' || *p == ']') {
        freeStrings(*name, *row, *col);
        return MAT_EMPTY_ROW_INDEX_E;
    }

    start = p;
    while (*p != '\0' && *p != ']') p++;
    if (*p != ']') {
        freeStrings(*name, *row, *col);
        return MAT_MISSING_FIRST_CLOSING_BRACKET_E;
    }
    len = p - start;
    *row = strnDup(start, len);
    p++; /* skip the ']' */

    while (isspace(*p)) p++;
    if (*p != '[') {
        freeStrings(*name, *row, *col);
        return MAT_MISSING_SECOND_BRACKET_E;
    }
    p++;

    while (isspace(*p)) p++;
    if (*p == '\0' || *p == ']') {
        freeStrings(*name, *row, *col);
        return MAT_EMPTY_COLUMN_INDEX_E;
    }

    start = p;
    while (*p != '\0' && *p != ']') p++;
    if (*p != ']') {
        freeStrings(*name, *row, *col);
        return MAT_MISSING_SECOND_CLOSING_BRACKET_E;
    }
    len = p - start;
    *col = strnDup(start, len);
    p++;

    while (isspace(*p)) p++;
    if (*p != '\0') {
        freeStrings(*name, *row, *col);
        return EXTRANEOUS_TEXT_E;
    }

    return LEXER_SUCCESS_S;
}

/* checks if the label is valid and ends with a colon
 * Returns: isValidLabelName(), LABEL_TEXT_AFTER_COLON_E, MALLOC_ERROR_F, LEXER_SUCCESS_S
 */
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

/* isValidLabelName - checks if the label name is valid (doesn't check in symbol table)
 * Returns: LEXER_SUCCESS_S, LABEL_EMPTY_E, LABEL_TOO_LONG_E, LABEL_INVALID_START_CHAR_E, LABEL_INVALID_CHAR_E, LABEL_NAME_IS_KEYWORD_E, LABEL_SAME_AS_MACRO_E
 */
ErrCode isValidLabelName(MacroTable *macroNames, const char *label)
{
    ErrCode errorCode = NULL_INITIAL; /* initialize the error code */

    if (label[0] == '\0') /* check if the label is empty */
        return LABEL_EMPTY_E; /* label is too short */

    errorCode = isValidLabelSyntax(label); /* check if the label syntax is valid */
    if (errorCode != LEXER_SUCCESS_S) /* if the label syntax is not valid */
        return errorCode; /* return the error code */

    if (isKeywords(label)) /* check if the label is a keyword */
        return LABEL_NAME_IS_KEYWORD_E; /* label cannot be a keyword */

    if (isRegister(label)) /* check if the label is a register */
        return LABEL_NAME_IS_REGISTER_E; /* label is a register */

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

ErrCode isMacroNameValid(MacroTable* table , const char* macroName)
{
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

    if (isRegister(macroName))
        return MACRO_NAME_IS_REGISTER_E; /* exit if the macro name is a register */
    
    if (isMacroExists(table, macroName))
        return MACRO_NAME_EXISTS_E; /* exit if the macro name already exists */
    
    return TABLES_SUCCESS_S; /* Macro name is valid */
}

registersNumber getRegisterNumber(const char *registerName)
{
    int regNum;
    if (registerName[0] != 'r')
        return NOT_REG;

    regNum = atoi(&(registerName[1])); /* look at the number after 'r' */
    if (regNum < 0 || regNum > 7)
        return NOT_REG;

    return (registersNumber)regNum;
}

OpCodeNumber getOpCodeNumber(const char *opName)
{
    if (opName == NULL)
        return invalid;

    if (strcmp(opName, "mov") == 0)
        return mov;
    else if (strcmp(opName, "cmp") == 0)
        return cmp;
    else if (strcmp(opName, "add") == 0)
        return add;
    else if (strcmp(opName, "sub") == 0)
        return sub;
    else if (strcmp(opName, "lea") == 0)
        return lea;
    else if (strcmp(opName, "clr") == 0)
        return clr;
    else if (strcmp(opName, "not") == 0)
        return not;
    else if (strcmp(opName, "inc") == 0)
        return inc;
    else if (strcmp(opName, "dec") == 0)
        return dec;
    else if (strcmp(opName, "jmp") == 0)
        return jmp;
    else if (strcmp(opName, "bne") == 0)
        return bne;
    else if (strcmp(opName, "jsr") == 0)
        return jsr;
    else if (strcmp(opName, "red") == 0)
        return red;
    else if (strcmp(opName, "prn") == 0)
        return prn;
    else if (strcmp(opName, "rts") == 0)
        return rts;
    else if (strcmp(opName, "stop") == 0)
        return stop;

    return invalid;
}
