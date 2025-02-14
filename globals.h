#ifndef GLOBALS_H
#define GLOBALS_H

#define MAX_CHILDREN 3

typedef enum NodeKind {
    StatementK, ExpressionK
} NodeKind;

typedef enum {
    IfK, WhileK, ReturnK, CompoundK, AssignK, VarDeclK, FunDeclK, ParamK
} StatementKind;

typedef enum {
    OpK, ConstK, IdK, CallK
} ExpressionKind;

typedef enum {
    Void, Integer
} ExpType;

struct ScopeListRec;

typedef struct treeNode {
    struct treeNode *children[MAX_CHILDREN];
    struct treeNode *sibling;
    int lineno;
    NodeKind nodeKind;

    union {
        StatementKind stmt;
        ExpressionKind exp;
    } kind;

    union {
        int op;
        int val;
        char *name;
        struct ScopeListRec *scope;
    } attr;

    ExpType type;
} treeNode;

#include "parser.tab.h"

extern char *filename;
extern int column;

void print_error(int line, int col, const char *type, const char *msg);

#endif