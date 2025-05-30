#ifndef DEFINE_H
#define DEFINE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* defines */
#define NULL_TERMINATOR 1

#define MAX_INFILE_LENGTH 81 /* +1 for null terminator */

#define BASE2_INSTRUCTION_LENGTH 10
#define BASE4_INSTRUCTION_LENGTH 5

#define REGISTER_SIZE 10

#define NULL_INITIAL 2 /* used to "forget" previous errorCode */

/* error codes */
typedef enum ErrorCode {
    EOF_REACHED = 3,
    MALLOC_ERROR = 4,
    FILE_READ_ERROR = 5,
    FILE_WRITE_ERROR = 6
} ErrorCode;

typedef enum opCode {
    mov = 0,
    cmp,
    add,
    sub,
    not,
    clr,
    lea,
    inc,
    dec,
    jmp,
    bne,
    red,
    prn,
    jsr,
    rts,
    stop,
    invalid = -1
} opCode;

/*enums*/
typedef enum boolean {
    FALSE = 0,
    TRUE = 1
} Bool;


/* functions */
char* base4(int *word);
char* readLine(FILE *fp, int *errorCode);
FILE* openFile(char* filename, char* ending , char* mode, int* errorCode);
#endif