#ifndef UTILS_H
#define UTILS_H

#include "globals.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern int yylineno;

treeNode* newExpNode(ExpressionKind kind);
treeNode* newStmtNode(StatementKind kind);
treeNode* addSibling(treeNode *first, treeNode *newSibling);
void printSpaces(int num);
void printSpacesToFile(int num, FILE *file);
char* getExpKindString(ExpressionKind kind);
char* getStmtKindString(StatementKind kind);
char* getExpTypeString(ExpType type);
char* getTokenString(int type);
void printTree(treeNode *tree);
void printTreeToFile(treeNode *tree, FILE *file);

#endif