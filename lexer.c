#include "lexer.h"
#include "global.h"
#include "error.h"
#include "util.h"
#include "tables.h"

parsedLine* readLineType(FILE *fp, ErrCode *errorCode)
{
    parsedLine *lineRead;
    *errorCode = NULL_INITIAL; /* reset error code to initial state */

    lineRead = malloc(sizeof(parsedLine));
    if (lineRead == NULL) {
        *errorCode = MALLOC_ERROR;
        return NULL;
    }

    lineRead->restOfLine = readLine(fp, errorCode); /* we will delete the parsed parts from this line */
    if (*errorCode != SCAN_SUCCESS) { /* if an error occurred while reading the line */
        free(lineRead); /* free the allocated memory for the parsedLine */
        return NULL; /* return NULL */
    }
    
    /* if an error occurred while determining the line type */
    if ((*errorCode = determineLineType(lineRead)) != SCAN_SUCCESS){
        freeScannedLine(lineRead); /* free the allocated memory for the parsedLine */
        return NULL; 
    }

    return lineRead;
}

ErrCode determineLineType(parsedLine *pline) 
{
    char *token, *line;
    ErrCode errorCode = NULL_INITIAL; /* reset error code to initial state */

    line = pline->restOfLine; /* get the line from the parsedLine structure (easier to read) */
    pline->firstToken = NULL; /* initialize the first token to NULL */
    pline->label = NULL; /* initialize the label to NULL */

    if (isEndOfLine(line)) { /* check if the line is empty or contains only whitespace */
        pline->typesOfLine = EMPTY_LINE; /* set the type of line to EMPTY_LINE */
        return SCAN_SUCCESS; /* return success */
    }

    if (line[0] == ';') { /* if the line is a comment */
        pline->typesOfLine = COMMENT_LINE;
        return SCAN_SUCCESS; /* return success */
    }

    token = cutFirstToken(line, &errorCode); /* get the first token from the line */

    /* note we still need to check for errors of getFirstToken() but we will do that after isLabel() call*/

    if (isLabel(token)) { /* check if the first token is a label */
        pline->label = token;
        token = cutFirstToken(line, &errorCode); /* get the next token after the label */
    }
    else
        pline->label = NULL; /* set the label to NULL */
    
    /* here we check for errors of getFirstToken() (may be the first token may be the second)*/
    if (errorCode != SCAN_SUCCESS) {
        free(token); /* free the allocated memory for the token */
        return errorCode;
    }
    
    pline->firstToken = token; /* set the first token to the operation name */

    if (isOperationName(token))   /* if the line is an instruction */
        pline->typesOfLine = INSTRUCTION_LINE;
    else if (isDirective(token)) 
        pline->typesOfLine = DIRECTIVE_LINE; /* if the line is a directive */
    else if (token[0] == '.') { /* if the line contains a dot, it is unknown directive */
            free(token);
            return INVALID_DIRECTIVE; /* return error code for invalid directive */
        }
    else /* if the line is not an instruction or a directive */
        return UNKNOWN_LINE_TYPE; /* return error code for unknown line type */

    return SCAN_SUCCESS; 
}

void freeScannedLine(parsedLine *pline) {
    if (pline == NULL) 
        return; /* if the parsedLine is NULL, do nothing */

    if (pline->restOfLine != NULL) 
        free(pline->restOfLine);
    if (pline->label != NULL) 
        free(pline->label);
    if (pline->firstToken != NULL)
        free(pline->firstToken);
    free(pline); /* free the parsedLine structure itself */
}

void printParsedLine(parsedLine *pline) {
    if (pline == NULL) {
        printf("Parsed line is NULL\n");
        return; /* if the parsedLine is NULL, do nothing */
    }

    printf("Rest of line: %s\n", pline->restOfLine);
    if (pline->restOfLine != NULL) 
    {
        printf("Rest of line len is %d\n", strlen(pline->restOfLine));
    }
    
    
    printf("Label: %s\n", pline->label);
    printf("First token: %s\n", pline->firstToken);
    printf("Type of line: %d\n", pline->typesOfLine);
}

/* is X functions: */

