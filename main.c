#include "global.h"
#include "macroTable.h"

/* test main function for macroTable */
int main(int argc, char const *argv[])
{
    macroTable* test = NULL;
    macroTable* t1 = malloc(sizeof(macroTable));
    MacroBody* line1 = NULL , *line2 = NULL;
    t1->macroName = strDup("m1");
    if (t1->macroName == NULL) {
        fprintf(stderr, "Memory allocation failed for macro name\n");
        free(t1);
        return 1; /* Exit if memory allocation fails */
    }
    t1->nextMacro = NULL;

    line1 = malloc(sizeof(MacroBody));
    line1->line = strDup("example line");
    if (line1->line == NULL) {
        fprintf(stderr, "Memory allocation failed for macro line\n");
        free(t1->macroName);
        free(t1);
        return 1; /* Exit if memory allocation fails */
    }
    line1->nextLine = NULL;

    line2 = malloc(sizeof(MacroBody));
    line2->line = strDup("example line 2");
    if (line2->line == NULL) {
        fprintf(stderr, "Memory allocation failed for macro line\n");
        free(t1->macroName);
        free(t1);
        return 1; /* Exit if memory allocation fails */
    }
    line2->nextLine = NULL;

    line1->nextLine = line2;

    t1->body = line1;

    test = t1;
    while (test != NULL) {
        MacroBody* b = test->body;
        printf("macro Name: %s\n", test->macroName);
        while (b != NULL) {
            printf("\tLine: %s\n", b->line);
            b = b->nextLine;
        }

        test = test->nextMacro;
    }
    free(line2->line);
    free(line2);
    free(line1->line);
    free(line1);
    free(t1->macroName);
    free(t1);
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
