#include "global.h"

char* readLine(FILE *fp, int *errorCode) {
    char *line = malloc(MAX_INFILE_LENGTH);
    *errorCode = NULL_INITIAL; /* Reset error code to initial state */
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
        return NULL; /* End of file or error */
    }

    short int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0'; /* Remove newline character */
    }
    

    return line;
}
