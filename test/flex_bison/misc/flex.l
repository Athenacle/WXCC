D            [0-9]
L            [a-zA-Z_]
H            [a-fA-F0-9]
E            ([Ee][+-]?{D}+)
P                       ([Pp][+-]?{D}+)
FS            (f|F|l|L)
IS                      ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))

%option c++
%option nounput
%option noinput
%option stack
%option noyywrap
%option yylineno
%option debug

%option yyclass="test::FlexBison_Lexer"


%{
// NOLINTBEGIN
#include "flex_bison/misc/flex_bison_lexer.h"
#include "location.hh"
#include <stdio.h>
#include "bison_parser.tab.h"
#include "flex_bison/misc/fb_fwd.h"


int column = 1;
int line = 1;
using namespace test;
using tt =  test::FlexBison_Parser::token::token_kind_type;


#define RETURN(kw) { yyret = tt::kw;  return test::FlexBison_Parser::make_##kw(test::location(&filename ,line,column));}

#define YY_USER_ACTION {}

%}



%x COMMENT

%%
"/*"                    { count(YYText()); BEGIN COMMENT; }
"//"[^\n]*              { count(YYText());  }
<COMMENT>"*/"           { count(YYText()); BEGIN INITIAL; }
<COMMENT>.              { count(YYText());}



"auto"                          { count(YYText()); RETURN(AUTO); }
"break"                         { count(YYText()); RETURN(BREAK); }
"case"                          { count(YYText()); RETURN(CASE); }
"char"                          { count(YYText()); RETURN(CHAR); }
"const"                         { count(YYText()); RETURN(CONST); }
"continue"                      { count(YYText()); RETURN(CONTINUE); }
"default"                       { count(YYText()); RETURN(DEFAULT); }
"do"                            { count(YYText()); RETURN(DO); }
"double"                        { count(YYText()); RETURN(DOUBLE); }
"else"                          { count(YYText()); RETURN(ELSE); }
"enum"                          { count(YYText()); RETURN(ENUM); }
"extern"                        { count(YYText()); RETURN(EXTERN); }
"float"                         { count(YYText()); RETURN(FLOAT); }
"for"                           { count(YYText()); RETURN(FOR); }
"goto"                          { count(YYText()); RETURN(GOTO); }
"if"                            { count(YYText()); RETURN(IF); }
"int"                           { count(YYText()); RETURN(INT); }
"long"                          { count(YYText()); RETURN(LONG); }
"register"                      { count(YYText()); RETURN(REGISTER); }
"restrict"                      { count(YYText()); RETURN(RESTRICT); }
"return"                        { count(YYText()); RETURN(RETURN); }
"short"                         { count(YYText()); RETURN(SHORT); }
"signed"                        { count(YYText()); RETURN(SIGNED); }
"sizeof"                        { count(YYText()); RETURN(SIZEOF); }
"static"                        { count(YYText()); RETURN(STATIC); }
"struct"                        { count(YYText()); RETURN(STRUCT); }
"switch"                        { count(YYText()); RETURN(SWITCH); }
"typedef"                       { count(YYText()); RETURN(TYPEDEF); }
"union"                         { count(YYText()); RETURN(UNION); }
"unsigned"                      { count(YYText()); RETURN(UNSIGNED); }
"void"                          { count(YYText()); RETURN(VOID); }
"volatile"                      { count(YYText()); RETURN(VOLATILE); }
"while"                         { count(YYText()); RETURN(WHILE); }

{L}({L}|{D})*                   { count(YYText()); RETURN(IDENTIFIER); }

0[xX]{H}+{IS}?                  { count(YYText()); parseHexNumber(YYText());  RETURN(CONSTANT); }
0[0-7]*{IS}?                    { count(YYText()); parseOctNumber(YYText());  RETURN(CONSTANT); }
[1-9]{D}*{IS}?                  { count(YYText()); parseDecNumber(YYText());  RETURN(CONSTANT); }
L?'(\\.|[^\\'\n])+'             { count(YYText()); RETURN(CONSTANT); }

{D}+{E}{FS}?                    { count(YYText()); RETURN(CONSTANT); }
{D}*"."{D}+{E}?{FS}?            { count(YYText()); RETURN(CONSTANT); }
{D}+"."{D}*{E}?{FS}?            { count(YYText()); RETURN(CONSTANT); }
0[xX]{H}+{P}{FS}?               { count(YYText()); RETURN(CONSTANT); }
0[xX]{H}*"."{H}+{P}?{FS}?       { count(YYText()); RETURN(CONSTANT); }
0[xX]{H}+"."{H}*{P}?{FS}?       { count(YYText()); RETURN(CONSTANT); }


