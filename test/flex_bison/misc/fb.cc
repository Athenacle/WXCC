#include <algorithm>
#include <cctype>
#include <cstring>
#include <map>

#include "fb_fwd.h"
#include "flex_bison.h"
#include "tools.h"

using namespace lex;
using namespace lex ::constants;
using namespace lex ::types;
using namespace test;

#define MAP_ADD(tt, v, map)                                \
    do {                                                   \
        map.emplace(test::FlexBison_Parser::token::tt, v); \
    } while (false)

namespace
{
    void buildOpMap(std::map<int, OP>& m)
    {
        m.clear();
        MAP_ADD(PTR_OP, OP_POINTER, m);
        MAP_ADD(INC_OP, OP_INC, m);
        MAP_ADD(DEC_OP, OP_DEC, m);
        MAP_ADD(LEFT_OP, OP_LEFTSHIFT, m);
        MAP_ADD(RIGHT_OP, OP_RIGHTSHIFT, m);
        MAP_ADD(LE_OP, OP_LE, m);
        MAP_ADD(GE_OP, OP_GE, m);
        MAP_ADD(EQ_OP, OP_EQ, m);
        MAP_ADD(NE_OP, OP_NE, m);
        MAP_ADD(AND_OP, OP_LOGAND, m);
        MAP_ADD(OR_OP, OP_LOGOR, m);
        MAP_ADD(MUL_ASSIGN, OP_ALMULT, m);
        MAP_ADD(DIV_ASSIGN, OP_ALDIV, m);
        MAP_ADD(MOD_ASSIGN, OP_ALMOD, m);
        MAP_ADD(ADD_ASSIGN, OP_ALPLUS, m);
        MAP_ADD(SUB_ASSIGN, OP_ALMINUS, m);
        MAP_ADD(LEFT_ASSIGN, OP_ALLEFTSHIFT, m);
        MAP_ADD(RIGHT_ASSIGN, OP_ALRIGHTSHIFT, m);
        MAP_ADD(AND_ASSIGN, OP_ALBITAND, m);
        MAP_ADD(XOR_ASSIGN, OP_ALBITXOR, m);
        MAP_ADD(OR_ASSIGN, OP_ALBITOR, m);
        MAP_ADD(SIZEOF, OP_SIZEOF, m);
    }

    void buildKeywordMap(std::map<int, KEYWORD>& m)
    {
        m.clear();
        MAP_ADD(TYPEDEF, KEY_TYPEDEF, m);
        MAP_ADD(EXTERN, KEY_EXTERN, m);
        MAP_ADD(STATIC, KEY_STATIC, m);
        MAP_ADD(AUTO, KEY_AUTO, m);
        MAP_ADD(REGISTER, KEY_REGISTER, m);
        MAP_ADD(CHAR, KEY_CHAR, m);
        MAP_ADD(SHORT, KEY_SHORT, m);
        MAP_ADD(INT, KEY_INT, m);
        MAP_ADD(LONG, KEY_LONG, m);
        MAP_ADD(SIGNED, KEY_SIGNED, m);
        MAP_ADD(UNSIGNED, KEY_UNSIGNED, m);
        MAP_ADD(FLOAT, KEY_FLOAT, m);
        MAP_ADD(DOUBLE, KEY_DOUBLE, m);
        MAP_ADD(CONST, KEY_CONST, m);
        MAP_ADD(VOLATILE, KEY_VOLATILE, m);
        MAP_ADD(VOID, KEY_KVOID, m);
        MAP_ADD(STRUCT, KEY_STRUCT, m);
        MAP_ADD(UNION, KEY_UNION, m);
        MAP_ADD(ENUM, KEY_ENUM, m);
        MAP_ADD(CASE, KEY_CASE, m);
        MAP_ADD(DEFAULT, KEY_DEFAULT, m);
        MAP_ADD(IF, KEY_IF, m);
        MAP_ADD(ELSE, KEY_ELSE, m);
        MAP_ADD(SWITCH, KEY_SWITCH, m);
        MAP_ADD(WHILE, KEY_WHILE, m);
        MAP_ADD(DO, KEY_DO, m);
        MAP_ADD(FOR, KEY_FOR, m);
        MAP_ADD(GOTO, KEY_GOTO, m);
        MAP_ADD(CONTINUE, KEY_CONTINUE, m);
        MAP_ADD(BREAK, KEY_BREAK, m);
        MAP_ADD(RETURN, KEY_RETURN, m);
    }

