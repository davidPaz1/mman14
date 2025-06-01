#include "global.h"
#include <string.h>

/* test main function for macroTable */
int main(int argc, char *argv[]){

    macroTable* t1 = malloc(sizeof(macroTable)) , *test;
    t1->macroName = strdup("m1");
    t1->body.line = strdup("example line");
    t1->nextMacro = NULL;
    t1->body.nextLine->line = strdup("example line 2");
    t1->body.nextLine->nextLine = NULL;

    test = t1;
    while (test != NULL) {
        printf("Macro Name: %s\n", test->macroName);
        while (test->body.nextLine != NULL)
        {
            printf("\t Line: %s\n", test->body.line);
            test = test->body.nextLine;
        }

        test = test->nextMacro;
    } 

    return 0;
}
/**/

/* test main function for base4 conversion
int main(int argc, char *argv[]){
    int input[BASE2_INSTRUCTION_LENGTH] = {0,0,0,1,1,0,1,1,2,9};
    char *base4_output;
    base4_output = base4(input);

    printf("Base 4 output: %s\n", base4_output);
    free(base4_output);
    return 0;
}
*/
