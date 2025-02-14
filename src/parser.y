%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"
#include "utils.h"

void yyerror(char *);

extern int yylineno;
extern int yylex(void);

treeNode * parse();

static treeNode* tree;
static char* name;

char* filename = NULL;
char current_scope[100] = "global";

%}


%union {
    treeNode *node;
    int integer;
    char* string;
    TokenAttr *token;
}

%token ELSE IF INT RETURN VOID WHILE
%token PLUS MINUS TIMES DIV LEQ LT GEQ GT EQ NEQ ASSIGN
%token COMMA SEMICOLON LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE
%token <integer> NUMBER
%token <token> ID

%left PLUS MINUS
%left TIMES DIV
%left LEQ LT GEQ GT EQ NEQ
%right ASSIGN

%type <node> program
%type <node> expression factor var call simple_expression
%type <node> additive_expression term args arg_list param_list param params declaration_list
%type <node> declaration var_declaration type_specifier fun_declaration compound_stmt local_declarations
%type <node> statement_list statement selection_stmt iteration_stmt return_stmt expression_stmt
%type <node> mult add rel

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%start program

%%

program: declaration_list
    {
        $$ = $1;
        tree = $1;
    }
    ;

declaration_list: declaration_list declaration
    { $$ = addSibling($1, $2); }
    | declaration
    ;

declaration: var_declaration
    | fun_declaration
    ;

var_declaration: type_specifier ID SEMICOLON
    {
        $$ = newStmtNode(VarDeclK);
        $$->type = $1->type;
        $$->attr.name = strdup($2->name);
        $$->lineno = $2->lineno;
    }
    | type_specifier ID LBRACKET NUMBER RBRACKET SEMICOLON
    {
        $$ = newStmtNode(VarDeclK);
        $$->type = $1->type;
        $$->attr.name = strdup($2->name);
        treeNode *size = newExpNode(ConstK);
        size->attr.val = $4;
        $$->children[0] = size;
        $$->lineno = $2->lineno;
    }
    ;

type_specifier: INT 
    {
        $$ = newExpNode(IdK);
        $$->type = Integer;
    }
    | VOID
    {
        $$ = newExpNode(IdK);
        $$->type = Void;
    }
    ;

fun_declaration: type_specifier ID LPAREN params RPAREN compound_stmt
    {
        $$ = newStmtNode(FunDeclK);
        $$->type = $1->type;
        $$->attr.name = strdup($2->name);
        $$->children[0] = $4;
        $$->children[1] = $6;
        $$->lineno = $2->lineno;
    }
    ;

params: param_list
    | VOID
    {
        $$ = newStmtNode(ParamK);
        $$->type = Void;
    }
    ;

param_list: param_list COMMA param
    { $$ = addSibling($1, $3); }
    | param
    ;

param: type_specifier ID
    {
        $$ = newStmtNode(ParamK);
        $$->type = $1->type;
        $$->attr.name = strdup($2->name);
        $$->lineno = $2->lineno;
    }
    | type_specifier ID LBRACKET RBRACKET
    {
        $$ = newStmtNode(ParamK);
        $$->type = $1->type;
        $$->attr.name = strdup($2->name);
        $$->lineno = $2->lineno;
    }
    ;

compound_stmt: LBRACE local_declarations statement_list RBRACE
    {
        $$ = newStmtNode(CompoundK);
        $$->children[0] = $2;
        $$->children[1] = $3;
    }
    ;

local_declarations: local_declarations var_declaration
    { $$ = addSibling($1, $2); }
    | { $$ = NULL; }
    ;

statement_list: statement_list statement
    { $$ = addSibling($1, $2); }
    | { $$ = NULL; }
    ;

statement: expression_stmt
    | compound_stmt
    | selection_stmt
    | iteration_stmt
    | return_stmt
    ;

expression_stmt: expression SEMICOLON
    | SEMICOLON { $$ = NULL; }
    ;

