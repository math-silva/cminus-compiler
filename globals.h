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
    } attr;
    ExpType type;
} treeNode;

#include "parser.tab.h"

#endif