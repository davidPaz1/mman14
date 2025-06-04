#include "global.h"
#include "scan.h"
#include "macroTable.h"
#include "preprocessor.h"
/* .as -> .am -> .ob , .ext , .ent*/
/*page 15 or 31 for algorithm for preprocessing */

int executePreprocessor(char *inputFileName) {
    FILE* asFile, *amFile;
    char* line;
    int errorCode = NULL_INITIAL; /* Initialize error code */
    macroTable* table = NULL;
    MacroBody* body = NULL;
    Bool isMacroDefLine = FALSE; /* flag to indicate if the current line is a macro definition line */

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
    while (errorCode != EOF_REACHED) {
        line = readLine(asFile, &errorCode); /* read a line from the .as file */
        
        if (line == NULL) { /* check if the line is NULL */
            if (errorCode == EOF_REACHED) {
                break; /* end of file reached, exit the loop */
            } else {
                fprintf(stderr, "Error reading line from .as file: %s\n", inputFileName);
                freeFilesAndMemory(table, body, asFile, amFile, line);
                return errorCode; /* return the error code */
            }
        }

        fputs(line, amFile); /* write the line to the .am file */
        fputc('\n', amFile); /* add a newline character after the line */
        free(line); /* free the line memory */
    }
    freeFilesAndMemory(table, body, asFile, amFile, line); /* free all allocated memory and close files */
    return SUCCESS; /* return success */
}

Bool isMacroDef(char *line) {
    /* check if the line is a macro definition line */
    return TRUE; 
}

Bool isMacroEndLine(char *line)
{
    /* check if the line indicates the end of a macro definition */
    return TRUE;
}

Bool isMacroUse(char *line, MacroBody **body)
{
    /* check if the line is a macro use line */
    return TRUE;
}

void freeFilesAndMemory(macroTable* table, MacroBody* body, FILE* asFile, FILE* amFile, char* line)
{
    if (body != NULL) {
        freeMacroBody(body); /* free the macro body if it was created */
    }
    if (table != NULL) {
        freeMacroTable(table); /* free the macro table if it was created */
    }
    if (line != NULL) {
        free(line);
    }
    if (asFile != NULL)
        fclose(asFile); /* close the .as file */
    if (amFile != NULL)
        fclose(amFile); /* close the .am file */
}
