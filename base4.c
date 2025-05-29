#include "define.h"

void base4(int *word, char **base4_output)
{
    char *base4 = malloc(BASE4_INSTRUCTION_LENGTH + NULL_TERMINATOR);
    int i = 0, first_digit, second_digit, result;
    char letter;
    for (i = 0; i < BASE2_INSTRUCTION_LENGTH; i = i + 2) {
        first_digit = word[i];
        second_digit = word[i + 1];
        result = (first_digit * 2) + second_digit;
        switch (result) {
        case a:
            letter = 'a';
            break;
        case b:
            letter = 'b';
            break;
        case c:
            letter = 'c';
            break;
        case d:
            letter = 'd';
            break;
        default: /*should not reach here*/
            letter = '?';
            break;
        }
        base4[i / 2] = letter;
    }
    base4[BASE4_INSTRUCTION_LENGTH] = '\0';
    *base4_output = base4;
}

