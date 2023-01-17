#ifndef TEST_FB_H_
#define TEST_FB_H_

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


extern int yyleng;
extern int yylineno;
extern char *yytext;
extern FILE *yyin, *yyout;

int yylex_destroy(void);
int yyget_debug(void);
void yyset_debug(int debug_flag);
FILE *yyget_in(void);
int yylex(void);
void yyerror(char const *s);
void yyset_in(FILE *_in_str);
FILE *yyget_out(void);
void yyset_out(FILE *_out_str);
int yyget_leng(void);
char *yyget_text(void);
int yyget_lineno(void);
void yyset_lineno(int _line_number);
void comment(void);
int yywrap(void);
int yyparse(void);

extern int column;
extern int line;

struct FBNumber {
    union val {
        uint64_t i;
        long double f;
    } data;

    int is_short;
    int is_long;
    int is_longlong;
    int is_unsigned;

    int is_float;
};

extern struct FBNumber *yynumber;
extern const char *yyfilename;

void parseDecNumber(void);
void parseHexNumber(void);
void parseOctNumber(void);
void parseFloatNumber(void);

void yyReset();

#ifdef __cplusplus
}
#endif
#endif
