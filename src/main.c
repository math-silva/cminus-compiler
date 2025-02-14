#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "utils.h"
#include "parser.tab.h"
#include "analyze.h"
#include "output.h"

void openFile(char *filename);
treeNode *parse();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    char outputPath[256];
    createOutputDir(argv[1], outputPath);

    openFile(argv[1]);

    treeNode* tree = parse();
    if (tree != NULL) {
        saveSyntaxTree(outputPath, tree);
        buildSymTab(tree);
        saveSymbolTable(outputPath);
    }

    printf("Compilation successful\n");
    printf("Results saved in %s\n", outputPath);
    return 0;
}