selection_stmt: IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
    {
        $$ = newStmtNode(IfK);
        $$->children[0] = $3;
        $$->children[1] = $5;
    }
    | IF LPAREN expression RPAREN statement ELSE statement
    {
        $$ = newStmtNode(IfK);
        $$->children[0] = $3;
        $$->children[1] = $5;
        $$->children[2] = $7;
    }
    ;

iteration_stmt: WHILE LPAREN expression RPAREN statement
    {
        $$ = newStmtNode(WhileK);
        $$->children[0] = $3;
        $$->children[1] = $5;
    }
    ;

return_stmt: RETURN SEMICOLON
    { 
        $$ = newStmtNode(ReturnK); 
        $$->type = Void;
    }
    | RETURN expression SEMICOLON
    {
        $$ = newStmtNode(ReturnK);
        $$->children[0] = $2;
    }
    ;

expression: var ASSIGN expression
    {
        $$ = newStmtNode(AssignK);
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | simple_expression
    ;

var: ID
    {
        $$ = newExpNode(IdK);
        $$->attr.name = strdup($1->name);
        $$->lineno = $1->lineno;
    }
    | ID LBRACKET expression RBRACKET
    {
        $$ = newExpNode(IdK);
        $$->attr.name = strdup($1->name);
        $$->children[0] = $3;
        $$->lineno = $1->lineno;
    }
    ;

simple_expression: additive_expression rel additive_expression
    {
        $$ = newExpNode(OpK);
        $$->attr.op = $2->attr.op;
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | additive_expression
    ;

rel: LEQ { $$ = newExpNode(OpK); $$->attr.op = LEQ; } 
    | LT { $$ = newExpNode(OpK); $$->attr.op = LT; } 
    | GEQ { $$ = newExpNode(OpK); $$->attr.op = GEQ; } 
    | GT { $$ = newExpNode(OpK); $$->attr.op = GT; }
    | EQ { $$ = newExpNode(OpK); $$->attr.op = EQ; }
    | NEQ { $$ = newExpNode(OpK); $$->attr.op = NEQ; }
    ;

additive_expression: additive_expression add term
    {
        $$ = newExpNode(OpK);
        $$->attr.op = $2->attr.op;
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | term
    ;

add: PLUS { $$ = newExpNode(OpK); $$->attr.op = PLUS; } 
    | MINUS { $$ = newExpNode(OpK); $$->attr.op = MINUS; }
    ;

term: term mult factor
    {
        $$ = newExpNode(OpK);
        $$->attr.op = $2->attr.op;
        $$->children[0] = $1;
        $$->children[1] = $3;
    }
    | factor
    ;

mult: TIMES { $$ = newExpNode(OpK); $$->attr.op = TIMES; } 
    | DIV { $$ = newExpNode(OpK); $$->attr.op = DIV; }
    ;

factor: LPAREN expression RPAREN { $$ = $2; }
    | var
    | call
    | NUMBER
    {
        $$ = newExpNode(ConstK);
        $$->attr.val = $1;
        $$->type = Integer;
    }
    ;

call: ID LPAREN args RPAREN
    {
        $$ = newExpNode(CallK);
        $$->attr.name = strdup($1->name);
        $$->children[0] = $3;
        $$->lineno = $1->lineno;
    }
    ;

args: arg_list
    | { $$ = NULL; }
    ;

arg_list: arg_list COMMA expression
    { $$ = addSibling($1, $3); }
    | expression
    ;

%%

void yyerror(char *msg) {
    extern char *yytext;
    char error_msg[50];
    snprintf(error_msg, sizeof(error_msg), "Unexpected token '\033[1;32m%s\033[0m'", yytext);
    print_error(yylineno, column, "Syntax Error", error_msg);
}

treeNode * parse() {
    yyparse();
    return tree;
}

void openFile(char *fname) {
    FILE* file = fopen(fname, "r");
    if (file == NULL) {
        printf("Cannot open file %s\n", fname);
        exit(1);
    }
    extern FILE* yyin;
    yyin = file;
    filename = fname;
}