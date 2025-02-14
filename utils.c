#include "utils.h"

treeNode * newExpNode(ExpressionKind kind) { 
    treeNode * t = (treeNode *) malloc(sizeof(treeNode));
    int i;
    if (t==NULL)
        printf("Out of memory error at line %d\n", yylineno);
    else {
        for (i=0;i<MAX_CHILDREN;i++) t->children[i] = NULL;
            t->sibling = NULL;
            t->nodeKind = ExpressionK;
            t->kind.exp = kind;
            t->lineno = yylineno;
            t->type = Void;
    }
    return t;
}

treeNode * newStmtNode(StatementKind kind) { 
    treeNode * t = (treeNode *) malloc(sizeof(treeNode));
    int i;
    if (t==NULL)
        printf("Out of memory error at line %d\n", yylineno);
    else {
        for (i=0;i<MAX_CHILDREN;i++) t->children[i] = NULL;
            t->sibling = NULL;
            t->nodeKind = StatementK;
            t->kind.stmt = kind;
            t->lineno = yylineno;
    }
    return t;
}

treeNode * addSibling(treeNode* first, treeNode* newSibling) {
    if (first != NULL) {
        treeNode *t = first;
        while (t->sibling != NULL)
            t = t->sibling;
        t->sibling = newSibling;
        return first;
    } else {
        return newSibling;
    }
}

void printSpaces(int num) {
    for (int i = 0; i < num; i++)
        printf(" ");
}

char * getExpKindString(ExpressionKind kind) {
    switch (kind) {
        case OpK:
            return "Op";
        case ConstK:
            return "Const";
        case IdK:
            return "Id";
        case CallK:
            return "Call";
        default:
            return "Unknown";
    }
}

char * getStmtKindString(StatementKind kind) {
    switch (kind) {
        case IfK:
            return "If";
        case WhileK:
            return "While";
        case AssignK:
            return "Assign";
        case ReturnK:
            return "Return";
        case CompoundK:
            return "Compound";
        case VarDeclK:
            return "VarDecl";
        case FunDeclK:
            return "FunDecl";
        case ParamK:
            return "Param";
        default:
            return "Unknown";
    }
}

char * getExpTypeString(ExpType type) {
    switch (type) {
        case Void:
            return "Void";
        case Integer:
            return "Integer";
        default:
            return "Unknown";
    }
}

char * getTokenString(int type) {
    switch (type) {
        case PLUS:
            return "+";
        case MINUS:
            return "-";
        case TIMES:
            return "*";
        case DIV:
            return "/";
        case LT:
            return "<";
        case LEQ:
            return "<=";
        case GT:
            return ">";
        case GEQ:
            return ">=";
        case EQ:
            return "==";
        case NEQ:
            return "!=";
        case ASSIGN:
            return "=";
        default:
            return "Unknown Token";
    }
}

void printTree(treeNode * tree) {
    static int indent = 0;

    while (tree != NULL) {
        printSpaces(indent);
        if (tree->nodeKind == StatementK) {
            printf("Stmt: %s\n", getStmtKindString(tree->kind.stmt));
            if (tree->kind.stmt == VarDeclK || tree->kind.stmt == FunDeclK) {
                printf(" %s %s", tree->attr.name, getExpTypeString(tree->type));
            }
            if (tree->kind.stmt == ParamK) {
                if (tree->type != Void) {
                    printf(" %s %s", tree->attr.name, getExpTypeString(tree->type));
                } else {
                    printf(" Void ");
                }
            }
            printf("\n");
        } else if (tree->nodeKind == ExpressionK) {
            printf("Exp: %s", getExpKindString(tree->kind.exp));
            if (tree->kind.exp == OpK) {
                printf(" %s\n", getTokenString(tree->attr.op));
            } else if (tree->kind.exp == ConstK) {
                printf(" %d\n", tree->attr.val);
            } else if (tree->kind.exp == IdK || tree->kind.exp == CallK) {
                printf(" %s\n", tree->attr.name);
            }
            printf("\n");
        }

        indent += 4;
        for (int i = 0; i < MAX_CHILDREN; i++) {
            printTree(tree->children[i]);
        }
        indent -= 4;
        tree = tree->sibling;
    }
}

void print_error(int line, int col, const char *type, const char *msg) {
    if (line == -1) {
        fprintf(stderr, "\033[1;33m%s:\033[0m \033[1;31m%s:\033[0m \033[1;37m%s\033[0m\n",
                filename ? filename : "stdin", type, msg);
    } else if (col == -1) {
        fprintf(stderr, "\033[1;33m%s:%d:\033[0m \033[1;31m%s:\033[0m \033[1;37m%s\033[0m\n",
                filename ? filename : "stdin", line, type, msg);
    } else {
        fprintf(stderr, "\033[1;33m%s:%d:%d:\033[0m \033[1;31m%s:\033[0m \033[1;37m%s\033[0m\n",
                filename ? filename : "stdin", line, col, type, msg);

    }
    exit(1);
}