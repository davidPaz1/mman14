#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include <stdio.h>
#include "global.h"   
#include "lexer.h"      

#define SECOND_PASS_SUCCESS_S 0
#define SECOND_PASS_FAILURE_S 1


ErrCode executeSecondPass(FILE* amFile, MacroTable* macroTable,
                          SymbolTable* symbolTable, CodeWord codeImage[],
                          DataWord dataImage[], unsigned int *IC,
                          unsigned int *DC, ErrorList* errorList);

#endif /* SECOND_PASS_H */
