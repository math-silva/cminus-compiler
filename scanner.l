%{
#include "globals.h"
extern int yylineno;
int column = 0;
%}

%option yylineno

%%

"else"      { column += yyleng; return ELSE; }
"if"        { column += yyleng; return IF; }
"int"       { column += yyleng; return INT; }
"return"    { column += yyleng; return RETURN; }
"void"      { column += yyleng; return VOID; }
"while"     { column += yyleng; return WHILE; }

"+"         { column++; return PLUS; }
"-"         { column++; return MINUS; }
"*"         { column++; return TIMES; }
"/"         { column++; return DIV; }
"<="        { column += 2; return LEQ; }
"<"         { column++; return LT; }
">="        { column += 2; return GEQ; }
">"         { column++; return GT; }
"=="        { column += 2; return EQ; }
"!="        { column += 2; return NEQ; }
"="         { column++; return ASSIGN; }

","         { column++; return COMMA; }
";"         { column++; return SEMICOLON; }
"("         { column++; return LPAREN; }
")"         { column++; return RPAREN; }
"["         { column++; return LBRACKET; }
"]"         { column++; return RBRACKET; }
"{"         { column++; return LBRACE; }
"}"         { column++; return RBRACE; }

[0-9]+      { column += yyleng; yylval.integer = atoi(yytext); return NUMBER; }

[a-zA-Z][a-zA-Z0-9]* {
    column += yyleng;
    yylval.string = strdup(yytext);
    return ID;
}

[ \t]+      { column += yyleng; }

\n          { column = 0; }

"/*"([^*]|\*+[^*/])*\*+\/ { column += yyleng; }
"//"[^\n]* { column += yyleng; }

.           { 
    char error_msg[50];
    snprintf(error_msg, sizeof(error_msg), "Unknown character '\033[1;32m%c\033[0m'", yytext[0]);
    print_error(yylineno, column, "Lexical Error", error_msg);
}

%%

int yywrap() {
    return 1;
}
