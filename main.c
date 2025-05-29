#include "global.h"

int main(int argc, char *argv[]){
    short int input[BASE2_INSTRUCTION_LENGTH] = {0,0,0,1,1,0,1,1,2,9};
    char *base4_output;
    base4_output = base4(input);

    printf("Base 4 output: %s\n", base4_output);
    free(base4_output);
    return 0;
}
