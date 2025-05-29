#include "mymat.h"
#include <ctype.h> 
#include <string.h>

/* reading line definitions */
#define START_SIZE 16 /*start size of the buffer*/
#define DOUBLE(x) ((x) * 2) /*double the size of x*/

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

/*
we start in parseCommand(), which reads a line from the input with readLine() and stores it in the line variable.
then we use getFirstWord() to get the first word of the line, which is the command name.
we identify the command name and send the remaining line to the corresponding parseCommand_name() function.
in parseCommand_name(), we use getSomething() to check whether the command syntax is followed correctly.
getSomething() expects the first word to be the something (matrix name, scalar, comma) and returns an appropriate error code if it is not.
if an error occurs, it sends an error code to errorMessage(), which prints the corresponding error message.
*/

/*returns 0 if reading line failed , -1 if EOF is reached*/
int readLine(char** pstr) { 
    size_t size = START_SIZE;  /*initial buffer size*/
    size_t len = 0;
    int ch;
    char *temp, *final; /*temp and final are used for realloc*/
    char* buffer = malloc(size); /*allocate memory for the buffer*/
    *pstr = NULL; /*set pstr to NULL*/
    if (buffer == NULL)
        return MALLOC_ERROR; /*malloc failed*/ 
        
    while (isspace(ch = getchar())); /*skip leading whitespace*/

    if (ch == EOF) { /*if EOF is reached*/
        free(buffer); /*free the buffer*/
        *pstr = NULL; /*set pstr to NULL*/
        return EOF_REACHED; /*return EOF*/
    }
    buffer[len++] = ch;

    /* read the line until EOF or newline is reached */
    while ((ch = getchar()) != EOF && ch != '\n') {
        if (len + 1 >= size) { /* if the buffer is full*/
            size = DOUBLE(size);
            temp = realloc(buffer, size); /* inlarge the buffer*/
            if (temp == NULL){ /*realloc failed*/
                free(buffer); 
                *pstr = NULL; /*set pstr to NULL*/
                return MALLOC_ERROR; /*return failure to read the line*/
            }
            buffer = temp;
        }

        buffer[len++] = ch;
    }
    
    buffer[len] = '\0'; 
    
    /*Shrink to eliminate unused space*/
    final = realloc(buffer, len + 1); /* +1 for '\0'*/
    if (final != NULL) 
        *pstr = final; /*set pstr to the final buffer*/
    else 
        *pstr = buffer; /*set pstr to the unshortened buffer*/
    
    return (ch == EOF) ? EOF_REACHED : VALID_SCAN; /*return length or EOF*/
}

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

/*parseCommand_name() functions are used to parse the command and check if the syntax is correct*/
int parseReadMat(char *str, mat *MAT_A, mat *MAT_B, mat *MAT_C, mat *MAT_D, mat *MAT_E, mat *MAT_F)
{
    int errorCode = VALID_SCAN; /*errorCode is used to check if the command is valid*/
    mat *matrix1;
    float toRead[ALL_MAT_LEN];
    int i = 0;

    matrix1 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the first matrix*/
    if (matrix1 == NULL) /*if the matrix is NULL*/
        return errorCode;
    
    
    while (i < ALL_MAT_LEN) {
        getComma(str , &errorCode);
        if (errorCode == MISSING_ARGUMENT) { /*if its the last argument*/
            read_mat(matrix1, toRead, i);
            return VALID_SCAN;
        }
        if (errorCode != FOUND_COMMA) 
            return errorCode;

        toRead[i] = getScalar(str, &errorCode); /*get the scalar*/
        if (errorCode != FOUND_SCALAR) 
            return errorCode;
        i++;
    }
    
    read_mat(matrix1, toRead, i); /*read the matrix*/
    return VALID_SCAN;
}

