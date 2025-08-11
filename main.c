#include "global.h"
#include "preprocessor.h"
#include "firstPass.h"
#include "secondPass.h"
#include "writeFiles.h"
#include "error.h"
#include "lexer.h"
#include "tables.h"
#include "util.h"

void executeAssembler(char* fileName);

int main(int argc, char const *argv[])
{
    char* inputFileName = NULL;
    int i;
    
    if (argc < 2) {
        printf("No input files provided. will run with default file name 'test1.as'\n\n");
        inputFileName = "test1";
        executeAssembler(inputFileName);
        printf("\ndone with file %s \n\n\n", inputFileName);
        return 0;
    }

    for (i = 1; i < argc; i++) {
        inputFileName = strDup((char*)argv[i]);
        printf("argv[%d]: %s\n", i, argv[i]);
        executeAssembler(inputFileName);
        printf("\ndone with file %s \n\n\n", inputFileName);
        free(inputFileName);
    }

    return 0;
}

void executeAssembler(char* fileName)
{
    ErrCode errCode = NULL_INITIAL;
    FILE *asFile = NULL, *amFile = NULL, *obFile = NULL;
    unsigned int DCF = 0, ICF = 0;
    CodeWord codeImage[MAX_MEMORY_SIZE] = {0};
    DataWord dataImage[MAX_MEMORY_SIZE] = {0};
    MacroTable* macroTable = createMacroTable();
    SymbolTable* symbolTable  = createSymbolTable();
    ErrorList* errorList = createErrorList(fileName);

    if (errorList == NULL || macroTable == NULL || symbolTable == NULL) {
        printErrorMsg(MALLOC_ERROR_F, "tables", 0);
        return;
    }

    printf("Starting preprocessor...\n");
    errorList->stage = "preprocessor";

    asFile = openFile(fileName, ".as", "r", &errCode);
    if(errCode == FILE_READ_ERROR_F) 
        errCode = INPUT_FILE_UNREADABLE_F;

    if (errCode != UTIL_SUCCESS_S){
        printf("Error opening file %s.as: %s\n", fileName, getErrorMessage(errCode));
        freeTableAndLists(macroTable, symbolTable , errorList); 
        return;
    }

    amFile = openFile(fileName, ".am", "w", &errCode);
    if (errCode != UTIL_SUCCESS_S) {
        printf("Error opening file %s.am: %s\n", fileName, getErrorMessage(errCode));
        freeTableAndLists(macroTable, symbolTable , errorList); 
        freeFiles(asFile, NULL, NULL);
        return;
    }

    errCode = executePreprocessor(asFile, amFile, macroTable, errorList);
    if (errCode == PREPROCESSOR_FAILURE_S) {
        freeFiles(asFile, amFile, NULL);
        errCode = delFile(errorList->filename, ".am"); 
        if (errCode != UTIL_SUCCESS_S)
            addErrorToList(errorList, errCode);
        printErrors(errorList);
        freeTableAndLists(macroTable, symbolTable , errorList);
        return;
    }
    printf("\nPreprocessor executed successfully.\n");
    freeFiles(asFile, amFile, NULL);

    printf("Starting first pass...\n");
    errorList->stage = "first pass";

    amFile = openFile(fileName, ".am", "r", &errCode);
    if (errCode != UTIL_SUCCESS_S) {
        printf("Error opening file %s.am for reading: %s\n", fileName, getErrorMessage(errCode));
        freeTableAndLists(macroTable, symbolTable , errorList);
        return;
    }

    errCode = executeFirstPass(amFile, dataImage, &DCF, &ICF, macroTable, symbolTable , errorList);
    if (errCode == FIRSTPASS_FAILURE_S) {
        printErrors(errorList);
        freeTableAndLists(macroTable, symbolTable , errorList);
        freeFiles(amFile, NULL, NULL);
        return;
    }
    printf("\nFirst pass executed successfully.\n");

    printf("Starting second pass...\n");
    errorList->stage = "second pass";
    rewind(amFile);

    errCode = executeSecondPass(amFile, macroTable, symbolTable, codeImage, dataImage, &ICF, &DCF, errorList);
    if (errCode == SECOND_PASS_FAILURE_S) {
        freeFiles(amFile, NULL, NULL);
        printSymbolTableSorted(symbolTable);
        printErrors(errorList);
        freeTableAndLists(macroTable, symbolTable , errorList);
        return;
    }

    if (ICF + DCF > 164){
        printf("Warning: ICF + DCF exceeds 164\n");
    }

    printf("Writing output files...\n");
    errorList->stage = "write files";

    obFile = openFile(fileName, ".ob", "w", &errCode);
    if (errCode != UTIL_SUCCESS_S) {
        printf("Error opening file %s.ob: %s\n", fileName, getErrorMessage(errCode));
        freeTableAndLists(macroTable, symbolTable , errorList);
        freeFiles(amFile, NULL, NULL);
        return;
    }

    /* Write .ob file */
    writeObjectFile(obFile, codeImage, ICF, dataImage, DCF);

    freeFiles(obFile, NULL, NULL);

    /* Write .ent file by filename if there are entry symbols */
    if (symbolTable->haveEntry) {
        errCode = writeEntryFile(fileName, symbolTable, errorList);
        if (errCode != UTIL_SUCCESS_S) {
            printf("Error writing .ent file: %s\n", getErrorMessage(errCode));
        }
    }

    /* Write .ext file by filename if there are extern symbols */
    if (symbolTable->haveExtern) {
        errCode = writeExternFile(fileName, symbolTable, errorList);
        if (errCode != UTIL_SUCCESS_S) {
            printf("Error writing .ext file: %s\n", getErrorMessage(errCode));
        }
    }

    freeFiles(amFile, NULL, NULL);
    freeTableAndLists(macroTable, symbolTable , errorList);

    printf("Successfully executed file %s\n", fileName);
}
