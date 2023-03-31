%{
	#include <stdlib.h>
	using namespace std;
	#include "compiler.hpp"
	#include "grammar.hpp"
	extern int yylex();
%}
%option noyywrap
%option yylineno
%%
\[[^\]]*\]		;
[ \t\n]         ;
"PROCEDURE"     return PROCEDURE;
"IS"            return IS;
"VAR"           return VAR;
"BEGIN"         return MY_BEGIN;
"ENDIF"         return ENDIF;
"END"           return END;
"PROGRAM"       return PROGRAM;
"IF"            return IF;
"THEN"          return THEN;
"ELSE"          return ELSE;
"WHILE"         return WHILE;
"DO"            return DO;
"ENDWHILE"      return ENDWHILE;
"REPEAT"        return REPEAT;
"UNTIL"         return UNTIL;
"READ"          return READ;
"WRITE"         return WRITE;
[0-9]+        	yylval.text = strdup(yytext); return NUM;
[_a-z]+      	yylval.text = strdup(yytext); return IDENTIFIER;
":="            return SUBSTITUTE;
"!="            return NEQ;
">="            return GEQ;
"<="            return LEQ;
">"             return '>';
"<"             return '<';
"("             return '(';
")"             return ')';
"+"				return '+';
"-"				return '-';
"*"				return '*';
"/"				return '/';
"%"				return '%';
"="             return '=';
";"             return ';';
","				return ',';
%%