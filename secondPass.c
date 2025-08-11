#include "secondPass.h"
#include "writeFiles.h" /* for recordExternReference */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h> /* for isdigit */
#include "util.h"
#include "lexer.h"

static int operandTypeToAddrField(operandType op)
{
    /* mapping according to spec:
       immediate = 0, direct (label) = 1, matrix = 2, register = 3
       Unknown operands map to 0 (safe default) */
    switch (op) {
        case NUMBER_OPERAND: return 0;
        case LABEL_SYNTAX_OPERAND:
        case LABEL_TABLE_OPERAND: return 1;
        case MATRIX_SYNTAX_OPERAND:
        case MATRIX_TABLE_OPERAND: return 2;
        case REGISTER_OPERAND: return 3;
        default: return 0;
    }
}

/* encode a matrix operand: we write two words:
   1) LabelWord for matrix base address (ARE depends on symbol type)
   2) MatrixRegistersWord containing row register (bits 6-9) and col register (bits 2-5)
   address is the next code address (absolute, e.g. 100..255) to store at; it will be incremented.
*/
ErrCode encodeMatrixOperand(const char *matLabel, const char *rowStr, const char *colStr,
                            CodeWord codeImage[], unsigned int *address,
                            SymbolTable *symbolTable, ErrorList *errorList)
{
    SymbolNode *sym;
    CodeWord lw;
    registersNumber rrow;
    registersNumber rcol;
    CodeWord mrw;

    sym = findSymbol(symbolTable, matLabel);
    if (sym == NULL) {
        addErrorToList(errorList, OPERAND1_LABEL_DOES_NOT_EXIST_E);
        return LEXER_FAILURE_S;
    }

    memset(&lw, 0, sizeof(CodeWord));
    lw.labelWord.labelAddress = (unsigned int)(sym->address & 0xFF);
    if (sym->type == EXTERN_SYMBOL) lw.labelWord.ARE = 1; /* extern */
    else lw.labelWord.ARE = 2; /* relocatable */

    codeImage[*address].allBits = lw.allBits;

    if (sym->type == EXTERN_SYMBOL) {
        recordExternReference(sym->symbolName, *address);
    }
    (*address)++;

    rrow = getRegisterNumber(rowStr);
    rcol = getRegisterNumber(colStr);
    if (rrow == NOT_REG) {
        addErrorToList(errorList, MAT_ROW_NOT_REGISTER_N);
        return LEXER_FAILURE_S;
    }
    if (rcol == NOT_REG) {
        addErrorToList(errorList, MAT_COL_NOT_REGISTER_N);
        return LEXER_FAILURE_S;
    }

    memset(&mrw, 0, sizeof(CodeWord));
    mrw.matrixRegistersWord.ARE = 0; /* absolute */
    mrw.matrixRegistersWord.registerRow = (unsigned int)rrow & 0xF;
    mrw.matrixRegistersWord.registerCol = (unsigned int)rcol & 0xF;
    codeImage[*address].allBits = mrw.allBits;
    (*address)++;

    return SECOND_PASS_SUCCESS_S;
}

