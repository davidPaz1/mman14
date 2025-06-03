#include "global.h"
#include "scan.h"
char* readLine(FILE *fp, int *errorCode) {
    char *line = malloc(MAX_INFILE_LENGTH); /* allocate memory for the line to read */
    short int len; /* length of the line read */
    *errorCode = NULL_INITIAL; /* reset error code to initial state */
    if (line == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        *errorCode = MALLOC_ERROR;
        return NULL; /* malloc failed */
    }
    
    if (fgets(line, MAX_INFILE_LENGTH, fp) == NULL) {
        free(line);
        if (feof(fp))
            *errorCode = EOF_REACHED;
        else 
            *errorCode = FILE_READ_ERROR;
        return NULL; /* end of file or error */
    }

    len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0'; /* remove newline character */
    }

    return line;
}

FILE *openFile(char *filename, char *ending, char *mode, int *errorCode)
{
    FILE *fp;
    *errorCode = NULL_INITIAL; /* reset error code to initial state */
    int len = strlen(filename) + strlen(ending) + NULL_TERMINATOR; /* calculate length of full file name */
    char fullFileName[len];

    strcpy(fullFileName, filename); /* copy filename to fullFileName */
    strcat(fullFileName, ending); /* add file ending */

    fp = fopen(fullFileName, mode);
    if (fp == NULL) {
        if (strcmp(mode, "r") == 0) {
            fprintf(stderr, "Error opening file for reading: %s\n", fullFileName);
            *errorCode = FILE_READ_ERROR;
        } else if (strcmp(mode, "w") == 0) {
            fprintf(stderr, "Error opening file for writing: %s\n", fullFileName);
            *errorCode = FILE_WRITE_ERROR;
        } else {
            fprintf(stderr, "david you are a fucking idiot, you should never reach this point\n");
            fprintf(stderr, "Error opening file: %s with mode: %s\n", fullFileName, mode);
            *errorCode = NULL_INITIAL; /* default error code for unknown mode */
        }
    }
    return fp;
}
