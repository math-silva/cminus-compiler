#include "globals.h"
#include "utils.h"
#include "parser.tab.h"
#include "analyze.h"

void openFile(char *filename);
treeNode *parse();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(1);
    }
    openFile(argv[1]);
    treeNode* tree = parse();
    if (tree != NULL) {
        //printTree(tree);
        buildSymTab(tree);
    }
    return 0;
}