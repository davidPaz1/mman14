#include "global.h"
#define a 0
#define b 1
#define c 2
#define d 3

char* base4(const int *word);

char* base4(const int *word)
{
    char *base4 = malloc(BASE4_INSTRUCTION_LENGTH + NULL_TERMINATOR);
    int i = 0, first_digit, second_digit, result;
    char letter;

    for (i = 0; i < BASE2_INSTRUCTION_LENGTH; i = i + 2) { 
        first_digit = word[i];
        second_digit = word[i + 1];
        result = (first_digit * 2) + second_digit; /* convert two base 2 digits to one base 4 digit */
        switch (result) {
        case a: /* 00 in base 2 */
            letter = 'a';
            break;
        case b: /* 01 in base 2 */
            letter = 'b';
            break;
        case c: /* 10 in base 2 */
            letter = 'c';
            break;
        case d: /* 11 in base 2 */
            letter = 'd';
            break;
        default: /* should not reach here */
            letter = '?';
            break;
        }
        base4[i / 2] = letter; /* every two digits in base 2 become one digit in base 4 */
    }
    base4[BASE4_INSTRUCTION_LENGTH] = '\0';
    return base4; /* return the base4 string */
}

void printBin(CodeWord word) {
    int i;
    int h, l;
    for (i = 9; i >= 0; i = i - 2) {

        h = (word.allBits >> i) & 1;
        l = (word.allBits >> (i - 1)) & 1;
        printf("%d", h * 2 + l);
    }
    printf("\n");
}

void printBin(DataWord word) {
    int i;
    int h, l;
    for (i = 9; i >= 0; i = i - 2) {

        h = (word.value >> i) & 1;
        l = (word.value >> (i - 1)) & 1;
        printf("%d", h * 2 + l);
    }
    printf("\n");
}

/* test for github */