int parsePrintMat(char *str, mat *MAT_A, mat *MAT_B, mat *MAT_C, mat *MAT_D, mat *MAT_E, mat *MAT_F)
{
    int errorCode = VALID_SCAN; /*errorCode is used to check if the command is valid*/
    mat *matrix1;

    matrix1 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the first matrix*/
    if (errorCode != FOUND_MAT)
        return errorCode;
    
    if (str[0] != '\0') 
        return EXTRANEOUS_TEXT; /*extraneous text after the end of the command*/
    
    print_mat(*matrix1);  /*print the matrix*/
    return VALID_SCAN;
}

int parseAddMat(char *str, mat *MAT_A, mat *MAT_B, mat *MAT_C, mat *MAT_D, mat *MAT_E, mat *MAT_F)
{
    int errorCode; /*errorCode is used to check if the command is valid*/
    mat *matrix1, *matrix2, *matrix3;

    matrix1 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the first matrix*/
    if (errorCode != FOUND_MAT)
        return errorCode;
    
    getComma(str , &errorCode);
    if (errorCode != FOUND_COMMA) 
        return errorCode; 
    
    matrix2 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the second matrix*/
    if (errorCode != FOUND_MAT) 
        return errorCode;
    
    getComma(str , &errorCode);
    if (errorCode != FOUND_COMMA) 
        return errorCode; 
    
    matrix3 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the third matrix*/
    if (errorCode != FOUND_MAT) 
        return errorCode;
    
    if (str[0] != '\0') 
        return EXTRANEOUS_TEXT; /*extraneous text after the end of the command*/
    
    add_mat(matrix1, matrix2, matrix3);  /*add the matrices*/
    return VALID_SCAN;
}

int parseSubMat(char *str, mat *MAT_A, mat *MAT_B, mat *MAT_C, mat *MAT_D, mat *MAT_E, mat *MAT_F)
{
    int errorCode = VALID_SCAN; /*errorCode is used to check if the command is valid*/
    mat *matrix1, *matrix2, *matrix3;

    matrix1 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the first matrix*/
    if (errorCode != FOUND_MAT)
        return errorCode;
    
    getComma(str , &errorCode);
    if (errorCode != FOUND_COMMA) 
        return errorCode; 
    
    matrix2 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the second matrix*/
    if (errorCode != FOUND_MAT) 
        return errorCode;
    
    getComma(str , &errorCode);
    if (errorCode != FOUND_COMMA) 
        return errorCode; 
    
    matrix3 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the third matrix*/
    if (errorCode != FOUND_MAT) 
        return errorCode;
    
    if (str[0] != '\0') 
        return EXTRANEOUS_TEXT; /*extraneous text after the end of the command*/
    
    sub_mat(matrix1, matrix2, matrix3);  /*subtract the matrices*/
    return VALID_SCAN;
}

int parseMulMat(char *str, mat *MAT_A, mat *MAT_B, mat *MAT_C, mat *MAT_D, mat *MAT_E, mat *MAT_F)
{
    int errorCode = VALID_SCAN; /*errorCode is used to check if the command is valid*/
    mat *matrix1, *matrix2, *matrix3;

    matrix1 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the first matrix*/
    if (errorCode != FOUND_MAT)
        return errorCode;
    
    getComma(str , &errorCode);
    if (errorCode != FOUND_COMMA) 
        return errorCode; 
    
    matrix2 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the second matrix*/
    if (errorCode != FOUND_MAT) 
        return errorCode;
    
    getComma(str , &errorCode);
    if (errorCode != FOUND_COMMA) 
        return errorCode; 
    
    matrix3 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the third matrix*/
    if (errorCode != FOUND_MAT) 
        return errorCode;
    
    if (str[0] != '\0') 
        return EXTRANEOUS_TEXT; /*extraneous text after the end of the command*/
    
    mul_mat(matrix1, matrix2, matrix3);  /*multiply the matrices*/
    return VALID_SCAN;
}

