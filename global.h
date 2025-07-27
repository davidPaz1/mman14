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
#define MAX_MEMORY_SIZE 256 /* maximum memory size */
#define MAX_LABEL_LENGTH 30 /* maximum length of a label name */
#define MAX_MACRO_LENGTH 30 /* maximum length of a macro name */

#define BASE2_INSTRUCTION_LENGTH 10
#define BASE4_INSTRUCTION_LENGTH 5

#define MAX_10BIT_INT 511
#define MIN_10BIT_INT -512
#define MAX_8BIT_INT 127
#define MIN_8BIT_INT -128


/*enums*/
typedef enum boolean {
    FALSE = 0,
    TRUE = 1
} Bool;

/* code image structs */

typedef struct FirstWord { /* first word of an instruction */
    unsigned int ARE: 2;
    unsigned int destAddress: 2;
    unsigned int srcAddress: 2;
    unsigned int opCode: 4;
} FirstWord;

#define SRC_REGISTER_ADDRESS_SIZE 4
#define DEST_REG_SHIFT(x) (x << SRC_REGISTER_ADDRESS_SIZE) /* macro to shift the destination register to the correct position (bits 6 - 9) */

typedef struct NumberWord { /* defines a word that contains a number */
    unsigned int ARE: 2;
    signed int numberVal: 8;
} NumberWord;

typedef struct RegisterWord { /* defines a word that contains a register */
    unsigned int ARE: 2;
    unsigned int registerSrc: 4; /* source register */
    unsigned int registerDest: 4; /* destination register */
} RegisterWord;

typedef struct MatrixRegistersWord { /* defines a word that contains a matrix registers */
    unsigned int ARE: 2;
    unsigned int registerRow: 4; /* row number in the matrix */
    unsigned int registerCol: 4; /* column number in the matrix */
} MatrixRegistersWord;

typedef struct LabelWord { /* defines a word that contains a label */
    unsigned int ARE: 2;
    unsigned int labelAddress: 8;
} LabelWord;

typedef union CodeWord {
    FirstWord firstWord;
    NumberWord numberWord;
    RegisterWord registerWord;
    MatrixRegistersWord matrixRegistersWord;
    LabelWord labelWord;
    unsigned int allBits : 10;
} CodeWord;

typedef struct DataWord {
    unsigned int value : 10; /* number of data items in the directive */
} DataWord;


#endif