/* main second pass routine */
ErrCode executeSecondPass(FILE* amFile, MacroTable* macroTable,
                          SymbolTable* symbolTable, CodeWord codeImage[],
                          DataWord dataImage[], unsigned int *IC,
                          unsigned int *DC, ErrorList* errorList)
{
    ErrCode err;
    unsigned int address;
    parsedLine *pLine;

    err = NULL_INITIAL;
    address = 100; /* code begins at memory address 100 decimal */
    errorList->currentLine = 0;

    rewind(amFile);

    while (1) {
        if (errorList->fatalError)
            return SECOND_PASS_FAILURE_S;

        errorList->currentLine++;

        pLine = readParsedLine(amFile, &err, macroTable, errorList);
        if (err == EOF_REACHED_S) {
            break;
        }
        if (err == LEXER_FAILURE_S) {
            continue;
        }

        if (pLine->typesOfLine == EMPTY_LINE || pLine->typesOfLine == COMMENT_LINE) {
            freeParsedLine(pLine);
            continue;
        }

        if (pLine->typesOfLine == DIRECTIVE_LINE) {
            if (strcmp(pLine->lineContentUnion.directive.directiveName, ".entry") == 0) {
                char *entryLabel;
                SymbolNode *sym;
                entryLabel = pLine->lineContentUnion.directive.directiveLabel;
                sym = findSymbol(symbolTable, entryLabel);
                if (sym == NULL) {
                    addErrorToList(errorList, ENTRY_LABEL_DOES_NOT_EXIST_E);
                } else {
                    sym->isEntry = TRUE;
                    symbolTable->haveEntry = TRUE;
                }
            }
            freeParsedLine(pLine);
            continue;
        }

        if (pLine->typesOfLine == INSTRUCTION_LINE) {
            CodeWord first;
            operandType op1, op2;
            int srcAddr, dstAddr;
            OpCodeNumber op;

            memset(&first, 0, sizeof(CodeWord));

            op = getOpCodeNumber(pLine->lineContentUnion.instruction.operationName);
            if (op == invalid) {
                addErrorToList(errorList, INVALID_DIRECTIVE_E);
                freeParsedLine(pLine);
                continue;
            }

            first.firstWord.opCode = (unsigned int)op & 0xF;

            op1 = pLine->lineContentUnion.instruction.operand1Type;
            op2 = pLine->lineContentUnion.instruction.operand2Type;

            srcAddr = operandTypeToAddrField(op1);
            dstAddr = operandTypeToAddrField(op2);

            first.firstWord.srcAddress = (unsigned int)(srcAddr & 0x3);
            first.firstWord.destAddress = (unsigned int)(dstAddr & 0x3);
            first.firstWord.ARE = 0; /* absolute */
            codeImage[address].allBits = first.allBits;
            address++;

            /* Encode operands extras */

            /* Operand 1 */
            if (op1 == NUMBER_OPERAND) {
                CodeWord nw;
                ErrCode e;
                int val;

                memset(&nw, 0, sizeof(CodeWord));
                e = isNumberOperand(pLine->lineContentUnion.instruction.operand1);
                if (e != UTIL_SUCCESS_S) {
                    addErrorToList(errorList, NUMBER_OPERAND_IS_NOT_INTEGER_E);
                }
                val = atoi(pLine->lineContentUnion.instruction.operand1 + 1);
                nw.numberWord.numberVal = val & 0xFF;
                nw.numberWord.ARE = 0;
                codeImage[address].allBits = nw.allBits;
                address++;
            } else if (op1 == REGISTER_OPERAND) {
                if (op2 == REGISTER_OPERAND) {
                    /* postpone */
                } else {
                    CodeWord rw;
                    registersNumber rsrc;

                    memset(&rw, 0, sizeof(CodeWord));
                    rw.registerWord.ARE = 0;
                    rsrc = getRegisterNumber(pLine->lineContentUnion.instruction.operand1);
                    rw.registerWord.registerSrc = (unsigned int)rsrc & 0xF;
                    rw.registerWord.registerDest = 0;
                    codeImage[address].allBits = rw.allBits;
                    address++;
                }
            } else if (op1 == MATRIX_TABLE_OPERAND || op1 == MATRIX_SYNTAX_OPERAND) {
                encodeMatrixOperand(pLine->lineContentUnion.instruction.operand1,
                                   pLine->lineContentUnion.instruction.row1,
                                   pLine->lineContentUnion.instruction.col1,
                                   codeImage, &address, symbolTable, errorList);
            } else if (op1 == LABEL_TABLE_OPERAND || op1 == LABEL_SYNTAX_OPERAND) {
                SymbolNode *sym;
                CodeWord lw;
                char *label;

                memset(&lw, 0, sizeof(CodeWord));
                label = pLine->lineContentUnion.instruction.operand1;
                sym = findSymbol(symbolTable, label);
                if (sym == NULL) {
                    addErrorToList(errorList, OPERAND1_LABEL_DOES_NOT_EXIST_E);
                    lw.labelWord.labelAddress = 0;
                    lw.labelWord.ARE = 2;
                } else {
                    lw.labelWord.labelAddress = (unsigned int)(sym->address & 0xFF);
                    if (sym->type == EXTERN_SYMBOL) {
                        lw.labelWord.ARE = 1;
                        recordExternReference(sym->symbolName, address);
                    } else {
                        lw.labelWord.ARE = 2;
                    }
                }
                codeImage[address].allBits = lw.allBits;
                address++;
            }

            /* Operand 2 */
            if (op2 == NUMBER_OPERAND) {
                CodeWord nw;
                ErrCode e;
                int val;

                memset(&nw, 0, sizeof(CodeWord));
                e = isNumberOperand(pLine->lineContentUnion.instruction.operand2);
                if (e != UTIL_SUCCESS_S) {
                    addErrorToList(errorList, NUMBER_OPERAND_IS_NOT_INTEGER_E);
                }
                val = atoi(pLine->lineContentUnion.instruction.operand2 + 1);
                nw.numberWord.numberVal = val & 0xFF;
                nw.numberWord.ARE = 0;
                codeImage[address].allBits = nw.allBits;
                address++;
            } else if (op2 == REGISTER_OPERAND) {
                if (op1 == REGISTER_OPERAND) {
                    CodeWord rr;
                    registersNumber rsrc, rdst;

                    memset(&rr, 0, sizeof(CodeWord));
                    rr.registerWord.ARE = 0;
                    rsrc = getRegisterNumber(pLine->lineContentUnion.instruction.operand1);
                    rdst = getRegisterNumber(pLine->lineContentUnion.instruction.operand2);
                    rr.registerWord.registerSrc = (unsigned int)rsrc & 0xF;
                    rr.registerWord.registerDest = (unsigned int)rdst & 0xF;
                    codeImage[address].allBits = rr.allBits;
                    address++;
                } else {
                    CodeWord rw;
                    registersNumber rdst;

                    memset(&rw, 0, sizeof(CodeWord));
                    rw.registerWord.ARE = 0;
                    rdst = getRegisterNumber(pLine->lineContentUnion.instruction.operand2);
                    rw.registerWord.registerSrc = 0;
                    rw.registerWord.registerDest = (unsigned int)rdst & 0xF;
                    codeImage[address].allBits = rw.allBits;
                    address++;
                }
            } else if (op2 == MATRIX_TABLE_OPERAND || op2 == MATRIX_SYNTAX_OPERAND) {
                encodeMatrixOperand(pLine->lineContentUnion.instruction.operand2,
                                   pLine->lineContentUnion.instruction.row2,
                                   pLine->lineContentUnion.instruction.col2,
                                   codeImage, &address, symbolTable, errorList);
            } else if (op2 == LABEL_TABLE_OPERAND || op2 == LABEL_SYNTAX_OPERAND) {
                SymbolNode *sym;
                CodeWord lw;
                char *label;

                memset(&lw, 0, sizeof(CodeWord));
                label = pLine->lineContentUnion.instruction.operand2;
                sym = findSymbol(symbolTable, label);
                if (sym == NULL) {
                    addErrorToList(errorList, OPERAND2_LABEL_DOES_NOT_EXIST_E);
                    lw.labelWord.labelAddress = 0;
                    lw.labelWord.ARE = 2;
                } else {
                    lw.labelWord.labelAddress = (unsigned int)(sym->address & 0xFF);
                    if (sym->type == EXTERN_SYMBOL) {
                        lw.labelWord.ARE = 1;
                        recordExternReference(sym->symbolName, address);
                    } else {
                        lw.labelWord.ARE = 2;
                    }
                }
                codeImage[address].allBits = lw.allBits;
                address++;
            }

            freeParsedLine(pLine);
            continue;
        }

        freeParsedLine(pLine);
    }

    if (errorList->count > 0)
        return SECOND_PASS_FAILURE_S;

    /* no change to *IC or *DC here */

    return SECOND_PASS_SUCCESS_S;
}
