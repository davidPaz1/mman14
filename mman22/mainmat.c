#include "mymat.h"

int main()
{
    int parseCommandReturn = 0; 
    mat MAT_A, MAT_B, MAT_C, MAT_D, MAT_E, MAT_F; /*the matrices*/
    initalizeMat(&MAT_A); /*initialize the matrix*/
    initalizeMat(&MAT_B); /*initialize the matrix*/
    initalizeMat(&MAT_C); /*initialize the matrix*/
    initalizeMat(&MAT_D); /*initialize the matrix*/
    initalizeMat(&MAT_E); /*initialize the matrix*/
    initalizeMat(&MAT_F); /*initialize the matrix*/

    printf("please enter some commands:\n");
    printf("to stop the program please enter the stop command\n\n");

    while (parseCommandReturn == VALID_SCAN) { /*read until EOF is reached*/
        printf("\nenter another command:\n");
        parseCommandReturn = parseCommand(&MAT_A, &MAT_B, &MAT_C, &MAT_D, &MAT_E, &MAT_F); /*use the command*/
        if (parseCommandReturn == MALLOC_ERROR) { 
            printf("reading line failed due to malloc failure please run the program again\n");
            return MALLOC_ERROR;
        }
    }
    
    printf("\n");
    switch (parseCommandReturn) { 
        case EOF_REACHED: /*if EOF is reached*/
            printf("next time enter the \"stop\" command at the end\n");
            break;
        case STOP_COMMAND_REACHED: /*if stop command reached*/
            printf("next time dont enter anything after the \"stop\" command\n");
            break;
        case EOF_AND_STOP: /*if EOF and stop command reached*/
            printf("ended the program correctly, goodbye\n");
            break;
        default:
            printf("unknown error\n"); /* should not happen*/
            break;
    }
    return 0;
}