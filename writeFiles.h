#ifndef WRITE_FILES_H
#define WRITE_FILES_H
#include "global.h"
#include "error.h"
#include "tables.h"

typedef enum Base4 {
    a = 0, /* 00 in base 2 */
    b = 1, /* 01 in base 2 */
    c = 2, /* 10 in base 2 */
    d = 3 /* 11 in base 2 */
} Base4;

char* base4(const int *word); /* convert a word from base 2 to base 4 */
void printBinCodeWord(CodeWord word);
void printBinDataWord(DataWord word);


void writeObjectFile(FILE* obFile, DataWord dataImage[], unsigned int DC, CodeWord codeImage[], unsigned int IC, SymbolTable* symbolTable);
void writeEntryFile(FILE* entFile, SymbolTable* symbolTable);
void writeExternFile(FILE* extFile, SymbolTable* symbolTable);

#endif