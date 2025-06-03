#include "global.h"
#include "scan.h"
#include "macroTable.h"
#include "preprocessor.h"
/* .as -> .am -> .ob , .ext , .ent*/
/*page 15 or 31 for algorithm for preprocessing */

int executePreprocessor(char *inputFileName) {
    FILE *asFile, *amFile;
    int errorCode;
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

    /* need to add preprocessing algorithm */

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