Bool isEndOfLine(char *str)
{
    int i = 0;
    while (isspace(str[i])) /* skip leading whitespace */
        i++;
    
    if (str[i] == '\0') /* if the string is empty or contains only whitespace */
        return TRUE; 
    return FALSE; /* if there are non-whitespace characters, it is not end of line */
}

Bool isOperationName(char* arg) {

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

Bool isRegister(char* arg) {
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

Bool isDirective(char* arg) {

    if (strcmp(arg, ".data") == 0 ||
        strcmp(arg, ".string") == 0 ||
        strcmp(arg, ".entry") == 0 ||
        strcmp(arg, ".extern") == 0)
        return TRUE;
        
    return FALSE;
}

Bool isMacroStart(char* arg) {
    if (strcmp(arg, "mcro") == 0)
        return TRUE;
    return FALSE;
}

Bool isMacroEnd(char* arg) {
    if (strcmp(arg, "mcroend") == 0)
        return TRUE;
    return FALSE;
}

Bool isKeywords(char *arg)
{
    if (isOperationName(arg) ||
        isRegister(arg) ||
        isDirective(arg) ||
        isMacroStart(arg) ||
        isMacroEnd(arg))
        return TRUE;
    return FALSE;
}

Bool isLabel(char *str)
{
    char *labelEnd;
    
    if (str == NULL || str[0] == '\0') /* check if the string is NULL or empty */
        return FALSE;

    labelEnd = strchr(str, ':'); /* check if the string contains ':' only found in labels */
    if (labelEnd == NULL) 
        return FALSE; /* if ':' is not found, it is not a label */
    return TRUE; /* if ':' is found, it is a label */
}

ErrCode isValidLabel(char *label, macroTable *table)
{
    int i, len = strlen(label); /* start from 1 to skip the first character */

    if (len == COLON_LENGTH) /* check if the label is empty (only contains ':') */
        return LABEL_EMPTY; /* label is too short */

    if (len > (MAX_LABEL_LENGTH + COLON_LENGTH)) /* check if the label length is valid */
        return LABEL_TOO_LONG; /* label is too long */

    if (!isalpha(label[0])) /* check if the first character is a valid letter */
        return LABEL_INVALID_START_CHAR; /* label must start with a letter */

    for (i = 1; i < len - 1; i++) {
        if (!isalnum(label[i])){ /* check if the character is letter or digit */

            if (label[i] == ':') /* if the character is ':' */
                return LABEL_TEXT_AFTER_COLON; /* label cannot have text after the colon */

            return LABEL_INVALID_CHAR; /* if char isnt alphanumeric or ':' its invalid */
        }
    }

    if(isMacroExists(table, label)) /* check if the label is a macro name */
        return LABEL_SAME_AS_MACRO; /* label cannot be a macro name */

    /* note: we do not check the last character cas it is ':' (we only call isValidLabel() if there is
     a colon and if we didn't find any other ':' in the range 1 to len-1 it must be in len) */
    return SCAN_SUCCESS; /* valid label */
}

ErrCode isMacroNameValid(macroTable* table ,char* macroName) {
    int i; /* index for iterating through the macro name */
    int len = strlen(macroName); /* length of the macro name */

    if (len == 0)
        return MACRO_NAME_EMPTY; /* exit if the macro name is empty */
    if(len > MAX_MACRO_LENGTH) 
        return MACRO_NAME_TOO_LONG; /* exit if the macro name is too long */
    
    if (!isalpha(macroName[0]))
        return MACRO_NAME_INVALID_CHAR; /* exit if the first character is not a letter */

    for (i = 1; i < len; i++) {
        if (!isalnum(macroName[i]) && macroName[i] != '_')
            return MACRO_NAME_INVALID_CHAR; /* exit if the macro name contains invalid characters */
    }
    
    if (isKeywords(macroName))
        return MACRO_NAME_KEYWORD; /* exit if the macro name is a keyword */
    
    if (isMacroExists(table, macroName))
        return MACRO_NAME_EXISTS; /* exit if the macro name already exists */
    
    return MACROTABLE_SUCCESS; /* Macro name is valid */
}
