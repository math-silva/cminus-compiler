%{
#include <stdio.h>
#include <stdlib.h>

void open_file(char *filename);
void yyerror(char *);

extern int yylineno;
extern int yylex(void);
%}

%union {
    int integer;
    char* string;
}

%token ELSE IF INT RETURN VOID WHILE
%token PLUS MINUS TIMES DIV LEQ LT GEQ GT EQ NEQ ASSIGN
%token COMMA SEMICOLON LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE
%token <integer> NUMBER
%token <string> ID

%left PLUS MINUS
%left TIMES DIV
%left LEQ LT GEQ GT EQ NEQ
%right ASSIGN

%nonassoc ELSE

%start program

%%

program: declaration_list
    ;

declaration_list: declaration_list declaration
    | declaration
    ;

declaration: var_declaration
    | fun_declaration
    ;

var_declaration: type_specifier ID SEMICOLON
    | type_specifier ID LBRACKET NUMBER RBRACKET SEMICOLON
    ;

type_specifier: INT | VOID
    ;

fun_declaration: type_specifier ID LPAREN params RPAREN compound_stmt
    ;

params: param_list
    | VOID
    ;

param_list: param_list COMMA param
    | param
    ;

param: type_specifier ID
    | type_specifier ID LBRACKET RBRACKET
    ;

compound_stmt: LBRACE local_declarations statement_list RBRACE
    ;

local_declarations: local_declarations var_declaration
    |
    ;

statement_list: statement_list statement
    |
    ;

statement: expression_stmt
    | compound_stmt
    | selection_stmt
    | iteration_stmt
    | return_stmt
    ;

expression_stmt: expression SEMICOLON
    | SEMICOLON
    ;

selection_stmt: IF LPAREN expression RPAREN statement
    | IF LPAREN expression RPAREN statement ELSE statement
    ;

iteration_stmt: WHILE LPAREN expression RPAREN statement
    ;

return_stmt: RETURN SEMICOLON
    | RETURN expression SEMICOLON
    ;

expression: var ASSIGN expression
    | simple_expression
    ;

var: ID
    | ID LBRACKET expression RBRACKET
    ;

simple_expression: additive_expression relop additive_expression
    | additive_expression
    ;

relop: LEQ | LT | GEQ | GT | EQ | NEQ
    ;

additive_expression: additive_expression addop term
    | term
    ;

addop: PLUS | MINUS
    ;

term: term mulop factor
    | factor
    ;

mulop: TIMES | DIV
    ;

factor: LPAREN expression RPAREN
    | var
    | call
    | NUMBER
    ;

call: ID LPAREN args RPAREN
    ;

args: arg_list
    | 
    ;

arg_list: arg_list COMMA expression
    | expression
    ;

%%

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "%s <input_file>\n", argv[0]);
        return 1;
    }
    open_file(argv[1]);
    return yyparse();
}

void yyerror(char *msg) {
    extern char *yytext;
    printf("\nSyntatic error at line %d: Unexpected error near '%s'\n", yylineno, yytext);
    exit(1);
}