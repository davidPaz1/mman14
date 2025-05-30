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

/*enums*/
typedef enum bool {
    FALSE = 0,
    TRUE = 1
} bool;

/* error codes */
typedef enum ErrorCode {
    EOF_REACHED = 3,
    MALLOC_ERROR = 4,
    FILE_READ_ERROR = 5,
    FILE_WRITE_ERROR = 6
} ErrorCode;

/* opcodes */
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

/* registers */
typedef enum registers{
    r0 = 0, 
    r1,
    r2,
    r3,
    r4,
    r5,
    r6,
    r7,
    NOT_REG = -1
} registers; 


/* struct */

/* Defines a first word in an instruction */
typedef struct word { /* bitfield struct */
    unsigned int ARE: 2;
    unsigned int dest_op_addr: 2;
    unsigned int src_op_addr: 2;
    unsigned int op_code: 4;
} word;

/* functions */
char* base4(int *word);
char* readLine(FILE *fp, int *errorCode);
FILE* openFile(char* filename, char* ending , char* mode, int* errorCode);
#endif