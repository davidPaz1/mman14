#include "global.h"
#include "scan.h"

#define overlength 1 /* overlength should store \n or \r if line is under MAX_LINE_FILE_LENGTH */

char* readLine(FILE *fp, int *errorCode) {
    char *line = malloc(MAX_LINE_FILE_LENGTH + overlength + NULL_TERMINATOR); /* overlength should store \n or \r if line is under MAX_LINE_FILE_LENGTH */
    short int len; /* length of the line read */
    *errorCode = NULL_INITIAL; /* reset error code to initial state */
    if (line == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        *errorCode = MALLOC_ERROR;
        return NULL; /* malloc failed */
    }

    if (fgets(line, MAX_LINE_FILE_LENGTH + overlength + NULL_TERMINATOR, fp) == NULL) {  /* overlength should store \n or \r if line is under MAX_LINE_FILE_LENGTH */
        free(line);
        if (feof(fp))
            *errorCode = EOF_REACHED;
        else 
            *errorCode = FILE_READ_ERROR;
        return NULL; /* end of file or error */
    }

    char next = fgetc(fp); /* peek on the next character */
    if (next != EOF) { /* if the next character is not EOF, it means the line is longer than MAX_LINE_FILE_LENGTH */
        ungetc(next, fp); /* put back the next character to the input stream */
    }
    if (strchr(line, '\n') == NULL && strchr(line, '\r') == NULL && next != EOF) {
        char c; /* discard the rest of the line from the input stream */
        while ((c = fgetc(fp)) != '\n' && c != EOF);
        *errorCode = LINE_TOO_LONG;
        free(line);
        return NULL; /* line is too long, return NULL */
    }
    else {
        line[strcspn(line, "\r\n")] = '\0'; /* remove the newline or carriage return character from the end of the line */
        len = strlen(line); /* get the length of the line read */
        if (len == MAX_LINE_FILE_LENGTH)  /* if the line is exactly MAX_LINE_FILE_LENGTH characters long, it has a remaining '\n' */
            (void) fgetc(fp);  /* discard the unused character '\n' from the input stream */
        *errorCode = SUCCESS; /* set error code to success */
    }
    return line;
}

FILE *openFile(char *filename, char *ending, char *mode, int *errorCode)
{
    FILE *fp;
    int len = strlen(filename) + strlen(ending) + NULL_TERMINATOR; /* length of the full file name */
    char* fullFileName = malloc(len); /* full file name including the ending */
    *errorCode = NULL_INITIAL; /* reset error code to initial state */

    if (fullFileName == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        *errorCode = MALLOC_ERROR;
        return NULL;
    }

    strcpy(fullFileName, filename); /* copy filename to fullFileName */
    strcat(fullFileName, ending); /* add file ending */

    fp = fopen(fullFileName, mode); /* open the file with the given mode */
    if (fp == NULL) {
        if (strcmp(mode, "r") == 0) {
            fprintf(stderr, "Error opening file for reading: %s\n", fullFileName);
            *errorCode = FILE_READ_ERROR;
        } else if (strcmp(mode, "w") == 0) {
            fprintf(stderr, "Error opening file for writing: %s\n", fullFileName);
            *errorCode = FILE_WRITE_ERROR;
        } else {
            fprintf(stderr, "david you are a fucking idiot, you should never reach this point\n"); /*test123*/
            fprintf(stderr, "Error opening file: %s with mode: %s\n", fullFileName, mode);
            *errorCode = NULL_INITIAL; /* default error code for unknown mode */
        }
    }
    free(fullFileName); /* free the allocated memory for fullFileName */
    return fp;
}
