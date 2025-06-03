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
    line = readLine(asFile, &errorCode);
    printf("errorCode: %d\n", errorCode);
    printf("Reading lines from .as file...\n");
    printf("Line: %s\n", line);

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