int parseMulScalar(char *str, mat *MAT_A, mat *MAT_B, mat *MAT_C, mat *MAT_D, mat *MAT_E, mat *MAT_F)
{
    int errorCode = VALID_SCAN; /*errorCode is used to check if the command is valid*/
    mat *matrix1, *matrix2;
    float scalar;

    matrix1 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the first matrix*/
    if (errorCode != FOUND_MAT)
        return errorCode;
    
    getComma(str , &errorCode);
    if (errorCode != FOUND_COMMA) 
        return errorCode; 
    
    scalar = getScalar(str, &errorCode); /*get the scalar*/
    if (errorCode != FOUND_SCALAR) 
        return errorCode;
    
    getComma(str , &errorCode);
    if (errorCode != FOUND_COMMA) 
        return errorCode; 
    
    matrix2 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the second matrix*/
    if (errorCode != FOUND_MAT) 
        return errorCode;
    
    if (str[0] != '\0') 
        return EXTRANEOUS_TEXT; /*extraneous text after the end of the command*/
    
    mul_scalar(matrix1, scalar, matrix2);  /*multiply the matrix by the scalar*/
    return VALID_SCAN;
}

int parseTransMat(char *str, mat *MAT_A, mat *MAT_B, mat *MAT_C, mat *MAT_D, mat *MAT_E, mat *MAT_F)
{
    int errorCode = VALID_SCAN; /*errorCode is used to check if the command is valid*/
    mat *matrix1, *matrix2;

    matrix1 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the first matrix*/
    if (errorCode != FOUND_MAT)
        return errorCode;
    
    getComma(str , &errorCode);
    if (errorCode != FOUND_COMMA) 
        return errorCode; 
    
    matrix2 = getMatrix(str, &errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F); /*get the second matrix*/
    if (errorCode != FOUND_MAT) 
        return errorCode;
    
    if (str[0] != '\0') 
        return EXTRANEOUS_TEXT; /*extraneous text after the end of the command*/

    trans_mat(matrix1, matrix2);  /*transpose the matrix*/
    return VALID_SCAN;
}

/*getSomething() functions expect next word to be the something (matrix name, scalar, comma) and return an appropriate error code if it is not*/
mat* getMatrix(char* str, int* errorCode , mat* MAT_A , mat* MAT_B , mat* MAT_C , mat* MAT_D , mat* MAT_E , mat* MAT_F){
    char* parameter;
    mat *ptrMat = NULL;
    parameter = getFirstWord(str); 
    *errorCode = NO_ERROR; /*initialize the error code*/
    
    if (parameter == NULL) { /*if the string is NULL*/
        *errorCode = MALLOC_ERROR; 
        return NULL;
    }
    
    isEndReached(parameter, errorCode);
    if (*errorCode == UNEXPECTED_END_OF_LINE) { /*if the end has been reached*/
        *errorCode = MISSING_MATRIX_NAME; /*missing matrix name*/
        free(parameter);
        return NULL;
    }
    
    isComma(parameter, errorCode); 
    if (*errorCode == FOUND_COMMA) { /*if the string is a comma*/
        free(parameter);
        *errorCode = COMMMA_INSTEAD_OF_MATRIX;
        return NULL;
    }
    
    ptrMat = isMatrix(parameter, errorCode, MAT_A , MAT_B , MAT_C , MAT_D , MAT_E , MAT_F);
    if (*errorCode == FOUND_MAT){ /*if the string is a matrix name*/
        free(parameter);
        return ptrMat;
    }

    free(parameter);
    *errorCode = MATRIX_NAME_INVALID; /*matrix name is invalid*/
    return NULL; 
}

float getScalar(char* str, int* errorCode)
{
    char *parameter;
    float value;
    *errorCode = NO_ERROR; /*initialize the error code*/
    
    parameter = getFirstWord(str);
    if (parameter == NULL) { /*malloc failed*/
        *errorCode = MALLOC_ERROR; 
        return 0.0;
    }

    isEndReached(parameter, errorCode); /*check if the string is empty*/
    if (*errorCode == UNEXPECTED_END_OF_LINE) { /*if the string is empty*/
        *errorCode = MISSING_SCALAR; /*missing scalar*/
        free(parameter);
        return 0.0;
    }
    
    value = isScalar(parameter, errorCode); /*check if the string is a scalar*/
    if (*errorCode == FOUND_SCALAR) { /*if the string is a scalar*/
        free(parameter);
        return value; /*return the value*/
    }

    isComma(parameter, errorCode); /*check if the string is a comma*/
    if (*errorCode == FOUND_COMMA) { /*if the string is a comma*/
        free(parameter);
        *errorCode = MULTIPLE_CONSECUTIVE_COMMAS; 
        return 0.0; /*return trash value*/
    }

    *errorCode = SCALAR_INVALID;
    free(parameter);
    return value; 
}

