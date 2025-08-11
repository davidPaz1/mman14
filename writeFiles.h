#ifndef WRITE_FILES_H
#define WRITE_FILES_H

#include "global.h"
#include "tables.h"
#include "error.h"

/* record extern reference for .ext file */
void recordExternReference(const char *symName, unsigned int address);
void clearExternRecords(void);

/* write output files */
ErrCode writeObjectFile(FILE *fp, CodeWord codeImage[], unsigned int codeSize,
                        DataWord dataImage[], unsigned int dataSize);

ErrCode writeEntryFile(const char *filename, SymbolTable *symbolTable,
                       ErrorList *errorList);

ErrCode writeExternFile(const char *filename, SymbolTable *symbolTable,
                        ErrorList *errorList);

#endif
