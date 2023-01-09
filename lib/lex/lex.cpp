/* main lexer
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 21, 2013
*
*/
#include "system.h"
#include "lex.h"
#include "symbol.h"
#include "tools.h"

using namespace NS_LEX_CONSTANTS;
using namespace NS_LEX_PODS;

#define GET       -1
#define BACK      0
#define LOOKAHEAD 1

Lex::Lex(const char* filename)
{
    currentPos = new Position(filename);
    currentToken = new Token;
    currentToken->token_pos = currentPos;
    initLex(filename);
    start = pointer = buffer;
}

char Lex::getNextChar(int type)
{
    //TODO : 龙书上的词法分析器双缓冲技术
    if (pointer - start == LEX_BUFFER_SIZE
        && type == GET) { /* all context in buffer has been proceed, re-fill the buffer*/
                          //fillBuffer();
                          //pointer = buffer;
    }
    switch (type) {
        case GET:
            return *(pointer++);
        case BACK:
            return *(pointer--);
        case LOOKAHEAD:
            return *(pointer);
        default:
            assert(0);
    }
    return 0;
}

void Lex::skipBlank(void)
{
    char t;
sb:
    t = getNextChar(GET);
    switch (t) {
        case ' ':
        case '\t':
            currentPos->place++;
            break;
        case '\0':
            return;
        default:
            getNextChar(BACK);
            return;
    }
    goto sb;
}

void Lex::getBufferUntilSp(void)
{
    int i = 0;
    char c = getNextChar(GET);
    for (;; i++) {
        tmpBuffer[i] = c;
        c = getNextChar(GET);
        currentPos->place++;
        if (c == '.' || c == '+' || c == '-')
            continue;
        if (!isalnum(c))
            break;
        /* stop copying to buffer while reach a not-number or not-alpha*/
    }
    getNextChar(BACK);
    tmpBuffer[i + 1] = '\0';
}

void Lex::resynch(void)
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
    currentPos->line++;
    return;
}

