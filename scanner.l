%{
#include <stdio.h>
#include <string.h>
#include "globals.h"

#include "utils.h"

extern YYSTYPE yylval;

int line_number = 1; // Line counter
%}

%option yylineno

%%
"else"      { return ELSE; }
"if"        { return IF; }
"int"       { return INT; }
"return"    { return RETURN; }
"void"      { return VOID; }
"while"     { return WHILE; }

"+"         { return PLUS; }
"-"         { return MINUS; }
"*"         { return TIMES; }
"/"         { return DIV; }
"<="        { return LEQ; }
"<"         { return LT; }
">="        { return GEQ; }
">"         { return GT; }
"=="        { return EQ; }
"!="        { return NEQ; }
"="         { return ASSIGN; }

","         { return COMMA; }
";"         { return SEMICOLON; }
"("         { return LPAREN; }
")"         { return RPAREN; }
"["         { return LBRACKET; }
"]"         { return RBRACKET; }
"{"         { return LBRACE; }
"}"         { return RBRACE; }

[0-9]+      { yylval.integer = atoi(yytext); return NUMBER; }

[a-zA-Z][a-zA-Z0-9]* {
    yylval.string = strdup(yytext); return ID;
}

[ \t]+      { /* Ignore spaces and tabs */ }

\n          { line_number++; }

"/*"([^*]|\*+[^*/])*\*+\/ { /* Ignore block comments */ }
"//"[^\n]* { /* Ignore single-line comments */ }

.           { printf("Unknown character: %c\n", yytext[0]); }

%%

int yywrap() {
    return 1;
}