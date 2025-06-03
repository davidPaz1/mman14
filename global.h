#ifndef DEFINE_H
#define DEFINE_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
/* defines */
#define NULL_TERMINATOR 1

#define MAX_INFILE_LENGTH 5 /*81 +1 for null terminator */
#define MAX_LABEL_SIZE 30 

#define BASE2_INSTRUCTION_LENGTH 10
#define BASE4_INSTRUCTION_LENGTH 5

#define REGISTER_SIZE 10

#define NULL_INITIAL 2 /* used to "forget" previous errorCode */

/*enums*/
typedef enum boolean {
    FALSE = 0,
    TRUE = 1
} Bool;

/* error codes */
typedef enum ErrorCode {
    SUCCESS = 3, 
    EOF_REACHED = 4,
    MALLOC_ERROR = 5,
    FILE_READ_ERROR = 6,
    FILE_WRITE_ERROR = 7,
    LINE_LONGER_THEN_MAX = 8
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
int executePreprocessor(char *inputFileName);

#endif