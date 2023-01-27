/* main lexer
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 21, 2013
*
*/


#include "system.h"

#include <cstring>

#include "lex/lexer.h"

#include "symbol.h"
#include "tools.h"

using namespace lex;
using namespace constants;
using namespace types;

#define GET       (-1)
#define BACK      (0)
#define LOOKAHEAD (1)

Lex::Lex(LexInputSource* s) : source(s) {}

char Lex::getNextChar(int type)
{
    LexInputSource::GET_TYPE t = LexInputSource::GET_TYPE::GET_CHAR;
    if (type == BACK) {
        t = LexInputSource::GET_TYPE::PUSH_BACK;
    } else if (type == LOOKAHEAD) {
        t = LexInputSource::GET_TYPE::LOOK_AHEAD;
    }
    return source->next(t);
}

void Lex::skipBlank()
{
    char t;
sb:
    t = getNextChar(GET);
    switch (t) {
        case ' ':
        case '\t':
            break;
        case '\0':
            return;
        default:
            getNextChar(BACK);
            return;
    }
    goto sb;
}

void Lex::getBufferUntilSp()
{
    int i  = 0;
    char c = getNextChar(GET);
    for (;; i++) {
        tmpBuffer[i] = c;
        c            = getNextChar(GET);
        if (c == '.' || c == '+' || c == '-') {
            continue;
        }
        if (isalnum(c) == 0) {
            break;
        }
        /* stop copying to buffer while reach a not-number or not-alpha*/
    }
    getNextChar(BACK);
    tmpBuffer[i + 1] = '\0';
}

void Lex::resynch()
/*re-synchnoize token:
	* #line 85 "lex.c"
	* #line 86
	* TODO: FINISH THIS FUNCTION.
	*/
{
    char t;
    int i = 0;
    for (; (t = getNextChar(GET)) != '\n'; i++) {
        tmpBuffer[i] = t;
    }
    tmpBuffer[i] = '\0';
}

int Lex::getAChar(const char* pos, int* length)
{
    char tc    = *pos;
    int result = 0;
    *length    = 1;
    switch (tc) {
        case '\\': /* escape char*/
            tc = *(pos + 1);
            (*length)++;
            switch (tc) {
                case 'a':
                    return '\a';
                case 'b':
                    return '\b';
                case 'f':
                    return '\f';
                case 'n':
                    return '\n';
                case 'r':
                    return '\r';
                case 't':
                    return '\t';
                case 'v':
                    return '\v';
                case '\\':
                    return '\\';
                case '\'':
                    return '\'';
                case '\"':
                    return '\"';
                case '\?':
                    return '\?';
                case '0':                             /* oct escape*/
                    if (tc = *(pos + 2); isdigit(tc)) /* eg. \01 */
                    {
                        if (tc == '9') {
                            break;
                        }
                        (*length)++;
                        result = (result << 3) + tc - '0';
                        if (tc = *(pos + 3); isdigit(tc) != 0) /* eg. \002 */
                        {
                            if (tc == '9') {
                                break;
                            }
                            result = (result << 3) + tc - '0';
                            (*length)++;
                        }
                        return result;
                    }
                    break;
                case 'x':
                case 'X':            /* hex escape */
                    tc = *(pos + 2); /* eg. \x1 */
                hex_par:
                    switch (tc) {
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                        case '0':
                            (*length)++;
                            result = (result << 4) + tc;
                            break;
                        case 'a':
                        case 'A':
                            (*length)++;
                            result = (result << 4) + 10;
                            break;
                        case 'b':
                        case 'B':
                            (*length)++;
                            result = (result << 4) + 11;
                            break;
                        case 'c':
                        case 'C':
                            (*length)++;
                            result = (result << 4) + 12;
                            break;
                        case 'd':
                        case 'D':
                            (*length)++;
                            result = (result << 4) + 13;
                            break;
                        case 'e':
                        case 'E':
                            (*length)++;
                            result = (result << 4) + 14;
                            break;
                        case 'f':
                        case 'F':
                            (*length)++;
                            result = (result << 4) + 15;
                            break;
                        default:
                            return result;
                    }
                    tc = *(pos + 3);
                    goto hex_par;
            }
            break;
        default:
            result = tc;
    }
    return result;
}

