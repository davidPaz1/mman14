#include <stdio.h>
#include <stdlib.h>

/* size of the square matrix */
#define MAT_SIDE_LEN 4 /*size of the one side of the matrix*/
#define ALL_MAT_LEN 16 /*size of \the entire matrix*/
#define EOF_AND_STOP -3 /*EOF and stop command reached*/
#define STOP_COMMAND_REACHED -2
#define EOF_REACHED -1 /*EOF reached*/
#define VALID_SCAN 0 /*valid scan*/
#define MALLOC_ERROR 1 /*malloc failed*/



/* define the matrix struct which is used to represent a 4x4 matrix of real numbers*/
typedef struct mat
{
    float values[4][4]; /*the square matrix*/
}mat;

/* function prototypes*/

/*scan func*/
int readLine(char** pstr); /*read a line from the input and store it in pstr*/
char* getFirstWord(char* str); /*get the first word from the str and deletes it from it + remove spaces*/
int parseCommand(mat* MAT_A , mat* MAT_B , mat* MAT_C , mat* MAT_D , mat* MAT_E , mat* MAT_F); /*try to run one line of command from stdin*/
void errorMessage(int errorCode); /*print the error message according to the error code*/
int parseReadMat(char* str, mat* MAT_A , mat* MAT_B , mat* MAT_C , mat* MAT_D , mat* MAT_E , mat* MAT_F);  /*parse based on read_mat syntax*/
int parsePrintMat(char* str, mat* MAT_A , mat* MAT_B , mat* MAT_C , mat* MAT_D , mat* MAT_E , mat* MAT_F); /*parse based on print_mat syntax*/
int parseAddMat(char* str, mat* MAT_A , mat* MAT_B , mat* MAT_C , mat* MAT_D , mat* MAT_E , mat* MAT_F);   /*parse based on add_mat syntax*/
int parseSubMat(char* str, mat* MAT_A , mat* MAT_B , mat* MAT_C , mat* MAT_D , mat* MAT_E , mat* MAT_F);   /*parse based on sub_mat syntax*/
int parseMulMat(char* str, mat* MAT_A , mat* MAT_B , mat* MAT_C , mat* MAT_D , mat* MAT_E , mat* MAT_F);   /*parse based on mul_mat syntax*/
int parseMulScalar(char* str, mat* MAT_A , mat* MAT_B , mat* MAT_C , mat* MAT_D , mat* MAT_E , mat* MAT_F);/*parse based on mul_scalar syntax*/
int parseTransMat(char* str, mat* MAT_A , mat* MAT_B , mat* MAT_C , mat* MAT_D , mat* MAT_E , mat* MAT_F); /*parse based on trans_mat syntax*/

/*getSomething() functions expect next word to be the something (matrix name, scalar, comma) and return an appropriate error code if it is not*/
mat* getMatrix(char* parameter, int* errorCode, mat* MAT_A , mat* MAT_B , mat* MAT_C , mat* MAT_D , mat* MAT_E , mat* MAT_F); /*expect str to be a matrix but can handle other cases*/
float getScalar(char* str, int* errorCode); /*expect str to be a scalar but can handle other cases*/
void getComma(char* str, int* errorCode); /*expect str to be a comma but can handle other cases*/
mat* isMatrix(char* parameter, int* errorCode, mat* MAT_A , mat* MAT_B , mat* MAT_C , mat* MAT_D , mat* MAT_E , mat* MAT_F); /*check if the string is a matrix name*/
float isScalar(char* str, int* errorCode); /*check if the string is a scalar*/
void isComma(char* parameter, int* errorCode); /*check if the string is a comma*/
void isEndReached(char* parameter, int* errorCode); /*if the end of line is reached*/


/*mymat func*/
void initalizeMat(mat* matrix);  /*initialize the matrix by filling it with 0s*/
void copyMat(mat *matrix1, mat *matrix2); /*copy a matrix to another matrix*/
/*official comands*/
void read_mat(mat* matrix, float userInput[], int len); /*read the matrix from the file*/
void print_mat(mat matrix); /*print the matrix to the file*/
void add_mat(mat* matrix1, mat* matrix2, mat* matrix3); /*add two matrices*/
void sub_mat(mat* matrix1, mat* matrix2, mat* matrix3); /*subtract two matrices*/
void mul_mat(mat* matrix1, mat* matrix2, mat* matrix3); /*multiply two matrices*/
void mul_scalar(mat* matrix1, float scalar , mat* matrix2); /*multiply a matrix by a scalar*/
void trans_mat(mat* matrix1, mat* matrix2); /*transpose a matrix*/

