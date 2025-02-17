#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "utils.h"



static void traverse(treeNode *t, void (*preProc)(treeNode *), void (*postProc)(treeNode *)) {
    if (t != NULL) {
        preProc(t);
        for (int i = 0; i < MAX_CHILDREN; i++) {
            traverse(t->children[i], preProc, postProc);
        }
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
}

static void popAfterInsertProc(treeNode *t) {
    if (t->nodeKind == StatementK && (t->kind.stmt == CompoundK)) {
        popScope();
    }
}

static void nullProc(treeNode *t) {
    if (t == NULL) return;
}

static void symbolError(treeNode *t, const char *message) {
    print_error(t->lineno, -1, "Symbol Error", message);
}

static void typeError(treeNode *t, const char *message) {
    print_error(t->lineno, -1, "Type Error", message);
}

static bool isFirstCompoundK = false;

static void insertNode(treeNode *t) {
    char msg[100];

    switch (t->nodeKind) {
        case ExpressionK: {
            switch (t->kind.exp) {
                case IdK: {
                    Symbol *sym = symtab_lookup_all_scope(t->attr.name);
                    if (sym == NULL) {
                        snprintf(msg, sizeof(msg), "Undeclared variable '\033[1;32m%s\033[1;37m' in the current scope", t->attr.name);
                        symbolError(t, msg);
                        break;
                    } else {
                        t->type = sym->type;
                        insertLines(t->attr.name, t->lineno);
                    }
                    break;
                }
                case CallK: {
                    Symbol *fun = symtab_lookup_all_scope(t->attr.name);
                    if (fun == NULL) {
                        snprintf(msg, sizeof(msg), "Undeclared function '\033[1;32m%s\033[1;37m'", t->attr.name);
                        symbolError(t, msg);
                        break;
                    } 
                    if (fun->kind != FUN) {
                        snprintf(msg, sizeof(msg), "Called object '\033[1;32m%s\033[1;37m' is not a function", t->attr.name);
                        symbolError(t, msg);
                        break;
                    }

                    // Check if the number of arguments is correct
                    int arg_count = 0;
                    treeNode *arg = t->children[0];
                    while (arg != NULL) {
                        // Arguments must be integer values
                        if (arg->kind.exp != OpK && arg->type != Integer) {
                            Symbol *argSym = symtab_lookup_all_scope(arg->attr.name);
                            if (argSym == NULL) {
                                snprintf(msg, sizeof(msg), "Undeclared variable '\033[1;32m%s\033[1;37m' in the current scope", arg->attr.name);
                                symbolError(t, msg);
                                break;
                            } else if (argSym->type == Void) {
                                snprintf(msg, sizeof(msg), "Function arguments must be integer values");
                                typeError(t, msg);
                                break;
                            }
                        }

                        arg_count++;
                        arg = arg->sibling;
                    }
                    
                    if (arg_count != fun->param_count) {
                        snprintf(msg, sizeof(msg), "Function '\033[1;32m%s\033[1;37m' expects %d arguments, but %d were given", t->attr.name, fun->param_count, arg_count);
                        symbolError(t, msg);
                        break;
                    }
                    
                    t->type = fun->type;
                    insertLines(t->attr.name, t->lineno);
                    break;
                }
                case OpK: {
                    Symbol *leftSide, *rightSide;
                    if (t->children[0]->kind.exp == IdK || t->children[0]->kind.exp == CallK) {
                        leftSide = symtab_lookup_all_scope(t->children[0]->attr.name);
                        if (leftSide == NULL) {
                            snprintf(msg, sizeof(msg), "Undeclared variable '\033[1;32m%s\033[1;37m' in the current scope", t->children[0]->attr.name);
                            symbolError(t, msg);
                            break;
                        }
                        if (leftSide->type == Void) {
                            snprintf(msg, sizeof(msg), "Cannot use void type function in operation");
                            typeError(t, msg);
                            break;
                        }
                    }
                    if (t->children[1]->kind.exp == IdK || t->children[1]->kind.exp == CallK) {
                        rightSide = symtab_lookup_all_scope(t->children[1]->attr.name);
                        if (rightSide == NULL) {
                            snprintf(msg, sizeof(msg), "Undeclared variable '\033[1;32m%s\033[1;37m' in the current scope", t->children[1]->attr.name);
                            symbolError(t, msg);
                            break;
                        }
                        if (rightSide->type == Void) {
                            snprintf(msg, sizeof(msg), "Cannot use void type function in operation");
                            typeError(t, msg);
                            break;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case StatementK: {
            switch (t->kind.stmt) {
                case CompoundK: {
                    if (!isFirstCompoundK) {
                        Scope scope = newScope(currScope()->name);
                        scope->parent = currScope();
                        pushScope(scope);
                    }
                    isFirstCompoundK = false;
                    break;
                }
                case FunDeclK: {
                    if (symtab_lookup_scope(t->attr.name) != NULL) {
                        snprintf(msg, sizeof(msg), "Function '\033[1;32m%s\033[1;37m' already declared", t->attr.name);
                        symbolError(t, msg);
                        break;
                    }

                    if (strcmp(currScope()->name, "global") == 0) {
                        symtab_insert(t->attr.name, currScope()->name, FUN, t->type, t);
                    }

                    Scope scope = newScope(t->attr.name);
                    scope->parent = currScope();
                    pushScope(scope);
                    isFirstCompoundK = true;
                    break;
                }
                case VarDeclK: {
                    if (symtab_lookup(t->attr.name) != NULL) {
                        snprintf(msg, sizeof(msg), "Variable '\033[1;32m%s\033[1;37m' already declared", t->attr.name);
                        symbolError(t, msg);
                        break;
                    }
                    
                    Symbol *var = symtab_lookup_all_scope(t->attr.name);
                    if (var != NULL && var->kind == FUN) {
                        snprintf(msg, sizeof(msg), "Invalid declaration of variable '\033[1;32m%s\033[1;37m' with the same name as a function", t->attr.name);
                        symbolError(t, msg);
                    }

                    if (t->type == Void) {
                        snprintf(msg, sizeof(msg), "Cannot declare void type variable");
                        symbolError(t, msg);
                        break;
                    }
                    symtab_insert(t->attr.name, currScope()->name, VAR, t->type, t);
                    break;
                }
                case ParamK: {
                    if (symtab_lookup(t->attr.name) != NULL) {
                        snprintf(msg, sizeof(msg), "Parameter '\033[1;32m%s\033[1;37m' already declared", t->attr.name);
                        symbolError(t, msg);
                        break;
                    }
                    // Void parameter don't need to be checked
                    if (t->type == Void) break;
                    
                    Symbol *fun = symtab_lookup_all_scope(currScope()->name);
                    fun->param_count++;
                    symtab_insert(t->attr.name, currScope()->name, VAR, t->type, t);
                    break;
                }
                case AssignK: {
                    if (t->children[1]->kind.exp != OpK && t->children[1]->type != Integer) {
                        Symbol *assign = symtab_lookup_all_scope(t->children[1]->attr.name);
                        if (assign->type == Void) {
                            snprintf(msg, sizeof(msg), "Cannot assign void type variable");
                            typeError(t, msg);
                            break;
                        }
                    }
                    break;
                }
                case ReturnK: {
                    Symbol *fun = symtab_lookup_all_scope(currScope()->name);
                    fun->hasReturn = true;
                    if (fun->type == Void && t->children[0] != NULL) {
                        snprintf(msg, sizeof(msg), "Function '\033[1;32m%s\033[1;37m' declared as void type", currScope()->name);
                        typeError(t, msg);
                        break;
                    } else if (fun->type != Void && t->children[0] == NULL) {
                        snprintf(msg, sizeof(msg), "Function '\033[1;32m%s\033[1;37m' must return a integer value", currScope()->name);
                        typeError(t, msg);
                        break;
                    } else if (t->children[0] != NULL) {
                        if (t->children[0]->type != Integer) {
                            Symbol *ret = symtab_lookup_all_scope(t->children[0]->attr.name);
                            if (ret == NULL) {
                                snprintf(msg, sizeof(msg), "Undeclared function '\033[1;32m%s\033[1;37m'", t->children[0]->attr.name);
                                typeError(t, msg);
                                break;
                            } else if (ret->type != fun->type) {
                                snprintf(msg, sizeof(msg), "Function '\033[1;32m%s\033[1;37m' must return a integer value", fun->name);
                                typeError(t, msg);
                                break;
                            }
                        }
                    }
                    break;
                }
                case WhileK: {
                    if (t->children[0]->kind.exp != OpK && t->children[0]->kind.exp != ConstK) {
                        Symbol *whileExp = symtab_lookup_all_scope(t->children[0]->attr.name);
                        if (whileExp->type == Void) {
                            snprintf(msg, sizeof(msg), "While condition must be a integer value");
                            typeError(t, msg);
                            break;
                        }
                    }
                    break;
                }
                case IfK: {
                    if (t->children[0]->kind.exp != OpK && t->children[0]->kind.exp != ConstK) {
                        Symbol *ifExp = symtab_lookup_all_scope(t->children[0]->attr.name);
                        if (ifExp->type == Void) {
                            snprintf(msg, sizeof(msg), "If condition must be a integer value");
                            typeError(t, msg);
                            break;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}

static void insertBuiltInFunctions() {
    treeNode *input_fun_decl = newStmtNode(FunDeclK);
    input_fun_decl->type = Integer;

    treeNode *input_type_specifier = newStmtNode(FunDeclK);
    input_type_specifier->type = Integer;

    treeNode *input_compound_stmt = newStmtNode(CompoundK);
    input_compound_stmt->children[0] = NULL;
    input_compound_stmt->children[1] = NULL;

    input_fun_decl->attr.name = "input";
    input_fun_decl->lineno = 0;
    input_fun_decl->children[0] = input_type_specifier;
    input_fun_decl->children[1] = NULL;
    input_fun_decl->children[2] = input_compound_stmt;

    symtab_insert("input", "global", FUN, Integer, input_fun_decl);
    symtab_lookup_all_scope("input")->hasReturn = true;

    treeNode *output_fun_decl = newStmtNode(FunDeclK);
    output_fun_decl->type = Void;

    treeNode *output_type_specifier = newStmtNode(FunDeclK);
    output_type_specifier->type = Void;

    treeNode *output_params = newStmtNode(ParamK);
    output_params->attr.name = "arg";
    output_params->children[0] = newExpNode(IdK);
    output_params->children[0]->type = Integer;

    treeNode *output_compound_stmt = newStmtNode(CompoundK);
    output_compound_stmt->children[0] = NULL;
    output_compound_stmt->children[1] = NULL;

    output_fun_decl->attr.name = "output";
    output_fun_decl->lineno = 0;
    output_fun_decl->children[0] = output_type_specifier;
    output_fun_decl->children[1] = output_params;
    output_fun_decl->children[2] = output_compound_stmt;

    symtab_insert("output", "global", FUN, Void, output_fun_decl);
    symtab_lookup_all_scope("output")->param_count = 1;
}

// Check if main function is declared
static void checkMainFunction() {
    Symbol *main = symtab_lookup_all_scope("main");
    if (main == NULL) {
        print_error(-1, -1, "Symbol Error", "main function not declared");
    }
}

void checkFunctionReturns(Scope globalScope) {
    for (int i = 0; i < globalScope->symbol_count; i++) {
        Symbol sym = globalScope->symTab[i];
        
        if (sym.kind == FUN && sym.type == Integer && !sym.hasReturn) {
            char msg[100];
            snprintf(msg, sizeof(msg), "Function '\033[1;32m%s\033[1;37m' must return a value", sym.name);
            print_error(sym.lines[0], -1, "Type Error", msg);
        }
    }
}


void buildSymTab(treeNode *syntaxTree) {
    Scope globalScope = newScope("global");
    pushScope(globalScope);

    insertBuiltInFunctions();

    // Semantic Analysis
    traverse(syntaxTree, insertNode, popAfterInsertProc);
    
    checkMainFunction();
    checkFunctionReturns(globalScope);
    
    popScope();
    // printSymTab();
}