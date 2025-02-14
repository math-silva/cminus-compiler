#ifndef OUTPUT_H
#define OUTPUT_H

#include "globals.h"

void createOutputDir(char *filepath, char *outputPath);
// TODO: void saveTokens(char *outputPath);
void saveSyntaxTree(char *outputPath, treeNode *tree);
void saveSymbolTable(char *outputPath);

#endif
