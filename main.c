#include "define.h"

int main(int argc, char *argv[]){
    int input[] = {0,0,0,1,1,0,1,1,2,9};
    char *base4_output;
    base4(input, &base4_output);

    printf("Base 4 output: %s\n", base4_output);

    free(base4_output);
    return 0;
}
