#include "define.h"

char* base4(int *word)
{
    char *base4 = malloc(BASE4_INSTRUCTION_LENGTH + NULL_TERMINATOR);
    int i = 0, first_digit, second_digit, result;
    char letter;

    for (i = 0; i < BASE2_INSTRUCTION_LENGTH; i = i + 2) { 
        first_digit = word[i];
        second_digit = word[i + 1];
        result = (first_digit * 2) + second_digit; /*convert two base 2 digits to one base 4 digit*/
        switch (result) {
        case a: /*00 in base 2*/
            letter = 'a';
            break;
        case b: /*01 in base 2*/
            letter = 'b';
            break;
        case c: /*10 in base 2*/
            letter = 'c';
            break;
        case d: /*11 in base 2*/
            letter = 'd';
            break;
        default: /*should not reach here*/
            letter = '?';
            break;
        }
        base4[i / 2] = letter; /*every two digits in base 2 become one digit in base 4*/
    }
    base4[BASE4_INSTRUCTION_LENGTH] = '\0'; 
    return base4; /*return the base4 string*/
}

