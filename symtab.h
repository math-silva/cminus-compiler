#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdio.h>

#define SIZE 500  // symbol table size
#define LINE_SIZE 50 // max number of occurrences of a symbol

typedef enum { VAR, FUN } SymbolKind;
typedef enum { VOID_T, INT_T } DataType;

typedef struct {
    char *name;
    int lines[LINE_SIZE];
    int line_count;
    SymbolKind kind;
    char *scope;
    DataType dataType;
} Symbol;

extern Symbol symtab[SIZE];
extern int symbol_count;

void insert_symbol(char *name, char *scope, SymbolKind kind, DataType dataType, int line);
int lookup_symbol(char *name, char *scope, SymbolKind kind);
void print_table();

#endif