    void parseNumber(const char* text)
    {
        if (yynumber != nullptr) {
            delete yynumber;
        }

        yynumber = new FBNumber;
        memset(yynumber, 0, sizeof(struct FBNumber));
        yynumber->is_float = 0;

        auto ptr = text;
        while (isxdigit(*ptr) || *ptr == 'x' || *ptr == 'X') {
            ptr++;
        }
        size_t pos = 0;
        std::string n(text, ptr);
        auto ull = std::stoull(n, &pos, 0);

        std::string postfix(ptr + 1);
        std::transform(postfix.begin(), postfix.end(), postfix.begin(), [](char c) {
            return std::tolower(c);
        });
        std::sort(postfix.begin(), postfix.end());

        if (postfix.find("ll")) {
            yynumber->is_longlong = 1;
        } else if (postfix.find("l")) {
            yynumber->is_long = 1;
        }
        if (postfix.find("u")) {
            yynumber->is_unsigned = 1;
        }

        yynumber->data.i = ull;
    }

    static std::shared_ptr<std::string> yyf;


    std::shared_ptr<std::string> yyfile()
    {
        if (!yyf) {
            yyf = std::make_shared<std::string>(yyfilename);
        }
        return yyf;
    }
}  // namespace


FBNumber* yynumber = nullptr;
const char* yyfilename = nullptr;
const char* yyytext = nullptr;
uint32_t yyret = 0;

void yyReset(void)
{
    line = column = 1;
    yyf.reset();
}

void parseDecNumber(const char* yytext)
{
    parseNumber(yytext);
}

void parseHexNumber(const char* yytext)
{
    parseNumber(yytext);
}
void parseOctNumber(const char* yytext)
{
    parseNumber(yytext);
}
void parseFloatNumber(const char*)
{
    if (yynumber != nullptr) {
        delete yynumber;
    }
    yynumber = new FBNumber;
    memset(yynumber, 0, sizeof(struct FBNumber));
    yynumber->is_float = 1;

    std::string s(yyytext);
    auto d = std::stold(s);
    std::transform(s.begin(), s.end(), s.begin(), [](char c) { return std::tolower(c); });

    if (s.find("f")) {
        yynumber->is_short = 1;
    } else if (s.find("l")) {
        yynumber->is_long = 1;
    }

    yynumber->data.f = d;
}

Token flex(FlexBison_Lexer* ll)
{
    static std::map<int, KEYWORD> keys;
    static std::map<int, OP> ops;
    static std::shared_ptr<std::string> filename;

    if (keys.size() == 0 || ops.size() == 0) {
        buildKeywordMap(keys);
        buildOpMap(ops);
    }
    Token tok;
    ll->next();
    auto code = yyret;

    if (code == FlexBison_Parser::token::token_kind_type::CONSTANT) {
        assert(yynumber != nullptr);
        tok.token_type = yynumber->is_float ? T_FLOAT_CON : T_INT_CON;
        tok.token_value.numVal = new Number();
        auto& num = *tok.token_value.numVal;
        if (yynumber->is_long) {
            num.numberType.setLong();
        } else if (yynumber->is_longlong) {
            num.numberType.setLongLong();
        }
        if (yynumber->is_unsigned) {
            num.numberType.setUnsigned();
        } else {
            num.numberType.setSigned();
        }

        if (yynumber->is_short) {
            num.numberType.setShort();
        }

        if (yynumber->is_float) {
            num.type = tok.token_type = T_FLOAT_CON;
            num.val.d_value = yynumber->data.f;
        } else {
            num.type = tok.token_type = T_INT_CON;
            num.val.i_value = yynumber->data.i;
        }
        delete yynumber;
        yynumber = nullptr;

    } else if (code == FlexBison_Parser::token::token_kind_type::STRING_LITERAL) {
        assert(*yyytext == '"');

        tok.token_type = T_STRING;
        auto result = utils::strdup(yyytext + 1);
        auto ptr = result;
        while (*ptr)
            ptr += 1;
        ptr--;
        assert(*ptr == '"');
        *ptr = 0;
        tok.token_value.string = result;
    } else if (code == FlexBison_Parser::token::token_kind_type::IDENTIFIER) {
        tok.token_type = T_ID;
        tok.token_value.id_name = utils::strdup(yyytext);
    } else if (auto f = keys.find(code); f != keys.end()) {
        tok.token_type = T_KEY;
        tok.token_value.keyword = f->second;
    } else if (auto f = ops.find(code); f != ops.end()) {
        tok.token_type = T_OPERATOR;
        tok.token_value.op = f->second;
    } else if (code == FlexBison_Parser::token::token_kind_type::EoF) {
        tok.token_type = T_NONE;
    } else {
        assert(0);
    }
    types::Position pos(yyfile(), line, column);
    tok.token_pos = std::move(pos);
    return tok;
}
