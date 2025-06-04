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
    
    if (fgets(line, MAX_INFILE_LENGTH + NULL_TERMINATOR, fp) == NULL) {
        free(line);
        if (feof(fp))
            *errorCode = EOF_REACHED;
        else 
            *errorCode = FILE_READ_ERROR;
        return NULL; /* end of file or error */
    }
    *errorCode = SUCCESS; /* set error code to success */

    len = strlen(line); /* get the length of the line read */
    printf ("len %d\n", len); /* debug print to check the length of the line */
    int i;
    for (i = 0; i < len + 1; i++) { /* iterate through the line */
        printf("line[%d] = %c in int %d\n", i, line[i], line[i]); /* debug print to check each character */
    }

    if (strchr(line, '\n') != NULL || strchr(line, '\r') != NULL) /* check if the line is longer than MAX_INFILE_LENGTH */
        printf("line is okkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk\n");
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