Number* Lex::tryParseFloatNumber(char* numberBuffer, Number** dest)
{
    /* try to parse a floating number. first check the vaildity, then call the
	* C library call strod()
	*
	*	FLOATING-CONSTANT:
	*		FRACTIONAL-CONSTANT [ EXPONENT-PART ] [ FLOATING-SUFFIX ]
	*
	*	FRACTIONAL-CONSTANT:
	*		[ DIGIT-SEQUENCE ]. DIGIT-SEQUENCE
	*
	*	EXPONENT-PART:
	*		E [ SIGN ] DIGIT-SEQUENCE
	*		e [ SIGN ] DIGIT-SEQUENCE
	*
	*	DIGIT-SEQUENCE:
	*		DIGIT { DIGIT }
	*
	*	FLOATING-SUFFIX:
	*		one of F f L l
	*/
    int reachE = 0;
    int reachP = 0;
    int reachL = 0;
    int reachF = 0;
    double result;
    char* tp            = numberBuffer;
    (*dest)->type       = T_FLOAT_CON;
    (*dest)->numberType = NT_DB;
    for (; *tp != 0; tp++) {
        switch (*tp) {
            case '.':

                if (reachE != 0) {
                    lexerError(LEX_ERROR_ILL_FLO_SUFFIX, tp);
                    goto parseFloatError;
                }
                if (reachP != 0) {
                    lexerError(LEX_ERROR_ILL_FLO_SUFFIX, tp);
                    goto parseFloatError;
                }
                reachP++;
                break;
            case 'E':
            case 'e':
                if (reachE != 0) {
                    lexerError(LEX_ERROR_ILL_FLO_SUFFIX, tp);
                    goto parseFloatError;
                }
                reachE++;
                break;
            case 'f':
            case 'F':
                reachF++;
                *tp = '\0';
                break;
            case 'l':
            case 'L':
                reachL++;
                *tp = '\0';
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
            case '+':
            case '-':
                break;
            default:
                lexerError(LEX_ERROR_ILL_FLO_SUFFIX, tp);
                (*dest)->numberType = NT_DB;
                goto parseFloatError;
        }
    }
    if (reachP > 1 || reachF > 1 || reachL > 1) {
        lexerError(LEX_ERROR_ILL_FLO_SUFFIX, tp);
        (*dest)->numberType = NT_DB;
        goto parseFloatError;
    } else {
        errno  = 0;
        result = strtod((char*)tmpBuffer, nullptr);
        if (errno == ERANGE) {
            lexerError(LEX_ERROR_FLOATING_OVERFLOW, tmpBuffer);
            goto parseFloatError;
        } else {
            if (reachF == 1)
            /* this is single precision value, check the range. */
            {
                if ((float)result > FLT_MAX + FLT_EPSILON
                    || (float)result < (FLT_MAX * -1) - FLT_EPSILON) {
                    lexerError(LEX_ERROR_FLOATING_OVERFLOW, tmpBuffer);
                    goto parseFloatError;
                }
                (*dest)->numberType = NT_FL;
            }
            if (reachL == 1) {
                (*dest)->numberType = NT_LD;
            }
        }
    }

    (*dest)->val.d_value = result;
    return *dest;
parseFloatError:
    (*dest)->val.d_value = 1.0;
    return *dest;
}

NumberType& constants::operator|=(NumberType& nt, unsigned long mask)
{
    auto ult = static_cast<unsigned long>(nt.type());
    ult      = ult | mask;
    nt       = static_cast<NumberType>(ult);
    return nt;
}

Number* Lex::tryParseDecNumber(char* numberBuffer)
{
    char* tp        = numberBuffer;
    int reachU      = 0;
    int reachL      = 0;
    uint64_t result = 0;
    Number* ret;
    int times       = 0;
    ret             = new Number();
    ret->numberType = NT_SI;
    ret->type       = T_INT_CON;
    while (*(tp) != 0) {
        switch (*tp) {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                result = (result * 10) + (*tp - '0');
                break;
            case 'u':
            case 'U':
                if (reachU == 0) /* 0x123u */
                {
                    ret->numberType |= NT_UNSIGNED_MASK;
                    reachU          = 1;
                } else {
                    lexerError(LEX_ERROR_ILL_SUFFIX, tp);
                }
                break;
            case 'l':
            case 'L':
                if (reachL == 0) /* 0x123l */
                {
                    ret->numberType |= NT_LONG_MASK;
                    reachU          = 1;
                } else {
                    lexerError(LEX_ERROR_ILL_SUFFIX, tp);
                }
                break;
            default:
                lexerError(LEX_ERROR_ILL_SUFFIX, tp);
                break;
            case '.':
            case 'e':
            case 'E':
                return tryParseFloatNumber(numberBuffer, &ret);
        }
        tp++;
        times++;
        if (times > 16)
        /* an integer or long integer is 32bit, 8 hex numbers
			* UINT_MAX = 0xffffffff
			*/
        {
            lexerError(LEX_ERROR_ICON_TOO_BIG, tmpBuffer);
            goto parHexNumFinish;
        }
    }
parHexNumFinish:
    ret->val.i_value = result;
    return ret;
}

Number* Lex::tryParseHexNumber(char* numberBuffer)
{
    char* tp   = numberBuffer;
    int reachU = 0;
    int reachL = 0;
    int result = 0;
    Number* ret;
    int times       = 0;
    ret             = new Number();
    ret->numberType = NT_SI;
    ret->type       = T_INT_CON;
    tp              += 2; /* ignore the '0x' or '0X' character */
    while (*(tp) != 0) {
        switch (*tp) {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                result = (result << 4) + (*tp - '0');
                break;
            case 'a':
            case 'A':
                result = (result << 4) + 10;
                break;
            case 'b':
            case 'B':
                result = (result << 4) + 11;
                break;
            case 'c':
            case 'C':
                result = (result << 4) + 12;
                break;
            case 'd':
            case 'D':
                result = (result << 4) + 13;
                break;
            case 'e':
            case 'E':
                result = (result << 4) + 14;
                break;
            case 'f':
            case 'F':
                result = (result << 4) + 15;
                break;
            case 'u':
            case 'U':
                if (reachU == 0) /* 0x123u */
                {
                    ret->numberType |= NT_UNSIGNED_MASK;
                    reachU          = 1;
                } else {
                    lexerError(LEX_ERROR_ILL_SUFFIX, tp);
                }
                break;
            case 'l':
            case 'L':
                if (reachL == 0) /* 0x123l */
                {
                    ret->numberType |= NT_LONG_MASK;
                    reachL          = 1;
                } else {
                    lexerError(LEX_ERROR_ILL_SUFFIX, tp);
                }
                break;
            case '\0':
                break;
            default:
                lexerError(LEX_ERROR_ILL_CHAR_IN_HEX, tp);
                goto parHexNumFinish;
        }
        tp++;
        if (times >= 8)
        /* an integer or long integer is 32bit, 8 hex numbers
			* UINT_MAX = 0xffffffff
			*/
        {
            lexerError(LEX_ERROR_ICON_TOO_BIG, tmpBuffer);
            goto parHexNumFinish;
        }
        times++;
    }
parHexNumFinish:
    ret->val.i_value = result;
    return ret;
}

