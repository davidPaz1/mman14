#include "global.h"
#include "scan.h"
#include "macroTable.h"
#include "preprocessor.h"
/* .as -> .am -> .ob , .ext , .ent*/
/*page 15 or 31 for algorithm for preprocessing */

int executePreprocessor(char *inputFileName) {
    FILE* asFile, *amFile;
    char* line;
    int errorCode;
    macroTable* table = NULL;
    MacroBody* body = NULL;

    /*
    macroTable *macroTable = NULL;
    MacroBody *macroBody = NULL;
    */
    /* Open the .as file for reading and .am file for writing */
    asFile = openFile(inputFileName, ".as", "r", &errorCode);
    if (asFile == NULL) {
        fprintf(stderr, "Error opening .as file: %s\n", inputFileName);
        return FILE_READ_ERROR;
    }

    amFile = openFile(inputFileName, ".am", "w", &errorCode);
    if (amFile == NULL) {
        fprintf(stderr, "Error opening .am file: %s\n", inputFileName);
        fclose(asFile);
        return FILE_WRITE_ERROR;
    }

    /* Initialize the macro table */
    table = createMacroTable();
    int i = 0;
    for (i = 0; i < 11; i++)
    {
        printf("\n");
        line = readLine(asFile, &errorCode);
        switch (errorCode) {
            case SUCCESS:
                printf("Line read successfully\n");
                break;
            case EOF_REACHED:
                printf("End of file reached\n");
                break;
            case MALLOC_ERROR:
                printf("Memory allocation error\n");
                break;
            case FILE_READ_ERROR:
                printf("File read error\n");
                break;
            case FILE_WRITE_ERROR:
                printf("File write error\n");
                break;
            case LINE_TOO_LONG:
                printf("Line too long\n");
                break;
            default:
                printf("Unknown error\n");
                break;
        }
        printf("Line: %s\n", line);
    }

    /* need to add preprocessing algorithm */

    free(line); /* free the line after reading */
    freeMacroTable(table); /* free the macro table */
    if (body != NULL) {
        freeMacroBody(body); /* free the macro body if it was created */
    }
    fclose(asFile);
    fclose(amFile);
    return 0;
}

Bool isMacroLine(char *line)
{

    return TRUE;
}

Bool isMacroEndLine(char *line)
{

    return FALSE;
}