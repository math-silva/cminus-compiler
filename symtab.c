#include "symtab.h"
#include <stdlib.h>
#include <string.h>

Symbol symtab[SIZE];
int symbol_count = 0;

int binary_search(char *name, char *scope, SymbolKind kind) {
    int left = 0, right = symbol_count - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(symtab[mid].name, name);
        if (cmp == 0) {
            cmp = strcmp(symtab[mid].scope, scope);
            if (cmp == 0 && symtab[mid].kind == kind) {
                return mid;
            }
        }
        if (cmp < 0) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

void insert_symbol(char *name, char *scope, SymbolKind kind, DataType dataType, int line) {
    if (symbol_count >= SIZE) {
        printf("Error: symbol table is full\n");
        return;
    }
    int pos = binary_search(name, scope, kind);
    if (pos != -1) { // Symbol already exists
        if (symtab[pos].line_count < LINE_SIZE)
            symtab[pos].lines[symtab[pos].line_count++] = line;
        return;
    }

    // Shift symbols to the right
    int i = symbol_count - 1;
    while (i >= 0 && strcmp(symtab[i].name, name) > 0) {
        symtab[i + 1] = symtab[i];
        i--;
    }
    // Insert new symbol
    symtab[i + 1].name = strdup(name);
    symtab[i + 1].scope = strdup(scope);
    symtab[i + 1].kind = kind;
    symtab[i + 1].dataType = dataType;
    symtab[i + 1].lines[0] = line;
    symtab[i + 1].line_count = 1;
    symbol_count++;
}

int lookup_symbol(char *name, char *scope, SymbolKind kind) {
    return binary_search(name, scope, kind);
}

void print_table() {
    printf("%-15s %-10s %-7s %-7s %-7s\n", "Variable Name", "Scope", "Type", "Kind", "Line Numbers");
    printf("-----------------------------------------------------------\n");
    for (int i = 0; i < symbol_count; i++) {
        printf("%-15s %-10s %-7s %-7s ",
            symtab[i].name,
            symtab[i].scope,
            symtab[i].dataType == INT_T ? "int" : "void",
            symtab[i].kind == VAR ? "var" : "fun");
        for (int j = 0; j < symtab[i].line_count; j++) {
            printf("%d ", symtab[i].lines[j]);
        }
        printf("\n");
    }
}
