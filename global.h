#ifndef DEFINE_H
#define DEFINE_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* defines */
#define NULL_TERMINATOR 1

#define MAX_LINE_FILE_LENGTH 80 /* maximum length of a line in the file */
#define MAX_OB_FILE_LENGTH 256 /* maximum length of an object file name */
#define MAX_LABEL_LENGTH 30 /* maximum length of a label name */
#define MAX_MACRO_LENGTH 30 /* maximum length of a macro name */

#define BASE2_INSTRUCTION_LENGTH 10
#define BASE4_INSTRUCTION_LENGTH 5

#define REGISTER_SIZE 10

/*enums*/
typedef enum boolean {
    FALSE = 0,
    TRUE = 1
} Bool;

/* may be used idk */
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

typedef enum directiveLineType {
    DATA_LINE = 0, /* data line */
    STRING_LINE,   /* string line */
    ENTRY_LINE,    /* entry line */
    EXTERN_LINE    /* extern line */
} directiveLineType;
/* stop */

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


#endif