Number* Lex::tryParseOctNumber(char* numberBuffer)
{
    char* tp   = numberBuffer;
    int reachU = 0;
    int reachL = 0;
    int result = 0;
    Number* ret;
    int times       = 0;
    ret             = new Number();
    ret->numberType = NT_SI;
    ret->type       = T_INT_CON;
    while (*(tp) != 0) {
        switch (*tp) {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '0':
                result = (result << 3) + (*tp - '0');
                break;
            case 'u':
            case 'U':
                if (reachU == 0) /* 0123u */
                {
                    ret->numberType |= NT_UNSIGNED_MASK;
                    reachU          = 1;
                } else {
                    lexerError(LEX_ERROR_ILL_SUFFIX, tp);
                }
                break;
            case 'l':
            case 'L':
                if (reachL == 0) /* 0123l */
                {
                    ret->numberType |= NT_LONG_MASK;
                    reachU          = 1;
                } else {
                    lexerError(LEX_ERROR_ILL_SUFFIX, tp);
                }
                break;
            default:
                lexerError(LEX_ERROR_ILL_CHAR_IN_OCT, tp);
                goto parOctNumFinish;
                break;
        }
        tp++;
        times++;
        if (times >= 11)
        /* an integer or long integer is 32bit, 11 oct numbers
			* UINT_MAX = 037777777777
			*/
        {
            lexerError(LEX_ERROR_ICON_TOO_BIG, tmpBuffer);
            goto parOctNumFinish;
        }
    }
parOctNumFinish:
    ret->val.i_value = result;
    return ret;
}

Token Lex::newNumber(char* numBuffer, int radix)
{
    Number* ret = nullptr;
    switch (radix) {
        case 10:
            ret = tryParseDecNumber(numBuffer);
            break;
        case 16:
            ret = tryParseHexNumber(numBuffer);
            break;
        case 8:
            ret = tryParseOctNumber(numBuffer);
            break;
        default:
            assert(0);
    }
    Token tok;
    tok.token_pos          = source->position();
    tok.token_type         = ret->type;
    tok.token_value.numVal = ret;
    switch (ret->type) {
        case T_FLOAT_CON:
            tok.token_value.numVal->val.d_value = ret->val.d_value;
            break;
        case T_INT_CON:
            tok.token_value.numVal->val.i_value = ret->val.i_value;
            break;
        default:
            assert(0);
    }

    return tok;
}

Token Lex::number(char peek)
/*
	*	CONSTANT:
	*		FLOATING-CONSTANT	|
	*		INTEGER-CONSTANT		|
	*		ENUMERATION-CONSTANT	|
	*		CHARACTER-CONSTANT
	*
	*	ENUMERATION-CONSTANT:
	*		IDENTIFIER
	*
	*	INTERGET-CONSTANT:
	*		DECIMAL-CONSTANT[INTEGER-SUFFIX]		|
	*		OCTAL-CONSTANT[INTEGER-SUFFIX]		|
	*		HEXADECIMAL-CONSTANT[INTEGER-SUFFIX]
	*
	*	INTERGER-SUFFIX:
	*		UNSIGNED-SUFFIX[LONG-SUFFIX]		|
	*		LONG-SUFFIX[UNSIGNED-SUFFIX]
	*
	*
	*	UNSIGNED-SUFFIX:
	*		U	|	u
	*	LONG-SUFFIX:
	*		L	|	l
	*
	*	DECIMAL-CONSTANT:
	*		NONZERO-DIGIT{DIGIT}
	*	NONZERO-DIGIT:
	*		one of 1 2 3 4 5 6 7 8 9
	*	DIGIT:
	*		one of 0 1 2 3 4 5 6 7 8 9
	*
	*	OCTAL-CONSTANT:
	*		0{OCTAL-DIGIT}
	*	OCTAL-DIGIT:
	*		one of 0 1 2 3 4 5 6 7
	*
	*	HEXADECIMAL-CONSTANT:
	*		(0X | 0x)HEXADECIMAL-DIGIT{HEXADECIMAL-DIGIT}
	*	HEXADECIMAL-DIGIT:
	*		one of 0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F
	*/
{
    getNextChar(BACK);
    getBufferUntilSp();
    switch (peek) {
        case '0':
            if (isdigit(tmpBuffer[1]) != 0) /* oct numbers */
            {
                return newNumber(tmpBuffer, 8);
            } else if (tmpBuffer[1] == 'x' || tmpBuffer[1] == 'X') { /*hex numbers*/
                return newNumber(tmpBuffer, 16);
            } else if (tmpBuffer[1] == '\0') { /* there is a 0 */
                return newNumber(tmpBuffer, 10);
            }
            FALLTHROUGH
        default: /* dec numbers */
            return newNumber(tmpBuffer, 10);
    }
}

