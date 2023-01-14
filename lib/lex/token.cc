#include "lex/token.h"
#include "lex/lexer.h"

using namespace lex;
using namespace lex::types;
using namespace lex::constants;


Token::Token(KEYWORD key)
{
    token_pos = new Position(*Lex::currentPos);
    token_type = T_KEY;
    token_value.keyword = key;
}

Token::Token(constants::TYPE ty, const char* cc)
{
    token_pos = new Position(*Lex::currentPos);
    token_type = ty;
    token_value.string = cc;
}

Token::Token(Number* n)
{
    token_pos = new Position(*Lex::currentPos);
    if (n->numberType == NT_FL || n->numberType == NT_DB)
        token_type = T_FLOAT_CON;
    else if (n->numberType == NT_CH)
        token_type = T_CHAR_CON;
    token_value.numVal = n;
}

Token::Token(Token& tok) : token_type(tok.token_type)
{
    token_pos = new Position(*Lex::currentPos);
    token_value.numVal = tok.token_value.numVal;
}