void getComma(char *str, int* errorCode)
{
    char* parameter;
    parameter = getFirstWord(str);
    *errorCode = NO_ERROR; /*initialize the error code*/

    if (parameter == NULL) { /*malloc failed*/
        *errorCode = MALLOC_ERROR; 
        return;
    }
    
    isEndReached(parameter, errorCode);
    if (*errorCode == UNEXPECTED_END_OF_LINE) { /*if the end has been reached*/
        *errorCode = MISSING_ARGUMENT; /*missing argument*/
        free(parameter);
        return;
    }

    isComma(parameter, errorCode); /*check if the string is a comma*/
    if (*errorCode == FOUND_COMMA) { /*if the string is a comma*/
        free(parameter);
        return;
    }

    isScalar(parameter, errorCode); /*check if the string is a scalar*/
    if (*errorCode == FOUND_SCALAR) { /*if the string is a scalar*/
        *errorCode = MISSING_COMMA; /*missing comma*/
        free(parameter);
        return;
    }

    isMatrix(parameter, errorCode, NULL , NULL , NULL , NULL , NULL , NULL); /*check if the string is a matrix name*/
    if (*errorCode == FOUND_MAT) { /*if the string is a matrix name*/
        *errorCode = MISSING_COMMA; /*missing comma*/
        free(parameter);
        return;
    }

    free(parameter);
    *errorCode = INVALID_COMMA; /*invalid comma*/
}

/* isSomething() functions check if the string is something (matrix name, scalar, comma, end of line)*/
mat* isMatrix(char *parameter, int* errorCode, mat *MAT_A, mat *MAT_B, mat *MAT_C, mat *MAT_D, mat *MAT_E, mat *MAT_F)
{
    if (strcmp(parameter, "MAT_A") == 0) {
        *errorCode = FOUND_MAT;
        return MAT_A;
    } else if (strcmp(parameter, "MAT_B") == 0) {
        *errorCode = FOUND_MAT;
        return MAT_B;
    } else if (strcmp(parameter, "MAT_C") == 0) {
        *errorCode = FOUND_MAT;
        return MAT_C;
    } else if (strcmp(parameter, "MAT_D") == 0) {
        *errorCode = FOUND_MAT;
        return MAT_D;
    } else if (strcmp(parameter, "MAT_E") == 0) {
        *errorCode = FOUND_MAT;
        return MAT_E;
    } else if (strcmp(parameter, "MAT_F") == 0) {
        *errorCode = FOUND_MAT;
        return MAT_F;
    }

    *errorCode = MATRIX_NAME_INVALID; /*matrix name is invalid*/
    return NULL;
}

float isScalar(char *str, int *errorCode)
{
    char* compareP;
    float value;

    value = (float)strtod(str, &compareP);
    
    if (compareP[0] == '\0') {
        *errorCode = FOUND_SCALAR; /*if the string is a scalar*/
        return value;
    }

    *errorCode = SCALAR_INVALID; /*if the string is not a scalar*/
    return 0.0;
}

void isComma(char *parameter, int *errorCode)
{
    if (strcmp(parameter, ",") == 0){
        *errorCode = FOUND_COMMA;
        return;
    }
    *errorCode = INVALID_COMMA; /*invalid comma*/
}

void isEndReached(char *parameter, int *errorCode)
{
    if (parameter[0] == '\0') /*if the string is empty*/
    *errorCode = UNEXPECTED_END_OF_LINE;
    
}