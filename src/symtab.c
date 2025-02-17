#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

Scope globalScope = NULL;

Scope newScope(char *scopeName) {
    Scope scope = (Scope) malloc(sizeof(struct ScopeListRec));
    scope->name = scopeName;
    scope->nestedLevel = 0;
    scope->startLine = 0;
    scope->parent = NULL;
    scope->symbol_count = 0;
    return scope;
}

Scope currScope() {
    return scopeStack[topScope-1];
}

void popScope() {
    scopeStack[topScope--] = NULL;
}

void pushScope(Scope scope) {
    for (int i = 0; i < sizeOfScopeList; i++) {
        if (strcmp(scopeList[i]->name, scope->name) == 0) {
            scope->nestedLevel = scopeList[i]->nestedLevel + 1;
        }
    }
    scopeStack[topScope++] = scope;
    insertScope(scope);
}

void insertScope(Scope scope) {
    scopeList[sizeOfScopeList++] = scope;
}


int binarySearch(char *name, Scope scope) {
    int low = 0;
    int high = scope->symbol_count - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (strcmp(scope->symTab[mid].name, name) == 0) {
            return mid;
        } else if (strcmp(scope->symTab[mid].name, name) < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

void symtab_insert(char *name, char *scopeName, SymbolKind kind, ExpType type, treeNode *node) {
    Scope scope = currScope();
    // try to find the symbol in the current scope
    int index = binarySearch(name, scope);
    if (index != -1) {
        // symbol already exists in the current scope
        Symbol *sym = &scope->symTab[index];
        sym->lines[sym->line_count++] = node->lineno;
        return;
    }
    
    // symbol does not exist in the current scope
    Symbol *sym = &scope->symTab[scope->symbol_count++];
    sym->name = name;
    sym->param_count = 0;
    sym->line_count = 0;
    sym->lines[sym->line_count++] = node->lineno;
    sym->kind = kind;
    sym->scope = scopeName;
    sym->type = type;
}
    
Symbol* symtab_lookup(char *name) {
    Scope scope = currScope();
    int index = binarySearch(name, scope);
    if (index != -1) {
        return &scope->symTab[index];
    }
    return NULL;
}

Scope symtab_lookup_scope(char *scopeName) {
    for (int i = 0; i < sizeOfScopeList; i++) {
        if (strcmp(scopeList[i]->name, scopeName) == 0) {
            return scopeList[i];
        }
    }
    return NULL;
}

Symbol* symtab_lookup_all_scope(char *name) {
    Scope scope = currScope();
    while (scope != NULL) {
        for (int i = 0; i < scope->symbol_count; i++) {
            if (strcmp(scope->symTab[i].name, name) == 0) {
                return &scope->symTab[i];
            }
        }
        scope = scope->parent;
    }
    return NULL;
}

void insertLines(char *name, int lineno) {
    Symbol *sym = symtab_lookup_all_scope(name);
    sym->lines[sym->line_count++] = lineno;
}

void printSymTab() {
    printf("%-15s %-10s %-10s %-10s %-10s\n", "Variable Name", "Scope", "Type", "Kind", "Line Numbers");
    printf("-------------------------------------------------------------------\n");
    for (int i = 0; i < sizeOfScopeList; i++) {
        Scope scope = scopeList[i];
        for (int j = 0; j < scope->symbol_count; j++) {
            Symbol sym = scope->symTab[j];

            printf("%-15s %-10s %-10s %-10s ", 
                sym.name, 
                sym.scope, 
                sym.type == Integer ? "integer" : "void", 
                sym.kind == VAR ? "var" : "fun");

            int lastPrinted = -1;  
            for (int k = 0; k < sym.line_count; k++) {
                if (sym.lines[k] != lastPrinted) {
                    if (k > 0) printf(", ");
                    printf("%d", sym.lines[k]);
                    lastPrinted = sym.lines[k];
                }
            }
            printf("\n");
        }
    }
}

void printSymTabToFile(FILE *file) {
    fprintf(file, "%-15s %-10s %-10s %-10s %-10s\n", "Variable Name", "Scope", "Type", "Kind", "Line Numbers");
    fprintf(file, "-------------------------------------------------------------------\n");
    for (int i = 0; i < sizeOfScopeList; i++) {
        Scope scope = scopeList[i];
        for (int j = 0; j < scope->symbol_count; j++) {
            Symbol sym = scope->symTab[j];

            fprintf(file, "%-15s %-10s %-10s %-10s ", 
                sym.name, 
                sym.scope, 
                sym.type == Integer ? "integer" : "void", 
                sym.kind == VAR ? "var" : "fun");

            int lastPrinted = -1;
            for (int k = 0; k < sym.line_count; k++) {
                if (sym.lines[k] != lastPrinted) {
                    if (k > 0) fprintf(file, ", ");
                    fprintf(file, "%d", sym.lines[k]);
                    lastPrinted = sym.lines[k];
                }
            }
            fprintf(file, "\n");
        }
    }
}
