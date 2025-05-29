#include "mymat.h"

void initalizeMat(mat *matrix)
{
    int i, j;
    for (i = 0; i < MAT_SIDE_LEN; i++)
    {
        for (j = 0; j < MAT_SIDE_LEN; j++)
        {
            matrix->values[i][j] = 0;
        }
    }
}

void copyMat(mat *matrix1, mat *matrix2)
{
    int i, j;
    for (i = 0; i < MAT_SIDE_LEN; i++)
    {
        for (j = 0; j < MAT_SIDE_LEN; j++)
        {
            matrix2->values[i][j] = matrix1->values[i][j];
        }
    }
}

void read_mat(mat *matrix, float userInput[], int len)
{
    int i, j, count = 0;

    for (i = 0; i < MAT_SIDE_LEN; i++) {
        for (j = 0; j < MAT_SIDE_LEN && count < len; j++) {
            matrix->values[i][j] = userInput[count];
            count++;
        }
    }
}

void print_mat(mat matrix)
{
    int i, j;
    for (i = 0; i < MAT_SIDE_LEN; i++)
    {
        for (j = 0; j < MAT_SIDE_LEN; j++)
        {
            printf("%g\t\t", matrix.values[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void add_mat(mat *matrix1, mat *matrix2, mat *matrix3)
{
    int i, j;
    for (i = 0; i < MAT_SIDE_LEN; i++)
    {
        for (j = 0; j < MAT_SIDE_LEN; j++)
        {
            matrix3->values[i][j] = matrix1->values[i][j] + matrix2->values[i][j];
        }
    }
}

void sub_mat(mat *matrix1, mat *matrix2, mat *matrix3)
{
    int i, j;
    for (i = 0; i < MAT_SIDE_LEN; i++)
    {
        for (j = 0; j < MAT_SIDE_LEN; j++)
        {
            matrix3->values[i][j] = matrix1->values[i][j] - matrix2->values[i][j];
        }
    }
}

void mul_mat(mat *matrix1, mat *matrix2, mat *matrix3)
{
    int i, j, k;
    mat temp; 
    initalizeMat(&temp);
    for (i = 0; i < MAT_SIDE_LEN; i++)
    {
        for (j = 0; j < MAT_SIDE_LEN; j++)
        {
            for (k = 0; k < MAT_SIDE_LEN; k++)
            {
                temp.values[i][j] += matrix1->values[i][k] * matrix2->values[k][j];
            }
        }
    }
    copyMat(&temp, matrix3); /*copy the result to the third matrix*/
}

void mul_scalar(mat *matrix1, float scalar, mat *matrix2)
{
    int i, j;
    for (i = 0; i < MAT_SIDE_LEN; i++)
    {
        for (j = 0; j < MAT_SIDE_LEN; j++)
        {
            matrix2->values[i][j] = matrix1->values[i][j] * scalar;
        }
    }
}

void trans_mat(mat *matrix1, mat *matrix2)
{
    int i, j;
    mat temp;
    initalizeMat(&temp);
    for (i = 0; i < MAT_SIDE_LEN; i++)
    {
        for (j = 0; j < MAT_SIDE_LEN; j++)
        {
            temp.values[j][i] = matrix1->values[i][j];
        }
    }
    *matrix2 = temp;
}