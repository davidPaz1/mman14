#include "lexer.h"
#include "global.h"
#include "error.h"
#include "util.h"
#include "tables.h"

/* readParsedLine - reads a line from the file and returns a parsedLine structure
 * errorCode:  END_OF_LINE_S , MALLOC_ERROR_F, UTIL_SUCCESS_S
 */
parsedLine* readParsedLine(FILE *fp, ErrCode *errorCode, macroTable *table, ErrorList *errorList)
{
    parsedLine* pLine;
    char* line;

    line = readLine(fp, errorCode); /* we will delete the parsed parts from this line */
    if (*errorCode == EOF_REACHED_S)
        return NULL; /* end of file reached, return NULL */

    if (*errorCode != UTIL_SUCCESS_S)  /* if an error occurred while reading the line */
        return NULL;
    

    pLine = malloc(sizeof(parsedLine));
    if (pLine == NULL) {
        *errorCode = MALLOC_ERROR_F;
        free(line); /* free the line memory */
        return NULL;
    }

    pLine->lineContentUnion = (union dataOrInstruction){0}; /* initialize the union to zero */

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

    if (pLine->typesOfLine == COMMENT_LINE || pLine->typesOfLine == EMPTY_LINE)
        return pLine; /* if the line is a comment or an empty line, return it */

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

ErrCode getLabelFromLine(parsedLine *pLine, char *line, macroTable *macroNames, ErrorList *errorList)
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
    if( errorCode != LEXER_SUCCESS_S) {
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

    if (isEndOfLine(line)) { /* if the line is empty */
        pLine->typesOfLine = EMPTY_LINE;
        return LEXER_SUCCESS_S; 
    }
    
    if (line[0] == ';') {
        pLine->typesOfLine = COMMENT_LINE; /* if the line starts with ';', it is a comment */
        return LEXER_SUCCESS_S; /* return success */
    }

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
    char *token;
    ErrCode errorCode = NULL_INITIAL;

    return LEXER_SUCCESS_S;
}

ErrCode parseInstructionLine(parsedLine *pLine, char *line, ErrorList *errorList){
    char *token;
    ErrCode errorCode = NULL_INITIAL;
    
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


/* is X functions: */

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

ErrCode isValidLabel(macroTable *table, const char *label)
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

ErrCode isMacroNameValid(macroTable* table , const char* macroName) {
    int i; /* index for iterating through the macro name */
    int len = strlen(macroName); /* length of the macro name */

    if (len == 0)
        return MACRO_NAME_EMPTY_E; /* exit if the macro name is empty */
    if(len > MAX_MACRO_LENGTH) 
        return MACRO_NAME_TOO_LONG_E; /* exit if the macro name is too long */
    
    if (!isalpha(macroName[0]))
        return MACRO_INVALID_CHAR_E; /* exit if the first character is not a letter */

    for (i = 1; i < len; i++) {
        if (!isalnum(macroName[i]) && macroName[i] != '_')
            return MACRO_INVALID_CHAR_E; /* exit if the macro name contains invalid characters */
    }
    
    if (isKeywords(macroName))
        return MACRO_NAME_IS_KEYWORD_E; /* exit if the macro name is a keyword */
    
    if (isMacroExists(table, macroName))
        return MACRO_NAME_EXISTS_E; /* exit if the macro name already exists */
    
    return MACROTABLE_SUCCESS_S; /* Macro name is valid */
}