L?\"(\\.|[^\\"\n])*\"           { count(YYText()); RETURN(STRING_LITERAL); }

"..."                           { count(YYText()); RETURN(ELLIPSIS); }
">>="                           { count(YYText()); RETURN(RIGHT_ASSIGN); }
"<<="                           { count(YYText()); RETURN(LEFT_ASSIGN); }
"+="                            { count(YYText()); RETURN(ADD_ASSIGN); }
"-="                            { count(YYText()); RETURN(SUB_ASSIGN); }
"*="                            { count(YYText()); RETURN(MUL_ASSIGN); }
"/="                            { count(YYText()); RETURN(DIV_ASSIGN); }
"%="                            { count(YYText()); RETURN(MOD_ASSIGN); }
"&="                            { count(YYText()); RETURN(AND_ASSIGN); }
"^="                            { count(YYText()); RETURN(XOR_ASSIGN); }
"|="                            { count(YYText()); RETURN(OR_ASSIGN); }
">>"                            { count(YYText()); RETURN(RIGHT_OP); }
"<<"                            { count(YYText()); RETURN(LEFT_OP); }
"++"                            { count(YYText()); RETURN(INC_OP); }
"--"                            { count(YYText()); RETURN(DEC_OP); }
"->"                            { count(YYText()); RETURN(PTR_OP); }
"&&"                            { count(YYText()); RETURN(AND_OP); }
"||"                            { count(YYText()); RETURN(OR_OP); }
"<="                            { count(YYText()); RETURN(LE_OP); }
">="                            { count(YYText()); RETURN(GE_OP); }
"=="                            { count(YYText()); RETURN(EQ_OP); }
"!="                            { count(YYText()); RETURN(NE_OP); }
";"                             { count(YYText()); RETURN(SEMICOLON); }
("{"|"<%")                      { count(YYText()); RETURN(LEFTBRACE); }
("}"|"%>")                      { count(YYText()); RETURN(RIGHTBRACE); }
","                             { count(YYText()); RETURN(COMMA); }
":"                             { count(YYText()); RETURN(COLON); }
"="                             { count(YYText()); RETURN(ASSIGN); }
"("                             { count(YYText()); RETURN(LEFTBRACK); }
")"                             { count(YYText()); RETURN(RIGHTBRACK); }
("["|"<:")                      { count(YYText()); RETURN(LEFTSQBRAC); }
("]"|":>")                      { count(YYText()); RETURN(RIGHTSQBRAC); }
"."                             { count(YYText()); RETURN(PERIOD); }
"&"                             { count(YYText()); RETURN(BITAND); }
"!"                             { count(YYText()); RETURN(LOGNOT); }
"~"                             { count(YYText()); RETURN(BITNOT); }
"-"                             { count(YYText()); RETURN(MINUS); }
"+"                             { count(YYText()); RETURN(PLUS); }
"*"                             { count(YYText()); RETURN(MULT); }
"/"                             { count(YYText()); RETURN(DIV); }
"%"                             { count(YYText()); RETURN(MOD); }
"<"                             { count(YYText()); RETURN(LT); }
">"                             { count(YYText()); RETURN(GT); }
"^"                             { count(YYText()); RETURN(BITXOR); }
"|"                             { count(YYText()); RETURN(BITOR); }
"?"                             { count(YYText()); RETURN(QUESTION); }

[ \t\v\n\f]                     { count(YYText()); }
.            { /* Add code to complain about unmatched characters */ }

<<EOF>>                                      {RETURN(EoF);}
%%

void count(const char* yytext)
{
    int i;
    yyytext = yytext;

    for (i = 0; yytext[i] != '\0'; i++)
        if (yytext[i] == '\n'){
            column = 0;
            line ++;
        } else {
            column++;
        }

}


int check_type(void)
{
/*
* pseudo code --- this is what it should check
*
*    if (yytext == type_name)
*        return TYPE_NAME;
*
*    return IDENTIFIER;
*/

/*
*    it actually will only return IDENTIFIER
*/

    return tt::IDENTIFIER;
}

// NOLINTEND
