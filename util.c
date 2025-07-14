#include "util.h"
#include "global.h"

/* scanning functions */

/**
 * readLine - reads a line from the file and returns it as a string.
 * if the line is longer than MAX_LINE_FILE_LENGTH, it will return NULL and set errorCode to LINE_TOO_LONG_E.
 * if memory allocation fails, returns NULL and sets errorCode to MALLOC_ERROR_F.
 * if end of file is reached, returns NULL and sets errorCode to EOF_REACHED_S.
 * errorCode:  EOF_REACHED_S , FILE_READ_ERROR_F , LINE_TOO_LONG_E , FILE_READ_ERROR_F, UTIL_SUCCESS_S
 */
char* readLine(FILE *fp, ErrCode *errorCode) {

    /* overlength should store \n or \r if line is under MAX_LINE_FILE_LENGTH */
    char *line = malloc(MAX_LINE_FILE_LENGTH + OVER_LENGTH  + NULL_TERMINATOR);
    char next; 
    *errorCode = NULL_INITIAL; /* reset error code to initial state */

    if (line == NULL) {
        *errorCode = MALLOC_ERROR_F;
        return NULL; /* malloc failed */
    }

    /* overlength should store \n or \r if line is under MAX_LINE_FILE_LENGTH */
    if (fgets(line, MAX_LINE_FILE_LENGTH + OVER_LENGTH  + NULL_TERMINATOR, fp) == NULL) {
        free(line);
        if (feof(fp))
            *errorCode = EOF_REACHED_S;
        else 
            *errorCode = FILE_READ_ERROR_F;
        return NULL; /* end of file or error */
    }

    next = fgetc(fp); /* peek on the next character */
    if (next != EOF)  /* if the next character is not EOF, it means the line is longer than MAX_LINE_FILE_LENGTH */
        ungetc(next, fp); /* put back the next character to the input stream */
    
    if (strchr(line, '\n') == NULL && strchr(line, '\r') == NULL && next != EOF) {
        char c; /* discard the rest of the line from the input stream */
        while ((c = fgetc(fp)) != '\n' && c != EOF);
        *errorCode = LINE_TOO_LONG_E;
        free(line);
        return NULL; /* line is too long, return NULL */
    }
    else {
        line[strcspn(line, "\r\n")] = '\0'; /* remove the newline or carriage return character from the end of the line */
        if ( strlen(line) == MAX_LINE_FILE_LENGTH)  /* if the line is exactly MAX_LINE_FILE_LENGTH characters long, it has a remaining '\n' */
            (void) fgetc(fp);  /* discard the unused character '\n' from the input stream */
        *errorCode = UTIL_SUCCESS_S; /* set error code to success */
    }
    return line;
}

/**
 * getFirstToken - returns the first token of a string, skipping leading spaces
 * if the string is empty or contains only whitespace, returns NULL and sets errorCode to END_OF_LINE_S.
 * if memory allocation fails, returns NULL and sets errorCode to MALLOC_ERROR_F.
 * if reaches a character '[' , it stops and returns the token.
 * errorCode:  END_OF_LINE_S , MALLOC_ERROR_F, UTIL_SUCCESS_S
 */
char* getFirstToken(const char *str, ErrCode *errorCode)
{
    int i = 0, startNonSpace = 0; /* index of the first non-space character */
    char* token;
    *errorCode = NULL_INITIAL; /* reset error code to initial state */

    while (isspace(str[i]))
        i++;

    if (str[i] == '\0') { /* if the string is empty or contains only whitespace */
        *errorCode = END_OF_LINE_S;
        return NULL;
    }

    startNonSpace = i;
    /* read the first character separately because we would get stuck on '[' */
    if (str[i] != '\0' && !isspace(str[i]))
        i++;
    while (str[i] != '\0' && str[i] != '[' && !isspace(str[i]))  /* find the end of the token */
        i++;

    token = malloc(i - startNonSpace + 1); /* +1 for '\0' */
    if (token == NULL) {
        *errorCode = MALLOC_ERROR_F;
        return NULL;
    }

    strncpy(token, str + startNonSpace, i - startNonSpace);
    token[i - startNonSpace] = '\0';

    while (isspace(str[i]))
        i++;

    *errorCode = UTIL_SUCCESS_S; /* set error code to success */
    return token;
}

/**
 * cutFirstToken - same as getFirstToken, but also cuts token from the string.
 * errorCode:  END_OF_LINE_S , MALLOC_ERROR_F, UTIL_SUCCESS_S
 */
char *cutFirstToken(char *str, ErrCode *errorCode)
{
    char *token;
    *errorCode = NULL_INITIAL; /* reset error code to initial state */
    token = getFirstToken(str, errorCode);

    if (*errorCode != UTIL_SUCCESS_S) /* if an error occurred while getting the first token */
        return NULL;

    cutnChar(str, strlen(token)); /* cut the first token from the string */
    return token;
}

/* string manipulation functions */

char* strDup(const char* src) {
    char* dest = malloc(strlen(src) + NULL_TERMINATOR);
    if (dest != NULL) 
        strcpy(dest, src);
    return dest;
}

char* mergeStrings(const char *start, const char *end)
{
    char *merged;
    if (start == NULL || end == NULL)
        return NULL;
    

    merged = malloc(strlen(start) + strlen(end) + NULL_TERMINATOR); /* enough room for merged string */
    if (merged == NULL) /* malloc failed */
        return NULL;


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

FILE* openFile(const char *filename,const char *ending, const char *mode, ErrCode *errorCode)
{
    FILE *fp;
    char* fullFileName = mergeStrings(filename, ending); /* merge filename and ending to create full file name */
    *errorCode = NULL_INITIAL; /* reset error code to initial state */

    if (fullFileName == NULL) {
        *errorCode = MALLOC_ERROR_F;
        return NULL;
    }

    fp = fopen(fullFileName, mode); /* open the file with the given mode */
    if (fp == NULL) {
        if (strcmp(mode, "r") == 0) 
            *errorCode = FILE_READ_ERROR_F;
        else if (strcmp(mode, "w") == 0) 
            *errorCode = FILE_WRITE_ERROR_F;
        else 
            *errorCode = INVALID_FILE_MODE_F;
    }

    if (*errorCode == NULL_INITIAL) /* if no error occurred */
        *errorCode = UTIL_SUCCESS_S; /* set error code to success */
    
    free(fullFileName); /* free the allocated memory for fullFileName */
    return fp;
}

/**
 * delFile - deletes a file with the given filename and ending.
 * errorCode:  MALLOC_ERROR_File_Del_F, FILE_DELETE_ERROR_F, UTIL_SUCCESS_S
 */
ErrCode delFile(const char *filename, const char *ending)
{
    char *fullFileName;
    
    fullFileName = mergeStrings(filename, ending);
    if (fullFileName == NULL) {
        return MALLOC_ERROR_File_Del_F;
    }

    if (remove(fullFileName) != 0) { /* if file delete was unsuccessful */
        free(fullFileName);
        return FILE_DELETE_ERROR_F;
    }

    free(fullFileName);
    return UTIL_SUCCESS_S;
}