Token Lex::newIdentifier(const char* name, int /*unused*/)
{
    // char* newName = new char[sizeof(char) * name_length];
    // strcpy(newName, name);
    Token tok(T_ID, name);
    tok.token_pos = source->position();
    return tok;
}

Token Lex::identifier(char* start)
{
    char t     = *start;
    int length = 0;
    for (; (isalnum(t) != 0) || t == '_'; t = getNextChar(GET)) {
        start[length++] = t;
    }
    *(start + length) = '\0';
    if (t != 0) {
        getNextChar(BACK);
    }
    return newIdentifier(tmpBuffer, (int)strlen(tmpBuffer) + 1);
}


#define CHECK_NEXT (!isalnum(getNextChar(LOOKAHEAD)) && (getNextChar(LOOKAHEAD) != '_'))

Token Lex::letter(char peek)
{
    char* p = tmpBuffer;
    Token tok;
    tok.token_type = T_KEY;

    switch (*p = peek) {
        case 'a': /* auto */
            *(++p) = getNextChar(GET);
            if (*p == 'u') {
                *(++p) = getNextChar(GET);
                if (*p == 't') {
                    *(++p) = getNextChar(GET);
                    if (*p == 'o' && CHECK_NEXT) {
                        tok.token_value.keyword = KEY_AUTO;
                        tok.token_pos           = source->position();
                        return tok;
                    }
                }
            }
            break;
        case 'b': /* break*/
            *(++p) = getNextChar(GET);
            if (*p == 'r') {
                *(++p) = getNextChar(GET);
                if (*p == 'e') {
                    *(++p) = getNextChar(GET);
                    if (*p == 'a') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'k' && CHECK_NEXT) {
                            tok.token_value.keyword = KEY_BREAK;
                            tok.token_pos           = source->position();
                            return tok;
                        }
                    }
                }
            }
            break;
        case 'c': /* case char const continue */
            *(++p) = getNextChar(GET);
            switch (*p) {
                case 'a': /* case */
                    *(++p) = getNextChar(GET);
                    if (*p == 's') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'e' && CHECK_NEXT) {
                            tok.token_value.keyword = KEY_CASE;
                            tok.token_pos           = source->position();
                            return tok;
                        }
                    }
                    break;
                case 'h': /* char */
                    *(++p) = getNextChar(GET);
                    if (*p == 'a') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'r' && CHECK_NEXT) {
                            tok.token_value.keyword = KEY_CHAR;
                            tok.token_pos           = source->position();
                            return tok;
                        }
                    }
                    break;
                case 'o': /* continue const*/
                    *(++p) = getNextChar(GET);
                    if (*p == 'n') {
                        *(++p) = getNextChar(GET);
                        switch (*p) {
                            case 's':
                                *(++p) = getNextChar(GET);
                                if (*p == 't' && CHECK_NEXT) {
                                    tok.token_value.keyword = KEY_CONST;
                                    tok.token_pos           = source->position();
                                    return tok;
                                }
                                break;
                            case 't':
                                *(++p) = getNextChar(GET);
                                if (*p == 'i') {
                                    *(++p) = getNextChar(GET);
                                    if (*p == 'n') {
                                        *(++p) = getNextChar(GET);
                                        if (*p == 'u') {
                                            *(++p) = getNextChar(GET);
                                            if (*p == 'e' && CHECK_NEXT) {
                                                tok.token_value.keyword = KEY_CONTINUE;
                                                tok.token_pos           = source->position();
                                                return tok;
                                            }
                                        }
                                    }
                                }
                        }
                    }
            }
            break;
        case 'd': /* default do double */
            *(++p) = getNextChar(GET);
            switch (*p) {
                case 'e':
                    *(++p) = getNextChar(GET);
                    if (*p == 'f') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'a') {
                            *(++p) = getNextChar(GET);
                            if (*p == 'u') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'l') {
                                    *(++p) = getNextChar(GET);
                                    if (*p == 't' && CHECK_NEXT) {
                                        tok.token_value.keyword = KEY_DEFAULT;
                                        tok.token_pos           = source->position();
                                        return tok;
                                    }
                                }
                            }
                        }
                    }
                    break;
                case 'o':
                    *(++p) = getNextChar(GET);
                    if (isalnum(*p) == 0) {
                        getNextChar(BACK);
                        tok.token_value.keyword = KEY_DO;
                        tok.token_pos           = source->position();
                        return tok;
                    }
                    if (*p == 'u') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'b') {
                            *(++p) = getNextChar(GET);
                            if (*p == 'l') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'e' && CHECK_NEXT) {
                                    tok.token_value.keyword = KEY_DOUBLE;
                                    tok.token_pos           = source->position();
                                    return tok;
                                }
                            }
                        }
                    }
            }
            break;
        case 'e': /* else enum extern*/
            *(++p) = getNextChar(GET);
            switch (*p) {
                case 'l':
                    *(++p) = getNextChar(GET);
                    if (*p == 's') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'e' && CHECK_NEXT) {
                            tok.token_value.keyword = KEY_ELSE;
                            tok.token_pos           = source->position();
                            return tok;
                        }
                    }
                    break;
                case 'n':
                    *(++p) = getNextChar(GET);
                    if (*p == 'u') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'm' && CHECK_NEXT) {
                            tok.token_value.keyword = KEY_ENUM;
                            tok.token_pos           = source->position();
                            return tok;
                        }
                    }
                    break;
                case 'x':
                    *(++p) = getNextChar(GET);
                    if (*p == 't') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'e') {
                            *(++p) = getNextChar(GET);
                            if (*p == 'r') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'n' && CHECK_NEXT) {
                                    tok.token_value.keyword = KEY_EXTERN;
                                    tok.token_pos           = source->position();
                                    return tok;
                                }
                            }
                        }
                    }
                    break;
            }
            break;
        case 'f': /* for float*/
            *(++p) = getNextChar(GET);
            switch (*p) {
                case 'o':
                    *(++p) = getNextChar(GET);
                    if (*p == 'r' && CHECK_NEXT) {
                        tok.token_value.keyword = KEY_FOR;
                        tok.token_pos           = source->position();
                        return tok;
                    }
                    break;
                case 'l':
                    *(++p) = getNextChar(GET);
                    if (*p == 'o') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'a') {
                            *(++p) = getNextChar(GET);
                            if (*p == 't' && CHECK_NEXT) {
                                tok.token_value.keyword = KEY_FLOAT;
                                tok.token_pos           = source->position();
                                return tok;
                            }
                        }
                    }
                    break;
            }
            break;
        case 'g': /* goto */
            *(++p) = getNextChar(GET);
            if (*p == 'o') {
                *(++p) = getNextChar(GET);
                if (*p == 't') {
                    *(++p) = getNextChar(GET);
                    if (*p == 'o' && CHECK_NEXT) {
                        tok.token_value.keyword = KEY_GOTO;
                        tok.token_pos           = source->position();
                        return tok;
                    }
                }
            }
            break;
        case 'i': /* if int*/
            *(++p) = getNextChar(GET);
            if (*p == 'f' && CHECK_NEXT) {
                tok.token_value.keyword = KEY_IF;
                tok.token_pos           = source->position();
                return tok;
            }
            if (*p == 'n') {
                *(++p) = getNextChar(GET);
                if (*p == 't' && CHECK_NEXT) {
                    tok.token_value.keyword = KEY_INT;
                    tok.token_pos           = source->position();
                    return tok;
                }
            }
            break;
        case 'l': /* long */
            *(++p) = getNextChar(GET);
            if (*p == 'o') {
                *(++p) = getNextChar(GET);
                if (*p == 'n') {
                    *(++p) = getNextChar(GET);
                    if (*p == 'g' && CHECK_NEXT) {
                        tok.token_value.keyword = KEY_LONG;
                        tok.token_pos           = source->position();
                        return tok;
                    }
                }
            }
            break;
        case 'r': /* register return */
            *(++p) = getNextChar(GET);
            if (*p == 'e') {
                *(++p) = getNextChar(GET);
                switch (*p) {
                    case 'g':
                        *(++p) = getNextChar(GET);
                        if (*p == 'i') {
                            *(++p) = getNextChar(GET);
                            if (*p == 's') {
                                *(++p) = getNextChar(GET);
                                if (*p == 't') {
                                    *(++p) = getNextChar(GET);
                                    if (*p == 'e') {
                                        *(++p) = getNextChar(GET);
                                        if (*p == 'r' && CHECK_NEXT) {
                                            tok.token_value.keyword = KEY_REGISTER;
                                            tok.token_pos           = source->position();
                                            return tok;
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    case 't':
                        *(++p) = getNextChar(GET);
                        if (*p == 'u') {
                            *(++p) = getNextChar(GET);
                            if (*p == 'r') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'n' && CHECK_NEXT) {
                                    tok.token_value.keyword = KEY_RETURN;
                                    tok.token_pos           = source->position();
                                    return tok;
                                }
                            }
                        }
                }
            }
            break;
        case 's': /* short signed sizeof static struct switch*/
            *(++p) = getNextChar(GET);
            switch (*p) {
                case 'h':
                    *(++p) = getNextChar(GET);
                    if (*p == 'o') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'r') {
                            *(++p) = getNextChar(GET);
                            if (*p == 't' && CHECK_NEXT) {
                                tok.token_value.keyword = KEY_SHORT;
                                tok.token_pos           = source->position();
                                return tok;
                            }
                        }
                    }
                    break;
                case 'i':
                    *(++p) = getNextChar(GET);
                    switch (*p) {
                        case 'g':
                            *(++p) = getNextChar(GET);
                            if (*p == 'n') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'e') {
                                    *(++p) = getNextChar(GET);
                                    if (*p == 'd' && CHECK_NEXT) {
                                        tok.token_value.keyword = KEY_SIGNED;
                                        tok.token_pos           = source->position();
                                        return tok;
                                    }
                                }
                            }
                            break;
                        case 'z':
                            *(++p) = getNextChar(GET);
                            if (*p == 'e') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'o') {
                                    *(++p) = getNextChar(GET);
                                    if (*p == 'f' && CHECK_NEXT) {
                                        tok.token_value.keyword = KEY_SIZEOF;
                                        tok.token_pos           = source->position();
                                        return tok;
                                    }
                                }
                            }
                            break;
                    }
                    break;
                case 't':
                    *(++p) = getNextChar(GET);
                    switch (*p) {
                        case 'a':
                            *(++p) = getNextChar(GET);
                            if (*p == 't') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'i') {
                                    *(++p) = getNextChar(GET);
                                    if (*p == 'c' && CHECK_NEXT) {
                                        tok.token_value.keyword = KEY_STATIC;
                                        tok.token_pos           = source->position();
                                        return tok;
                                    }
                                }
                            }
                            break;
                        case 'r':
                            *(++p) = getNextChar(GET);
                            if (*p == 'u') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'c') {
                                    *(++p) = getNextChar(GET);
                                    if (*p == 't' && CHECK_NEXT) {
                                        tok.token_value.keyword = KEY_STRUCT;
                                        tok.token_pos           = source->position();
                                        return tok;
                                    }
                                }
                            }

                            break;
                    }
                    break;
                case 'w':
                    *(++p) = getNextChar(GET);
                    if (*p == 'i') {
                        *(++p) = getNextChar(GET);
                        if (*p == 't') {
                            *(++p) = getNextChar(GET);
                            if (*p == 'c') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'h' && CHECK_NEXT) {
                                    tok.token_value.keyword = KEY_SWITCH;
                                    tok.token_pos           = source->position();
                                    return tok;
                                }
                            }
                        }
                    }
            }
            break;
        case 't': /* typedef*/
            *(++p) = getNextChar(GET);
            if (*p == 'y') {
                *(++p) = getNextChar(GET);
                if (*p == 'p') {
                    *(++p) = getNextChar(GET);
                    if (*p == 'e') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'd') {
                            *(++p) = getNextChar(GET);
                            if (*p == 'e') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'f' && CHECK_NEXT) {
                                    tok.token_value.keyword = KEY_TYPEDEF;
                                    tok.token_pos           = source->position();
                                    return tok;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 'u': /* union unsigned*/
            *(++p) = getNextChar(GET);
            if (*p == 'n') {
                *(++p) = getNextChar(GET);
                switch (*p) {
                    case 'i':
                        *(++p) = getNextChar(GET);
                        if (*p == 'o') {
                            *(++p) = getNextChar(GET);
                            if (*p == 'n' && CHECK_NEXT) {
                                tok.token_value.keyword = KEY_UNION;
                                tok.token_pos           = source->position();
                                return tok;
                            }
                        }
                        break;
                    case 's':
                        *(++p) = getNextChar(GET);
                        if (*p == 'i') {
                            *(++p) = getNextChar(GET);
                            if (*p == 'g') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'n') {
                                    *(++p) = getNextChar(GET);
                                    if (*p == 'e') {
                                        *(++p) = getNextChar(GET);
                                        if (*p == 'd' && CHECK_NEXT) {
                                            tok.token_value.keyword = KEY_UNSIGNED;
                                            tok.token_pos           = source->position();
                                            return tok;
                                        }
                                    }
                                }
                            }
                        }
                        break;
                }
            }
            break;
        case 'v': /* void volatile */
            *(++p) = getNextChar(GET);
            if (*p == 'o') {
                *(++p) = getNextChar(GET);
                switch (*p) {
                    case 'i': {
                        *(++p) = getNextChar(GET);
                        if (*p == 'd' && CHECK_NEXT) {
                            tok.token_value.keyword = KEY_KVOID;
                            tok.token_pos           = source->position();
                            return tok;
                        }
                        break;
                    }
                    case 'l': {
                        *(++p) = getNextChar(GET);
                        if (*p == 'a') {
                            *(++p) = getNextChar(GET);
                            if (*p == 't') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'i') {
                                    *(++p) = getNextChar(GET);
                                    if (*p == 'l') {
                                        *(++p) = getNextChar(GET);
                                        if (*p == 'e' && CHECK_NEXT) {
                                            tok.token_value.keyword = KEY_VOLATILE;
                                            tok.token_pos           = source->position();
                                            return tok;
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
            break;
        case 'w': /* while */
            *(++p) = getNextChar(GET);
            if (*p == 'h') {
                *(++p) = getNextChar(GET);
                if (*p == 'i') {
                    *(++p) = getNextChar(GET);
                    if (*p == 'l') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'e' && CHECK_NEXT) {
                            tok.token_value.keyword = KEY_WHILE;
                            tok.token_pos           = source->position();
                            return tok;
                        }
                    }
                }
            }
            break;
        default:
            /* cannot reach default branch*/
            assert(0);
    }
    /* when reach here, means there is a indentifier which begin with a keyword
	* prefix.		e.g.:	 inta	ina	ia
	* p is the pointer to the first letter in the indentifier which is not shared
	* with a same prefix of a keyword. but the tmpBuffer cotains the whole word.
	*		inta\0
	*     tmpBuffer^  ^p
	*/
    return identifier(p);
    //return KEY_NONE;
}

Token Lex::parseLetter(char peek)
/* try to get a keyword. */
{
    auto tok = letter(peek);
    if (tok.token_type == T_KEY) {
        if (tok.token_value.keyword == KEY_SIZEOF) {
            tok.token_type     = T_OPERATOR;
            tok.token_value.op = OP_SIZEOF;
        }
    }
    return tok;
}

Token Lex::operators(char peek)
{
    constants::OP op = OP_NONE;
    switch (peek) {
        case '!': {
            peek = getNextChar(GET);
            switch (peek) {
                case '=':
                    op = OP_NE;
                    goto length_2;
                default:
                    op = OP_LOGNOT;
                    getNextChar(BACK);
                    goto length_1;
            }
        }

        case '%': {
            peek = getNextChar(GET);
            switch (peek) {
                case '=':
                    op = OP_ALMOD;
                    goto length_2;
                default:
                    op = OP_MOD;
                    getNextChar(BACK);
                    goto length_1;
            }
        }
        case '^': {
            peek = getNextChar(GET);
            switch (peek) {
                case '=':
                    op = OP_ALBITXOR;
                    goto length_2;
                default:
                    op = OP_BITXOR;
                    getNextChar(BACK);
                    goto length_1;
            }
        }
        case '&': {
            peek = getNextChar(GET);
            switch (peek) {
                case '&':
                    op = OP_LOGAND;
                    goto length_2;
                case '=':
                    op = OP_ALBITAND;
                    goto length_2;
                default:
                    op = OP_BITAND;
                    getNextChar(BACK);
                    goto length_1;
            }
        }
        case '*': {
            peek = getNextChar(GET);
            switch (peek) {
                case '=':
                    op = OP_ALMULT;
                    goto length_2;
                default:
                    op = OP_MULT;
                    getNextChar(BACK);
                    goto length_1;
            }
            break;
        }
        case '+': {
            peek = getNextChar(GET);
            switch (peek) {
                case '=':
                    op = OP_ALPLUS;
                    goto length_2;
                case '+':
                    op = OP_INC;
                    goto length_2;
                default:
                    op = OP_PLUS;
                    getNextChar(BACK);
                    goto length_1;
            }
            break;
        }
        case '-': {
            peek = getNextChar(GET);
            switch (peek) {
                case '=':
                    op = OP_ALMINUS;
                    goto length_2;
                case '-':
                    op = OP_DEC;
                    goto length_2;
                case '>':
                    op = OP_POINTER;
                    goto length_2;
                default:
                    op = OP_MINUS;
                    getNextChar(BACK);
                    goto length_1;
            }
            break;
        }
        case '|': {
            peek = getNextChar(GET);
            switch (peek) {
                case '=':
                    op = OP_ALBITOR;
                    goto length_2;
                case '|':
                    op = OP_LOGOR;
                    goto length_2;
                default:
                    op = OP_BITOR;
                    getNextChar(BACK);
                    goto length_1;
            }
        }
        case '~': {
            op = OP_BITNOT;
            goto length_1;
        }
        case '.': {
            peek = getNextChar(GET);
            switch (peek) {
                case '.':
                    peek = getNextChar(GET);
                    if (peek == '.') {
                        op = OP_EPPIPSIS;
                        goto length_2;
                    }
                    /* fall through*/
                default:
                    op = OP_PERIOD;
                    getNextChar(BACK);
                    goto length_1;
            }
            break;
        }
        case '<': {
            peek = getNextChar(GET);
            switch (peek) {
                case '=':
                    op = OP_LE;
                    goto length_2;
                case '<': {
                    peek = getNextChar(GET);
                    if (peek == '=') {
                        op = OP_ALLEFTSHIFT;
                        goto length_2;
                    } else {
                        getNextChar(BACK);
                        op = OP_LEFTSHIFT;
                        goto length_3;
                    }
                }
                default:
                    op = OP_LT;
                    goto length_1;
            }
        }
        case '>': {
            peek = getNextChar(GET);
            switch (peek) {
                case '=':
                    op = OP_GE;
                    goto length_2;
                case '>': {
                    peek = getNextChar(GET);
                    if (peek == '=') {
                        op = OP_ALRIGHTSHIFT;
                        goto length_2;
                    } else {
                        getNextChar(BACK);
                        op = OP_RIGHTSHIFT;
                        goto length_3;
                    }
                }
                default:
                    op = OP_GT;
                    goto length_1;
            }
        }
        case '/': {
            peek = getNextChar(GET);
            if (peek == '=') {
                op = OP_ALDIV;
                goto length_2;
            } else {
                op = OP_DIV;
                goto length_1;
            }
        }
        case '=': {
            peek = getNextChar(GET);
            if (peek == '=') {
                op = OP_EQ;
                goto length_1;
            } else {
                op = OP_ASSIGN;
                goto length_1;
            }
        }
        case '?': {
            op = OP_QUESTION;
            goto length_1;
        }
        case '(': {
            op = OP_LEFTBRACK;
            goto length_1;
        }
        case ')': {
            op = OP_RIGHTBRACK;
            goto length_1;
        }
        case '[': {
            op = OP_LEFTSQBRAC;
            goto length_1;
        }
        case ']': {
            op = OP_RIGHTSQBRAC;
            goto length_1;
        }
        case '{': {
            op = OP_LEFTBRACE;
            goto length_1;
        }
        case '}': {
            op = OP_RIGHTBRACE;
            goto length_1;
        }
        case ',': {
            op = OP_COMMA;
            goto length_1;
        }
        case ';': {
            op = OP_SEMICOLON;
            goto length_1;
        }
        case ':': {
            op = OP_COLON;
            goto length_1;
        }
        default: {
            lexerError(LEX_ERROR_ILL_TOKEN, &peek);
        }
    }

length_1:
length_2:
length_3:
    Token token(op);
    token.token_pos = source->position();
    return token;
}

Token Lex::parseOperator(char peek)
{
    return operators(peek);
}

Token Lex::parseString()
{
    char t;
    char* tp = tmpBuffer;
    char* ret;
    int lengthCount     = 0;
    static int errTimes = 0;


stringParse:
    for (t = getNextChar(GET); t != '"'; t = getNextChar(GET), lengthCount++) {
        if (lengthCount == TMPBUFFER_SIZE) {
            if (errTimes == 0) {
                lexerError(LEX_ERROR_STRING_TOO_LONG, nullptr);
            }
            errTimes    = 1;
            tp          = tmpBuffer;
            lengthCount = 0;
            continue;
        }
        if (t == '\n') {
            lexerError(LEX_ERROR_STRING_MEET_NEWLINE, nullptr);
            break;
        }
        if (t == EOF) {
            lexerError(2, nullptr);
            fatalError("meet a EOF in a string constant.");
        }
        if (t == '\\') {
            if (getNextChar(LOOKAHEAD) == '\"') {
                *(tp++) = t;
                *(tp++) = getNextChar(GET);
                continue;
            }
        }
        *(tp++) = t;
        // currentToken->token_pos->place++;
    }
    *tp = '\0';
    skipBlank();
    if (getNextChar(LOOKAHEAD) == '\"') {
        getNextChar(GET); /*get the next " but ignore it.*/
        goto stringParse;
    }
    ret = new char[sizeof(char) * (tp - tmpBuffer + 1)];
    strcpy(ret, tmpBuffer);

    Token tok(T_STRING, ret);
    tok.token_pos = source->position();
    delete[] ret;

    return tok;
}

Token Lex::parseCharConstant()
{
    char t;
    char* tp            = tmpBuffer;
    int lengthCount     = 0;
    static int errTimes = 0;
    int result          = 0;
    int charLength      = 0;
    t                   = getNextChar(GET);
    for (; t != '\''; t = getNextChar(GET), lengthCount++) {
        if (lengthCount == TMPBUFFER_SIZE) {
            if (errTimes == 0) {
                lexerError(LEX_ERROR_CHAR_CON_TOO_LONG, nullptr);
            }
            errTimes    = 1;
            lengthCount = 0;
            tp          = tmpBuffer;
        }
        if (t == '\n') {
            lexerError(LEX_ERROR_STRING_MEET_NEWLINE, nullptr);
            break;
        }
        if (t == EOF) {
            lexerError(2, nullptr);
            fatalError("meet a EOF in a string constant.");
        }
        *tp++ = t;
    }
    *tp         = '\0';
    lengthCount = 0;
    tp          = tmpBuffer;
    for (; lengthCount <= 4 && *tp != '\0'; lengthCount++) {
        result = (result << 8) + getAChar(tp, &charLength);
        tp     += charLength;
        if (lengthCount >= 4) {
            lexerError(LEX_ERROR_CHAR_CON_TOO_LONG, nullptr);
            lengthCount = 0;
        }
    }
    Token tok;
    if (lengthCount == 1) {
        tok.token_type                      = T_CHAR_CON;
        tok.token_value.numVal->type        = T_CHAR_CON;
        tok.token_value.numVal->numberType  = NT_CH;
        tok.token_value.numVal->val.i_value = result;
    } else {
        tok.token_type = tok.token_value.numVal->type = T_INT_CON;
        tok.token_value.numVal->numberType            = NT_UI;
        tok.token_value.numVal->val.i_value           = result;
    }
    tok.token_pos = source->position();
    return tok;
}

Token Lex::getNextToken()
{
    for (;;) {
        skipBlank();
        tmpBuffer[0] = getNextChar(GET);

        switch (tmpBuffer[0]) {
            case '\n':
            case '\v':
            case '\f':
            case '\r':
                break;
            case '#':
                resynch();
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                return number(tmpBuffer[0]);
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'i':
            case 'l':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
                return parseLetter(tmpBuffer[0]);
            case 'A':
            case 'B':
            case 'C':
            case 'E':
            case 'D':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case 'h':
            case 'j':
            case 'k':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'x':
            case 'y':
            case 'z':
            case '_':
                return identifier(tmpBuffer);
            case '"':
                return parseString(); /* ignore peek, it is a " */
            case EOF:
                break; /* lex finished*/
            case '\0':
                goto end;
            case ' ':
                break;
            case '\'':
                return parseCharConstant(); /* ignore peek, it is a ' */
            default:
                return parseOperator(tmpBuffer[0]);
        }
    }
end:
    Token tok;
    return tok;
}

Lex& Lex::operator=(Lex&& l) noexcept
{
    source = l.source;
    memcpy(tmpBuffer, l.tmpBuffer, TMPBUFFER_SIZE);
    lexWarningCount = l.lexWarningCount;
    lexErrorCount   = l.lexErrorCount;
    return *this;
}
