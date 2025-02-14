#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include "output.h"
#include "parser.tab.h"
#include "analyze.h"
#include "symtab.h"
#include "utils.h"

void removeDir(const char *path) {
    char command[256];
    snprintf(command, sizeof(command), "rm -rf %s", path);
    system(command);
}

void createOutputDir(char *filepath, char *outputPath) {
    char *filename = strrchr(filepath, '/');
    filename = (filename) ? filename + 1 : filepath;

    char *dot = strrchr(filename, '.');
    int nameLength = (dot) ? (dot - filename) : strlen(filename);

    snprintf(outputPath, 256, "output/%.*s", nameLength, filename);

    struct stat st;
    if (stat(outputPath, &st) == 0) {
        removeDir(outputPath);
    }

    mkdir("output", 0777);
    mkdir(outputPath, 0777);
}

// void saveTokens(char *outputPath) {
//     TODO: Implement this function
// }

void saveSyntaxTree(char *outputPath, treeNode *tree) {
    char treeFile[512];
    sprintf(treeFile, "%s/syntax_tree.txt", outputPath);

    FILE *file = fopen(treeFile, "w");
    if (!file) {
        perror("Error creating syntax tree file");
        return;
    }
    printTreeToFile(tree, file);

    fclose(file);
}

void saveSymbolTable(char *outputPath) {
    char symTabFile[512];
    sprintf(symTabFile, "%s/symbol_table.txt", outputPath);

    FILE *file = fopen(symTabFile, "w");
    if (!file) {
        perror("Error creating symbol table file");
        return;
    }

    printSymTabToFile(file);

    fclose(file);
}