int Lex::getAChar(char* pos, int* length)
{
    char tc = *pos;
    int result = 0;
    *length = 1;
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
                    return '\\';
                case '\"':
                    return '\"';
                case '\?':
                    return '\?';
                case '0':                         /* oct escape*/
                    if (isdigit(tc = *(pos + 2))) /* eg. \01 */
                    {
                        if (tc == '9') {
                            break;
                        }
                        (*length)++;
                        result = (result << 3) + tc - '0';
                        if (isdigit(tc = *(pos + 3))) /* eg. \002 */
                        {
                            if (tc == '9') {
                                break;
                            } else {
                                result = (result << 3) + tc - '0';
                                (*length)++;
                            }
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
    int reachE = 0, reachP = 0, reachL = 0, reachF = 0;
    double result;
    char* tp = numberBuffer;
    (*dest)->type = T_FLOAT_CON;
    (*dest)->numberType = NT_DB;
    for (; *tp; tp++) {
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
        errno = 0;
        result = strtod((char*)tmpBuffer, NULL);
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
            if (reachL == 1)
                (*dest)->numberType = NT_LD;
        }
    }

    (*dest)->val.d_value = result;
    return *dest;
parseFloatError:
    (*dest)->val.d_value = 1.0;
    return *dest;
}

NUMBER_TYPE& NS_LEX_CONSTANTS::operator|=(NUMBER_TYPE& nt, unsigned long mask)
{
    unsigned long ult = static_cast<unsigned long>(nt.type());
    ult = ult | mask;
    nt = static_cast<NUMBER_TYPE>(ult);
    return nt;
}

Number* Lex::tryParseDecNumber(char* numberBuffer)
{
    char* tp = numberBuffer;
    int reachU = 0, reachL = 0;
    int result = 0;
    Number* ret;
    int times = 0;
    ret = new Number();
    ret->numberType = NT_SI;
    ret->type = T_INT_CON;
    while (*(tp)) {
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
                    reachU = 1;
                } else {
                    lexerError(LEX_ERROR_ILL_SUFFIX, tp);
                }
                break;
            case 'l':
            case 'L':
                if (reachL == 0) /* 0x123l */
                {
                    ret->numberType |= NT_LONG_MASK;
                    reachU = 1;
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
        if (times >= 8)
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
    char* tp = numberBuffer;
    int reachU = 0, reachL = 0;
    int result = 0;
    Number* ret;
    int times = 0;
    ret = new Number();
    ret->numberType = NT_SI;
    ret->type = T_INT_CON;
    tp += 2; /* ignore the '0x' or '0X' character */
    while (*(tp)) {
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
                    reachU = 1;
                } else {
                    lexerError(LEX_ERROR_ILL_SUFFIX, tp);
                }
                break;
            case 'l':
            case 'L':
                if (reachL == 0) /* 0x123l */
                {
                    ret->numberType |= NT_LONG_MASK;
                    reachL = 1;
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
    char* tp = numberBuffer;
    int reachU = 0, reachL = 0;
    int result = 0;
    Number* ret;
    int times = 0;
    ret = new Number();
    ret->numberType = NT_SI;
    ret->type = T_INT_CON;
    while (*(tp)) {
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
                    reachU = 1;
                } else {
                    lexerError(LEX_ERROR_ILL_SUFFIX, tp);
                }
                break;
            case 'l':
            case 'L':
                if (reachL == 0) /* 0123l */
                {
                    ret->numberType |= NT_LONG_MASK;
                    reachU = 1;
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

int Lex::newNumber(char* numBuffer, int radix)
{
    Number* ret = NULL;
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
    //newTok = new Token(ret);
    currentToken->token_type = ret->type;
    currentToken->token_value.numVal = ret;
    switch (ret->type) {
        case T_FLOAT_CON:
            currentToken->token_value.numVal->val.d_value = ret->val.d_value;
            break;
        case T_INT_CON:
            currentToken->token_value.numVal->val.i_value = ret->val.i_value;
            break;
        default:
            assert(0);
    }

    return currentToken->token_type;
}

int Lex::number(char peek)
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
            if (isdigit(tmpBuffer[1])) /* oct numbers */
            {
                return newNumber(tmpBuffer, 8);
            } else if (tmpBuffer[1] == 'x' || tmpBuffer[1] == 'X') /*hex numbers*/
                return newNumber(tmpBuffer, 16);
            else if (tmpBuffer[1] == '\0') /* there is a 0 */
                return newNumber(tmpBuffer, 10);
        default: /* dec numbers */
            return newNumber(tmpBuffer, 10);
    }
}

int Lex::newIdentifier(const char* name, int name_length)
{
    char* newName = new char[sizeof(char) * name_length];
    currentToken->token_type = T_ID;
    currentToken->token_value.string = newName;
    strcpy(newName, name);
    //newTok = new Token(T_ID, newName);
    return 0;
}

int Lex::identifier(char* start)
{
    char t = *start;
    int length = 0;
    for (; isalnum(t) || t == '_'; t = getNextChar(GET)) {
        start[length++] = t;
    }
    *(start + length) = '\0';
    //TODO: Test whether here should has a getNextChar(BACK);
    getNextChar(BACK);
    currentPos->place += length;
    newIdentifier(tmpBuffer, (int)strlen(tmpBuffer) + 1);
    return T_ID;
}

NS_LEX_CONSTANTS::KEYWORD Lex::letter(char peek)
{
    char* p = tmpBuffer;
    switch (*p = peek) {
        case 'a': /* auto */
            *(++p) = getNextChar(GET);
            if (*p == 'u') {
                *(++p) = getNextChar(GET);
                if (*p == 't') {
                    *(++p) = getNextChar(GET);
                    if (*p == 'o' && !isalnum(getNextChar(LOOKAHEAD))) {
                        currentPos->place += 4;
                        return KEY_AUTO;
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
                        if (*p == 'k' && !isalnum(getNextChar(LOOKAHEAD))) {
                            currentPos->place += 5;
                            return KEY_BREAK;
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
                        if (*p == 'e' && !isalnum(getNextChar(LOOKAHEAD))) {
                            currentPos->place += 4;
                            return KEY_CASE;
                        }
                    }
                    break;
                case 'h': /* char */
                    *(++p) = getNextChar(GET);
                    if (*p == 'a') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'r' && !isalnum(getNextChar(LOOKAHEAD))) {
                            currentPos->place += 4;
                            return KEY_CHAR;
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
                                if (*p == 't' && !isalnum(getNextChar(LOOKAHEAD))) {
                                    currentPos->place += 5;
                                    return KEY_CONST;
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
                                            if (*p == 'e' && !isalnum(getNextChar(LOOKAHEAD))) {
                                                currentPos->place += 7;
                                                return KEY_CONTINUE;
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
                                    if (*p == 't' && !isalnum(getNextChar(LOOKAHEAD))) {
                                        currentPos->place += 6;
                                        return KEY_DEFAULT;
                                    }
                                }
                            }
                        }
                    }
                    break;
                case 'o':
                    if (!isalnum(*(++p) = getNextChar(GET))) {
                        currentPos->place += 2;
                        return KEY_DO;
                    } else if (*p == 'u') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'b') {
                            *(++p) = getNextChar(GET);
                            if (*p == 'l') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'e' && !isalnum(getNextChar(LOOKAHEAD))) {
                                    currentPos->place += 6;
                                    return KEY_DOUBLE;
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
                        if (*p == 'e' && !isalnum(getNextChar(LOOKAHEAD))) {
                            currentPos->place += 4;
                            return KEY_ELSE;
                        }
                    }
                    break;
                case 'n':
                    *(++p) = getNextChar(GET);
                    if (*p == 'u') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'm' && !isalnum(getNextChar(LOOKAHEAD))) {
                            currentPos->place += 4;
                            return KEY_ENUM;
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
                                if (*p == 'n' && !isalnum(getNextChar(LOOKAHEAD))) {
                                    currentPos->place += 6;
                                    getNextChar(BACK);
                                    return KEY_EXTERN;
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
                    if (*p == 'r' && !isalnum(getNextChar(LOOKAHEAD))) {
                        currentPos->place += 3;
                        return KEY_FOR;
                    }
                    break;
                case 'l':
                    *(++p) = getNextChar(GET);
                    if (*p == 'o') {
                        *(++p) = getNextChar(GET);
                        if (*p == 'a') {
                            *(++p) = getNextChar(GET);
                            if (*p == 't' && !isalnum(getNextChar(LOOKAHEAD))) {
                                currentPos->place += 5;
                                return KEY_FLOAT;
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
                    if (*p == 'o' && !isalnum(getNextChar(LOOKAHEAD))) {
                        currentPos->place += 4;
                        return KEY_GOTO;
                    }
                }
            }
            break;
        case 'i': /* if int*/
            *(++p) = getNextChar(GET);
            if (*p == 'f' && !isalnum(getNextChar(LOOKAHEAD))) {
                currentPos->place += 2;
                return KEY_IF;
            } else if (*p == 'n') {
                *(++p) = getNextChar(GET);
                if (*p == 't' && !isalnum(getNextChar(LOOKAHEAD))) {
                    currentPos->place += 3;
                    return KEY_INT;
                }
            }
            break;
        case 'l': /* long */
            *(++p) = getNextChar(GET);
            if (*p == 'o') {
                *(++p) = getNextChar(GET);
                if (*p == 'n') {
                    *(++p) = getNextChar(GET);
                    if (*p == 'g' && !isalnum(getNextChar(LOOKAHEAD))) {
                        currentPos->place += 4;
                        return KEY_LONG;
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
                                        if (*p == 'r' && !isalnum(getNextChar(LOOKAHEAD))) {
                                            currentPos->place += 8;
                                            return KEY_REGISTER;
                                        }
                                    }
                                }
                            }
                        }
                    case 't':
                        *(++p) = getNextChar(GET);
                        if (*p == 'u') {
                            *(++p) = getNextChar(GET);
                            if (*p == 'r') {
                                *(++p) = getNextChar(GET);
                                if (*p == 'n' && !isalnum(getNextChar(LOOKAHEAD))) {
                                    currentPos->place += 6;
                                    return KEY_RETURN;
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
                            if (*p == 't' && !isalnum(getNextChar(LOOKAHEAD))) {
                                currentPos->place += 5;
                                return KEY_SHORT;
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
                                    if (*p == 'd' && !isalnum(getNextChar(LOOKAHEAD))) {
                                        currentPos->place += 5;
                                        return KEY_SIGNED;
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
                                    if (*p == 'f' && !isalnum(getNextChar(LOOKAHEAD))) {
                                        currentPos->place += 5;
                                        return KEY_SIZEOF;
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
                                    if (*p == 'c' && !isalnum(getNextChar(LOOKAHEAD))) {
                                        currentPos->place += 6;
                                        return KEY_STATIC;
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
                                    if (*p == 't' && !isalnum(getNextChar(LOOKAHEAD))) {
                                        currentPos->place += 6;
                                        return KEY_STRUCT;
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
                                if (*p == 'h' && !isalnum(getNextChar(LOOKAHEAD))) {
                                    currentPos->place += 6;
                                    return KEY_SWITCH;
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
                                if (*p == 'f') {
                                    currentPos->place += 7;
                                    return KEY_TYPEDEF;
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
                            if (*p == 'n') {
                                currentPos->place += 5;
                                return KEY_UNION;
                            }
                        }
                        break;
                    case 's':
                        *(++p) = getNextChar(GET);
                        if (*p == 'i') {
                            *(++p) = getNextChar(GET);
                            if (*p == 'g') {
                                if (*p == 'n') {
                                    *(++p) = getNextChar(GET);
                                    if (*p == 'e') {
                                        *(++p) = getNextChar(GET);
                                        if (*p == 'd' && !isalnum(getNextChar(LOOKAHEAD))) {
                                            currentPos->place += 8;
                                            return KEY_UNSIGNED;
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
                        if (*p == 'd') {
                            currentPos->place += 4;
                            return KEY_KVOID;
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
                                        if (*p == 'e') {
                                            currentPos->place += 8;
                                            return KEY_VOLATILE;
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
                        if (*p == 'e') {
                            currentPos->place += 5;
                            return KEY_WHILE;
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
    identifier(p);
    return KEY_NONE;
}

int Lex::parseLetter(char peek)
/* try to get a keyword. */
{
    KEYWORD type = letter(peek);
    if (type == KEY_NONE)
        currentToken->token_type = T_ID;
    else if ((type & KEY_NONE) == KEY_NONE) {
        if (type == KEY_SIZEOF) {
            currentToken->token_type = T_OPERATOR;
            currentToken->token_value.op = OP_SIZEOF;
        } else {
            currentToken->token_type = T_KEY;
            currentToken->token_value.keyword = type;
        }
    }
    return currentToken->token_type;
}

int Lex::operators(char peek)
{
    int op = OP_NONE;
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
    currentPos->place += 1;
    return op;
length_2:
    currentPos->place += 2;
    return op;
length_3:
    currentPos->place += 3;
    return op;
}

int Lex::parseOperator(char peek)
{
    int type = operators(peek);
    if ((type & OP_NONE) == OP_NONE) {
        currentToken->token_type = T_OPERATOR;
        currentToken->token_value.op = static_cast<OP>(type);
    } else {
        assert(0);
    }
    return T_OPERATOR;
}

int Lex::parseString(void)
{
    char t;
    char* tp = tmpBuffer;
    char* ret;
    int lengthCount = 0;
    static int errTimes = 0;


stringParse:
    for (t = getNextChar(GET); t != '"'; t = getNextChar(GET), lengthCount++) {
        if (lengthCount == TMPBUFFER_SIZE) {
            if (errTimes == 0) {
                lexerError(LEX_ERROR_STRING_TOO_LONG, NULL);
            }
            errTimes = 1;
            tp = tmpBuffer;
            lengthCount = 0;
            continue;
        }
        if (t == '\n') {
            lexerError(LEX_ERROR_STRING_MEET_NEWLINE, NULL);
            //currentPos->line++;
            break;
        }
        if (t == EOF) {
            lexerError(2, NULL);
            fatalError("meet a EOF in a string constant.");
        }
        if (t == '\\') {
            if (getNextChar(LOOKAHEAD) == '\"') {
                *(tp++) = t;
                *(tp++) = getNextChar(GET);
                currentToken->token_pos->place++;
                continue;
            }
        }
        *(tp++) = t;
        currentToken->token_pos->place++;
    }
    *tp = '\0';
    skipBlank();
    if (getNextChar(LOOKAHEAD) == '\"') {
        getNextChar(GET); /*get the next " but ignore it.*/
        goto stringParse;
    }
    ret = new char[sizeof(char) * (tp - tmpBuffer + 1)];
    strcpy(ret, tmpBuffer);

    currentToken->token_type = T_STRING;
    currentToken->token_value.string = ret;
    //Token *newTok = new Token(T_STRING, ret);
    //memcpy(currentToken, newTok, sizeof(Token));
    return T_STRING;
}

int Lex::parseCharConstant(void)
{
    char t;
    char* tp = tmpBuffer;
    int lengthCount = 0;
    static int errTimes = 0;
    int result = 0;
    int charLength = 0;
    t = getNextChar(GET);
    for (; t != '\''; t = getNextChar(GET), lengthCount++) {
        if (lengthCount == TMPBUFFER_SIZE) {
            if (errTimes == 0) {
                lexerError(LEX_ERROR_CHAR_CON_TOO_LONG, NULL);
            }
            errTimes = 1;
            lengthCount = 0;
            tp = tmpBuffer;
        }
        if (t == '\n') {
            lexerError(LEX_ERROR_STRING_MEET_NEWLINE, NULL);
            //currentPos->line++;
            break;
        }
        if (t == EOF) {
            lexerError(2, NULL);
            fatalError("meet a EOF in a string constant.");
        }
        *tp++ = t;
        currentPos->place++;
    }
    *tp = '\0';
    lengthCount = 0;
    tp = tmpBuffer;
    for (; lengthCount <= 4 && *tp != '\0'; lengthCount++) {
        result = (result << 8) + getAChar(tp, &charLength);
        tp += charLength;
        if (lengthCount >= 4) {
            lexerError(LEX_ERROR_CHAR_CON_TOO_LONG, NULL);
            lengthCount = 0;
        }
    }
    if (lengthCount == 1) {
        currentToken->token_type = T_CHAR_CON;
        currentToken->token_value.numVal->type = T_CHAR_CON;
        currentToken->token_value.numVal->numberType = NT_CH;
        currentToken->token_value.numVal->val.i_value = result;
        return T_CHAR_CON;
    } else {
        currentToken->token_type = currentToken->token_value.numVal->type = T_INT_CON;
        currentToken->token_value.numVal->numberType = NT_UI;
        currentToken->token_value.numVal->val.i_value = result;
        return T_INT_CON;
    }
}

int Lex::getNextToken(void)
{
    for (;;) {
        skipBlank();
        tmpBuffer[0] = getNextChar(GET);

        switch (tmpBuffer[0]) {
            case '\n':
            case '\v':
            case '\f':
            case '\r':
                currentToken->token_pos->line++;
                currentToken->token_pos->place = 0;
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
    currentToken->token_type = T_NONE;
    return EOT;
}


NS_LEX_PODS::Token::Token(KEYWORD key)
{
    token_pos = new Position(*Lex::currentPos);
    token_type = T_KEY;
    token_value.keyword = key;
}

NS_LEX_PODS::Token::Token(NS_LEX_CONSTANTS::TYPE ty, const char* cc)
{
    token_pos = new Position(*Lex::currentPos);
    token_type = ty;
    token_value.string = cc;
}
NS_LEX_PODS::Token::Token(Number* n)
{
    token_pos = new Position(*Lex::currentPos);
    if (n->numberType == NT_FL || n->numberType == NT_DB)
        token_type = T_FLOAT_CON;
    else if (n->numberType == NT_CH)
        token_type = T_CHAR_CON;
    token_value.numVal = n;
}

NS_LEX_PODS::Token::Token(Token& tok) : token_type(tok.token_type)
{
    token_pos = new Position(*Lex::currentPos);
    token_value.numVal = tok.token_value.numVal;
}


unsigned long NS_LEX_TOOLS::strHash(const char* st)
{
    // Knuth[1973b]: TAOCP Vol2
    unsigned long hash = 1315423911ul;
    while (size_t ch = (size_t)*st++) {
        hash = ((hash << 13) ^ (hash >> 3)) ^ ch;
    }
    return hash;
}
