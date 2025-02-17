#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdio.h>
#include <stdbool.h>
#include "globals.h"

#define SIZE 200  // symbol table size
#define LINE_SIZE 50 // max number of occurrences of a symbol

typedef enum { VAR, FUN } SymbolKind;

typedef struct {
    char *name;
    int lines[LINE_SIZE];
    int line_count;
    int param_count;
    SymbolKind kind;
    char *scope;
    ExpType type;
    bool hasReturn;
} Symbol;

typedef struct ScopeListRec {
    char *name; // function name
    int nestedLevel;
    int startLine;
    struct ScopeListRec *parent;
    Symbol symTab[SIZE];
    int symbol_count;
} * Scope;

extern Scope globalScope;

static Scope scopeList[SIZE];
static int sizeOfScopeList = 0;

static Scope scopeStack[SIZE];
static int topScope = 0;

int binarySearch(char *name, Scope scope);

Scope newScope(char *scopeName);
Scope currScope();
void popScope();
void pushScope(Scope scope);
void insertScope(Scope scope); // insert scope into scopeList


void symtab_insert(char *name, char *scopeName, SymbolKind kind, ExpType type, treeNode *node);
Symbol* symtab_lookup(char *name);
Scope symtab_lookup_scope(char *scopeName);
Symbol* symtab_lookup_all_scope(char *name);
void insertLines(char *name, int lineno);

void printSymTab();
void printSymTabToFile(FILE *file);

#endif
