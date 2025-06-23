#include "global.h"
#include "util.h"

/* scanning functions */
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

/* string manipulation functions */

char* strDup(char* src) {
    char* dest = malloc(strlen(src) + NULL_TERMINATOR);
    if (dest != NULL) 
        strcpy(dest, src);
    return dest;
}

char *mergeStrings(char *start, char *end)
{
    char *merged;
    if (start == NULL || end == NULL)
        return NULL;
    

    merged = malloc(strlen(start) + strlen(end) + NULL_TERMINATOR); /* enough room for merged string */
    if (merged == NULL) { /* malloc failed */
        return NULL;
    }

    strcpy(merged, start); /* copy the first string */
    strcat(merged, end); /* concatenate the two strings */
    return merged;
}

void cutnChar(char *str, int n)
{
    int spacesCount = 0, cuttingLength = n; /* length to cut from the string */
    while (isspace(str[spacesCount])) 
        spacesCount++;
    
    cuttingLength += spacesCount; /* adjust the length to cut based on leading spaces */

    memmove(str, str + cuttingLength, strlen(str) - cuttingLength + 1); /* Move the string left by n characters */

    cuttingLength = 0; /* reset cutting length to 0 for deleting trailing spaces */
    while (isspace(str[cuttingLength])) 
        cuttingLength++;

    memmove(str, str + cuttingLength, strlen(str) - cuttingLength + 1); /* Move the string left by the number of leading spaces */
}

/* file management functions */

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

ErrCode delFile(char *filename, char *ending)
{
    char *fullFileName;

    fullFileName = mergeStrings(filename, ending);
    if (fullFileName == NULL) {
        return MALLOC_ERROR;
    }

    if (remove(fullFileName) != 0) { /* if file delete was unsuccessful */
        free(fullFileName);
        return FILE_DELETE_ERROR;
    }

    free(fullFileName);
    return SCAN_SUCCESS;
}
