#include <ctype.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* error codes*/
#define NO_ERROR 0 /*used to start identifying the error codes*/
#define FOUND_COMMA 1
#define FOUND_MAT 2
#define FOUND_SCALAR 3
#define EXTRANEOUS_TEXT 4
#define COMMAND_NAME_INVALID 5
#define MATRIX_NAME_INVALID 6
#define SCALAR_INVALID 7
#define INVALID_COMMA 8
#define MISSING_MATRIX_NAME 9
#define MISSING_COMMA 10
#define MISSING_SCALAR 11
#define MISSING_ARGUMENT 12
#define COMMMA_INSTEAD_OF_MATRIX 13
#define MULTIPLE_CONSECUTIVE_COMMAS 14
#define UNEXPECTED_END_OF_LINE 15

#define STOP_COMMAND_REACHED -2
#define EOF_REACHED -1 /*EOF reached*/
#define VALID_SCAN 0 /*valid scan*/
#define MALLOC_ERROR 1 /*malloc failed*/
typedef struct mat
{
    /* data */
} mat;


/* getFirstWord() gets the first word from the str and deletes it from it + removes spaces*/
char* getFirstWord(char *str)
{
    int i = 0, start = 0;
    char* word;
    while (isspace(str[i]))
        i++;
    

    if (str[i] == ','){
        i++; /*skip the comma*/
        word = malloc(2); /* 1 for ',' and 1 for '\0' */
        if (word == NULL) {
            return NULL; /*malloc failed*/
        }
        word[0] = ','; /* set the first character to ',' */
        word[1] = '\0';  /* set the last character to '\0' */
    }
    else {
        start = i;
        while (str[i] != EOF && str[i] != '\0' && str[i] != ',' && !isspace(str[i])) {
            i++; 
        }

        word = malloc(i - start + 1); /* +1 for '\0' */
        if (word == NULL)  /*malloc failed*/
            return NULL; 
        
        strncpy(word, str + start, i - start);
        word[i - start] = '\0';
        
        /* Move the rest of the string to the beginning */
    }
    
    while (isspace(str[i])) 
        i++;
    
    memmove(str, str + i, strlen(str) - i + 1); /*move the rest of the string to the beginning*/
    return word;
}

/* returns -1 if stop comand is reached, 0 if reading line failed, 1 if reading line success*/
int parseCommand(mat* MAT_A , mat* MAT_B , mat* MAT_C , mat* MAT_D , mat* MAT_E , mat* MAT_F)
{
    int isEOF, errorCode; /*isEOF is used to check if EOF is reached and errorCode is used to check if the command is valid*/
    char *command, *line; /*command is used to store the command and line is used to store the line*/

    isEOF = readLine(&line); /*read the line and if EOF is reached set isEOF to -1*/
    printf("%s\n", line); /*print the line*/
    command = getFirstWord(line); /*copy the command to the command variable*/

    if (command == NULL || line == NULL) /*malloc failed*/
        return MALLOC_ERROR; 
    

    if (strcmp(command, "stop") == 0) { /*if the command is stop*/
        free(command);
        free(line);
        return STOP_COMMAND_REACHED + isEOF; /*STOP + isEOF = stop && isEOF*/
    }
    else if (strcmp(command, "add_mat") == 0)
        errorCode = parseAddMat(line, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F);
    else if (strcmp(command, "sub_mat") == 0)
        errorCode = parseSubMat(line, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F);
    else if (strcmp(command, "mul_mat") == 0)
        errorCode = parseMulMat(line, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F);
    else if (strcmp(command, "mul_scalar") == 0)
        errorCode = parseMulScalar(line, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F);
    else if (strcmp(command, "print_mat") == 0) 
        errorCode = parsePrintMat(line, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F);
    else if (strcmp(command, "read_mat") == 0)
        errorCode = parseReadMat(line, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F);
    else if (strcmp(command, "trans_mat" ) == 0)
        errorCode = parseTransMat(line, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F);
    else
        errorCode = COMMAND_NAME_INVALID; /*command name is invalid*/
    
    free(command);
    free(line);
    if (errorCode != VALID_SCAN) { /*if the command is invalid*/
        errorMessage(errorCode); /*print the error message*/
    }
    return isEOF;
}

void errorMessage(int errorCode){
    printf("error - ");
    switch (errorCode){
        case MALLOC_ERROR:
            printf("malloc failed\n");
            break;
        case EXTRANEOUS_TEXT:
            printf("there is extraneous text after the end of the command\n");
            break;
        case COMMAND_NAME_INVALID:
            printf("the command name isnt an set command\n");
            break;
        case MATRIX_NAME_INVALID:
            printf("the matrix name isnt an set matrix\n");
            break;
        case SCALAR_INVALID:
            printf("the argument should be a real number\n");
            break;
        case INVALID_COMMA:
            printf("enter gibberish instead of a comma\n");
            break;
        case MISSING_MATRIX_NAME:
            printf("matrix name argument is missing\n");
            break;
        case MISSING_SCALAR:
            printf("scalar argument is missing\n");
            break;
        case MISSING_COMMA:
            printf("there is a missing comma\n");
            break;
        case MISSING_ARGUMENT:
            printf("missing an entire argument\n");
            break;
        case COMMMA_INSTEAD_OF_MATRIX:
            printf("there is a comma instead of the matrix name\n");
            break;
        case MULTIPLE_CONSECUTIVE_COMMAS:
            printf("multiple consecutive commas\n");
            break;
        default:
            printf("unknown error\n"); /* should never happen*/
            break;
    }
}
