#include "global.h"
#include "error.h"
#include "scan.h"
#include "util.h"
#define OVER_LENGTH  1 /* overlength should store \n or \r if line is under MAX_LINE_FILE_LENGTH */

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

scannedLine* readLineType(FILE *fp, ErrCode *errorCode)
{
    scannedLine *lineRead = malloc(sizeof(scannedLine));
    if (lineRead == NULL) {
        *errorCode = MALLOC_ERROR;
        return NULL;
    }
    lineRead->line = readLine(fp, errorCode);
    if (lineRead->line == NULL) {
        free(lineRead);
        return NULL;
    }
    
    determineLineType(lineRead);
    return lineRead;
}

FILE* openFile(char *filename, char *ending, char *mode, ErrCode *errorCode)
{
    FILE *fp;
    char* fullFileName = mergeStrings(filename, ending); /* merge filename and ending to create full file name */
    *errorCode = NULL_INITIAL; /* reset error code to initial state */

    if (fullFileName == NULL) {
        *errorCode = MALLOC_ERROR;
        return NULL;
    }

    fp = fopen(fullFileName, mode); /* open the file with the given mode */
    if (fp == NULL) {
        if (strcmp(mode, "r") == 0) 
            *errorCode = FILE_READ_ERROR;
        else if (strcmp(mode, "w") == 0) 
            *errorCode = FILE_WRITE_ERROR;
        else 
            *errorCode = INVALID_FILE_MODE;
    }

    if (*errorCode == NULL_INITIAL) /* if no error occurred */
    *errorCode = SCAN_SUCCESS; /* set error code to success */
    
    free(fullFileName); /* free the allocated memory for fullFileName */
    return fp;
}

char* getFirstWord(char **strPtr, ErrCode *errorCode)
{
    int i = 0;
    char *str = *strPtr;
    char* word;
    *errorCode = NULL_INITIAL; /* reset error code to initial state */
    while (isspace(str[i]))
        i++;

    if (str[i] == '\0') { /* if the string is empty or contains only whitespace */
        *errorCode = END_OF_LINE;
        return NULL; /* return NULL */
    }

    if (str[i] == ',') { /* if the first character is ';' or ',' */
        word = malloc(2); /* 1 for illegal character and 1 for '\0' */
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
        while (str[i] != '\0' && str[i] != ',' && !isspace(str[i])) {
            i++;
        }

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

char *cutFirstWord(char **strPtr, ErrCode *errorCode)
{
    char *word;
    *errorCode = NULL_INITIAL; /* reset error code to initial state */
    word = getFirstWord(strPtr, errorCode);

    if (*errorCode != SCAN_SUCCESS) /* if an error occurred while getting the first word */
        return NULL;
    
    cutnChar(*strPtr, strlen(word)); /* cut the first word from the string */
    return NULL;
}

lineType* determineLineType(scannedLine *sLine) /* add errorcode */
{
    int i = 0;
    lineType* type = malloc(sizeof(lineType)); /* pointer to the type of the line */
    char* dupLine = strDup(sLine->line); /* duplicate the line to avoid modifying the original line */

    if (dupLine == NULL) {
        *type = NULL_INITIAL; /* malloc failed */
        return type; /* return NULL_INITIAL */
    }

    *type = -1; /* default type is empty line */
    while (isspace(dupLine[i])) /* skip leading whitespace */
        i++;
    cutnChar(dupLine, i); /* cut the leading whitespace */
    sLine->isLabel = isLabel(dupLine); /* check if the line has a label, this function is not implemented yet */
    if (dupLine[0] == '\0') { /* if the line is empty */
        *type = EMPTY_LINE;
    } else if (dupLine[0] == ';') { /* if the line is a comment */
        *type = COMMENT_LINE;
    /*} else  if (isOperationName(getFirstWord(dupLine))) {  if the line is an instruction */
        *type = INSTRUCTION_LINE;
    } else if (strchr(dupLine, '.') != NULL) { /* if the line is a data or string line */
        if (strstr(dupLine, ".data") != NULL || strstr(dupLine, ".extern") != NULL) {
            *type = DATA_LINE;
        } else if (strstr(dupLine, ".string") != NULL) {
            *type = STRING_LINE;
        } else if (strstr(dupLine, ".entry") != NULL) {
            *type = ENTRY_LINE;
        } else {
            *type = EXTERN_LINE; /* default to extern line */
        }
    }
    return type; /* return the type of the line */
}

Bool isLabel(char *str)
{
    /* UNSET */
    return TRUE;
}