#ifndef TEST_FB_H_
#define TEST_FB_H_


#include <stdint.h>
#include <stdio.h>

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

extern const char *yyytext;
extern uint32_t yyret;

void parseDecNumber(const char *);
void parseHexNumber(const char *);
void parseOctNumber(const char *);
void parseFloatNumber(const char *);

void yyReset(void);
void count(const char *);

#endif
