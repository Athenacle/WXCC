#include "lex/token.h"

#include <cstring>

#include "lex/lexer.h"
#include "lex/tools.h"

#include "tools.h"

using namespace lex;
using namespace lex::types;
using namespace lex::constants;
using namespace lex::tools;


bool Position::operator==(const Position& other) const
{
    return line == other.line && place == other.place && other.filename && filename
           && *other.filename == *filename;
}

Token::Token(KEYWORD key)
{
    token_type = T_KEY;
    token_value.keyword = key;
}

Token::Token(constants::TYPE ty, const char* cc)
{
    token_type = ty;
    token_value.string = utils::strdup(cc);
}

Token::Token(Number* n)
{
    if (n->numberType == NT_FL || n->numberType == NT_DB)
        token_type = T_FLOAT_CON;
    else if (n->numberType == NT_CH)
        token_type = T_CHAR_CON;
    token_value.numVal = n;
}

Token::Token(constants::OP op)
{
    token_type = T_OPERATOR;
    token_value.op = op;
}

Token::Token(Token&& other)
{
    token_type = other.token_type;
    token_pos = std::move(other.token_pos);
    memcpy(&token_value, &other.token_value, sizeof(token_value));
    other.token_type = T_NONE;
}

Token::Token(const Token& other)
{
    token_type = other.token_type;
    token_pos = other.token_pos;
    if (isNumCon(other)) {
        token_value.numVal = new Number(*other.token_value.numVal);
    } else if (token_type == T_ID || token_type == T_STRING) {
        token_value.string = utils::strdup(other.token_value.string);
    } else {
        memcpy(&token_value, &other.token_value, sizeof(token_value));
    }
}

bool Token::operator==(const Token& other) const
{
    if (token_type != other.token_type) {
        return false;
    }
    switch (token_type) {
        case T_ID:
        case T_STRING:
            return strcmp(token_value.id_name, other.token_value.id_name) == 0;
        case T_KEY:
            return token_value.keyword == other.token_value.keyword;
        case T_OPERATOR:
            return token_value.op == other.token_value.op;
        case T_INT_CON:
        case T_FLOAT_CON:
        case T_CHAR_CON:
            return token_value.numVal->operator==(*other.token_value.numVal);
        case T_NONE:
            return true;
    }
    return false;
}

Token::~Token()
{
    if (token_type == T_ID || token_type == T_STRING) {
        delete[] token_value.string;
    }

    if (isNumCon(*this)) {
        delete token_value.numVal;
    }
}