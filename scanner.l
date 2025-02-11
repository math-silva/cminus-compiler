%{
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "parser.tab.h"

extern YYSTYPE yylval;
extern void open_file(char *filename);

int line_number = 1; // Line counter
%}

%option yylineno

%%
"else"      { printf("ELSE "); return ELSE; }
"if"        { printf("IF "); return IF; }
"int"       { printf("INT "); return INT; }
"return"    { printf("RETURN "); return RETURN; }
"void"      { printf("VOID "); return VOID; }
"while"     { printf("WHILE "); return WHILE; }

"+"         { printf("PLUS "); return PLUS; }
"-"         { printf("MINUS "); return MINUS; }
"*"         { printf("TIMES "); return TIMES; }
"/"         { printf("DIV "); return DIV; }
"<="        { printf("LEQ "); return LEQ; }
"<"         { printf("LT "); return LT; }
">="        { printf("GEQ "); return GEQ; }
">"         { printf("GT "); return GT; }
"=="        { printf("EQ "); return EQ; }
"!="        { printf("NEQ "); return NEQ; }
"="         { printf("ASSIGN "); return ASSIGN; }

","         { printf("COMMA "); return COMMA; }
";"         { printf("SEMICOLON "); return SEMICOLON; }
"("         { printf("LPAREN "); return LPAREN; }
")"         { printf("RPAREN "); return RPAREN; }
"["         { printf("LBRACKET "); return LBRACKET; }
"]"         { printf("RBRACKET "); return RBRACKET; }
"{"         { printf("LBRACE "); return LBRACE; }
"}"         { printf("RBRACE "); return RBRACE; }

[0-9]+      { printf("NUMBER(%s) ", yytext); yylval.integer = atoi(yytext); return NUMBER; }

[a-zA-Z][a-zA-Z0-9]* {
    printf("ID(%s) ", yytext); yylval.string = strdup(yytext); return ID;
}

[ \t]+      { /* Ignore spaces and tabs */ }

\n          { printf("\n"); line_number++; }

"/*"([^*]|\*+[^*/])*\*+\/ { /* Ignore block comments */ }
"//"[^\n]* { /* Ignore single-line comments */ }

.           { fprintf(stderr, "Unknown character: %c\n", yytext[0]); }

%%

void open_file(char *filename) {
    FILE *input = fopen(filename, "r");
    if (!input) {
        perror("Error opening file");
        exit(1);
    }
    yyin = input;
}

int yywrap() {
    return 1;
}