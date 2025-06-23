#include "global.h"
#include "error.h"
#include "scan.h"
#include "util.h"


char* readLine(FILE *fp, ErrCode *errorCode) {

    /* overlength should store \n or \r if line is under MAX_LINE_FILE_LENGTH */
    char *line = malloc(MAX_LINE_FILE_LENGTH + OVER_LENGTH  + NULL_TERMINATOR);
    char next; 
    *errorCode = NULL_INITIAL; /* reset error code to initial state */
    if (line == NULL) {
        *errorCode = MALLOC_ERROR;
        return NULL; /* malloc failed */
    }

    /* overlength should store \n or \r if line is under MAX_LINE_FILE_LENGTH */
    if (fgets(line, MAX_LINE_FILE_LENGTH + OVER_LENGTH  + NULL_TERMINATOR, fp) == NULL) {
        free(line);
        if (feof(fp))
            *errorCode = EOF_REACHED;
        else 
            *errorCode = FILE_READ_ERROR;
        return NULL; /* end of file or error */
    }

    next = fgetc(fp); /* peek on the next character */
    if (next != EOF)  /* if the next character is not EOF, it means the line is longer than MAX_LINE_FILE_LENGTH */
        ungetc(next, fp); /* put back the next character to the input stream */
    
    if (strchr(line, '\n') == NULL && strchr(line, '\r') == NULL && next != EOF) {
        char c; /* discard the rest of the line from the input stream */
        while ((c = fgetc(fp)) != '\n' && c != EOF);
        *errorCode = LINE_TOO_LONG;
        free(line);
        return NULL; /* line is too long, return NULL */
    }
    else {
        line[strcspn(line, "\r\n")] = '\0'; /* remove the newline or carriage return character from the end of the line */
        if ( strlen(line) == MAX_LINE_FILE_LENGTH)  /* if the line is exactly MAX_LINE_FILE_LENGTH characters long, it has a remaining '\n' */
            (void) fgetc(fp);  /* discard the unused character '\n' from the input stream */
        *errorCode = SCAN_SUCCESS; /* set error code to success */
    }
    return line;
}

char* getFirstToken(char *str, ErrCode *errorCode)
{
    int i = 0;
    char* word;
    *errorCode = NULL_INITIAL; /* reset error code to initial state */

    while (isspace(str[i]))
        i++;

    if (str[i] == '\0') { /* if the string is empty or contains only whitespace */
        *errorCode = END_OF_LINE;
        return NULL;
    }

    if (str[i] == ',') { /* if the first character is ';' or ',' */
        word = malloc(COMMA_LENGTH + NULL_TERMINATOR); /* 1 for illegal character and 1 for '\0' */
        if (word == NULL) {
            *errorCode = MALLOC_ERROR;
            return NULL; 
        }
        word[0] = ','; /* set the first character to ',' */
        word[1] = '\0';  /* set the last character to '\0' */
        i++; /* skip the illegal character */
    }
    else {
        int start = i;
        while (str[i] != '\0' && str[i] != ',' && !isspace(str[i]))  /* find the end of the word */
            i++;

        word = malloc(i - start + 1); /* +1 for '\0' */
        if (word == NULL) {
            *errorCode = MALLOC_ERROR;
            return NULL;
        }

        strncpy(word, str + start, i - start);
        word[i - start] = '\0';
    }

    while (isspace(str[i])) 
        i++;

    *errorCode = SCAN_SUCCESS; /* set error code to success */
    return word;
}

char *cutFirstToken(char *str, ErrCode *errorCode)
{
    char *word;
    *errorCode = NULL_INITIAL; /* reset error code to initial state */
    word = getFirstToken(str, errorCode);

    if (*errorCode != SCAN_SUCCESS) /* if an error occurred while getting the first word */
        return NULL;

    cutnChar(str, strlen(word)); /* cut the first word from the string */
    return word;
}

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
        if ((errorCode = isValidLabel(token)) != SCAN_SUCCESS) { /* check if the label is valid */
            free(token);
            return errorCode; /* return the error code if the label is invalid */
        }
        pline->label = token;
        token = cutFirstToken(line, &errorCode); /* get the next token after the label */
    }
    else
        pline->label = NULL; /* set the label to NULL */
    
    /* here we check for errors of getFirstToken() (may be the first token may be the second)*/
    if (errorCode != SCAN_SUCCESS) 
        return errorCode;
    
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

ErrCode isValidLabel(char *label)
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

    /* note: we do not check the last character cas it is ':' (we only call isValidLabel() if there is
     a colon and if we didn't find any other ':' in the range 1 to len-1 it must be in len) */
    return SCAN_SUCCESS; /* valid label */
}

Bool isEndOfLine(char *str)
{
    int i = 0;
    while (isspace(str[i])) /* skip leading whitespace */
        i++;
    
    if (str[i] == '\0') /* if the string is empty or contains only whitespace */
        return TRUE; 
    return FALSE; /* if there are non-whitespace characters, it is not end of line */
}

void freeScannedLine(parsedLine *pline) {
    if (pline == NULL) 
        return; /* if the parsedLine is NULL, do nothing */

    if (pline->restOfLine != NULL) 
        free(pline->restOfLine);
    if (pline->label != NULL) 
        free(pline->label);
    free(pline); /* free the parsedLine structure itself */
}