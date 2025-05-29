#include <stdio.h>

void printBin(int num);
int bits_ones_count(int x);
int bits_zero_count(int x);

int main()
{
    int user_Input;
    printf("insert an unsigned int and i will tell you how many 0 does the binary representation have\n");
    scanf("%d" , &user_Input);
    printf("the binary representation of the number %d is: ", user_Input);
    printBin(user_Input);
    printf("the binary representation of the number %d have %d zeros in it\n" ,user_Input, bits_zero_count(user_Input));
    return 0;
}

void printBin(int num)
{
    int bits[ sizeof(num) * 8 ]; /*sizeof(num) * 8 is number of bits in an integer in this machine*/
    int size = sizeof(num) * 8; /*recorde the number of bits in an integer in this machine*/
    int i ,j ,k;
    for (i = 0; i < size; i++) /*fill arr with 0*/
        bits[i] = 0;
    
    /*now the bits represent the number 0*/
    
    for (j = 0; num != 0; num >>= 1, j++) {
        bits[j] = num & 1;  /* insert first bit to bits[j] */
    }

    for (k = size - 1; k != -1; k--) /*going through the arr from thee end to the start*/
    {
        printf("%d", bits[k]);
        if (k%4 == 0 && k != 0) /*every 4 bits print ',' for better reading*/
        {
            printf(",");
        }
        
    }
    printf("\n");
}

int bits_ones_count(int x)
{
    int counter = 0;
    for (; x != 0; x >>= 1){
        if (x & 1) /*if first bit is 1*/
            counter++; /*counter + 1*/
    }
    return counter;
}

int bits_zero_count(int x)
{
    /* sizeof(x) * 8 = amount of bits in x
     x bits = all 1 bits + all 0 bits 
     x bits - all 1 bits = all 0 bits */
    return (sizeof(x) * 8) - bits_ones_count(x);
}
