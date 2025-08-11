#include "writeFiles.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h" /* for strDup */

/* local structure for holding extern references recorded during second pass */
typedef struct ExternRec {
    char *name;
    unsigned int address; /* decimal address */
    struct ExternRec *next;
} ExternRec;

static ExternRec *externHead = NULL;

/* helper: add extern rec */
void recordExternReference(const char *symName, unsigned int address)
{
    ExternRec *node = (ExternRec*)malloc(sizeof(ExternRec));
    if (!node) return;
    node->name = strDup(symName);
    if (!node->name) { free(node); return; }
    node->address = address;
    node->next = externHead;
    externHead = node;
}

/* clear list (call after finishing file) */
void clearExternRecords(void)
{
    ExternRec *cur = externHead;
    while (cur) {
        ExternRec *n = cur->next;
        free(cur->name);
        free(cur);
        cur = n;
    }
    externHead = NULL;
}

/* convert unsigned 10-bit value to base-4 unique string of length digits (5 for full word, 4 for address) */
static void to_base4_unique(unsigned int value, int digits, char *out)
{
    /* digits between 1 and 10 (we only use 4 or 5). map 0->a,1->b,2->c,3->d */
    char buf[16];
    unsigned int tmp = value;
    int i;

    /* fill with 'a' to be safe */
    for (i = 0; i < digits; ++i) buf[i] = 'a';
    buf[digits] = '\0';

    for (i = digits - 1; i >= 0; --i) {
        unsigned int d = tmp % 4u;
        tmp /= 4u;
        buf[i] = (d == 0 ? 'a' : d == 1 ? 'b' : d == 2 ? 'c' : 'd');
    }
    strcpy(out, buf);
}

/* convert signed 10-bit value to 10-bit unsigned representation (two's complement) */
static unsigned int to_unsigned_10bit(int val)
{
    unsigned int mask = 0x3FFu; /* 10 bits */
    return ((unsigned int)val) & mask;
}

ErrCode writeObjectFile(FILE *fp, CodeWord codeImage[], unsigned int codeSize, DataWord dataImage[], unsigned int dataSize)
{
    if (!fp) return FILE_WRITE_ERROR_F;

    /* header: two numbers: codeSize and dataSize, encoded in base-4 unique with 4 digits each */
    char header1[8], header2[8];
    to_base4_unique(codeSize, 4, header1);
    to_base4_unique(dataSize, 4, header2);
    fprintf(fp, "%s %s\n", header1, header2);

    /* write code segment: addresses from 100 to 100 + codeSize - 1 */
    {
        unsigned int i;
        for (i = 0; i < codeSize; ++i) {
            unsigned int addr = 100u + i;
            char addrStr[8], codeStr[8];
            to_base4_unique(addr, 4, addrStr);
            /* code word is 10 bits; use the i index into codeImage */
            unsigned int word = codeImage[i].allBits & 0x3FFu;
            to_base4_unique(word, 5, codeStr);
            fprintf(fp, "%s %s\n", addrStr, codeStr);
        }
    }

    /* write data segment: addresses from 100 + codeSize to 100 + codeSize + dataSize - 1 */
    {
        unsigned int i;
        for (i = 0; i < dataSize; ++i) {
            unsigned int addr = 100u + codeSize + i;
            char addrStr[8], dataStr[8];
            to_base4_unique(addr, 4, addrStr);
            /* dataImage[i].value is signed 10-bit; convert to unsigned 10-bit pattern */
            unsigned int word = to_unsigned_10bit(dataImage[i].value);
            to_base4_unique(word, 5, dataStr);
            fprintf(fp, "%s %s\n", addrStr, dataStr);
        }
    }

    return UTIL_SUCCESS_S;
}

ErrCode writeEntryFile(const char *filename, SymbolTable *symbolTable, ErrorList *errorList)
{
    if (!symbolTable || !filename) return FILE_WRITE_ERROR_F;

    /* create filename with .ent */
    char *full = (char*)malloc(strlen(filename) + 5);
    if (!full) return MALLOC_ERROR_F;
    strcpy(full, filename);
    strcat(full, ".ent");

    FILE *fp = fopen(full, "w");
    if (!fp) {
        free(full);
        return FILE_WRITE_ERROR_F;
    }

    SymbolNode *cur = symbolTable->head;
    while (cur) {
        if (cur->isEntry) {
            char addr[8];
            to_base4_unique(cur->address, 4, addr);
            fprintf(fp, "%s %s\n", cur->symbolName, addr);
        }
        cur = cur->next;
    }

    fclose(fp);
    free(full);
    return UTIL_SUCCESS_S;
}

ErrCode writeExternFile(const char *filename, SymbolTable *symbolTable, ErrorList *errorList)
{
    /* extern references list populated by second pass */
    if (externHead == NULL) {
        /* no extern references -> no file to write */
        return UTIL_SUCCESS_S;
    }

    if (!filename) return FILE_WRITE_ERROR_F;

    char *full = (char*)malloc(strlen(filename) + 5);
    if (!full) return MALLOC_ERROR_F;
    strcpy(full, filename);
    strcat(full, ".ext");

    FILE *fp = fopen(full, "w");
    if (!fp) {
        free(full);
        return FILE_WRITE_ERROR_F;
    }

    ExternRec *cur = externHead;
    while (cur) {
        char addr[8];
        to_base4_unique(cur->address, 4, addr);
        fprintf(fp, "%s %s\n", cur->name, addr);
        cur = cur->next;
    }

    fclose(fp);
    free(full);
    /* clear extern records after writing (optional) */
    clearExternRecords();
    return UTIL_SUCCESS_S;
}
