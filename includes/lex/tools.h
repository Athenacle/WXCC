#ifndef LEX_TOOLS_H_
#define LEX_TOOLS_H_

#include "lex/constants.h"
#include "lex/token.h"

namespace lex
{

    namespace tools
    {
        using types::Token;
        using namespace constants;


        bool inline is(const Token &_tok, TYPE _ty)
        {
            return _tok.token_type == _ty;
        }

        bool inline isNumCon(const Token &_tok)
        {
            return _tok.token_type == T_INT_CON || _tok.token_type == T_FLOAT_CON
                   || _tok.token_type == T_CHAR_CON;
        }

        bool inline matchKEY(const Token &_tok, KEYWORD k)
        {
            return is(_tok, T_KEY) && _tok.token_value.keyword == k;
        }

        bool inline isOP(const Token &_tok)
        {
            return is(_tok, T_OPERATOR);
        }

        bool inline isBaseType(const Token &_tok)
        {
            return is(_tok, T_KEY)
                   && ((_tok.token_value.keyword & TYPE_SPECIFIER_MASK) == TYPE_SPECIFIER_MASK
                       || matchKEY(_tok, KEY_TVOID) || matchKEY(_tok, KEY_KVOID));
        }

        bool inline matchOP(const Token &_tok, OP _op)
        {
            return isOP(_tok) && _tok.token_value.op == _op;
        }

        bool inline matchQualifier(const Token &_tok)
        {
            return _tok.token_type == T_KEY
                   && (_tok.token_value.keyword == KEY_CONST
                       || _tok.token_value.keyword == KEY_VOLATILE);
        }

        bool inline isID(const Token &_tok)
        {
            return is(_tok, T_ID);
        }

        bool inline isIter(const Token &_tok)
        {
            return is(_tok, T_KEY)
                   && (_tok.token_value.keyword == KEY_FOR || _tok.token_value.keyword == KEY_WHILE
                       || _tok.token_value.keyword == KEY_DO);
        }

        bool inline isJump(const Token &_tok)
        {
            return is(_tok, T_KEY)
                   && (_tok.token_value.keyword == KEY_RETURN
                       || _tok.token_value.keyword == KEY_BREAK
                       || _tok.token_value.keyword == KEY_CONTINUE
                       || _tok.token_value.keyword == KEY_GOTO);
        }

        bool inline isSelect(const Token &_tok)
        {
            return is(_tok, T_KEY)
                   && (_tok.token_value.keyword == KEY_SWITCH
                       || _tok.token_value.keyword == KEY_IF);
        }

        bool inline isLabel(const Token &_tok)
        {
            return is(_tok, T_KEY)
                   && (_tok.token_value.keyword == KEY_CASE
                       || _tok.token_value.keyword == KEY_DEFAULT);
        }

        bool inline isASSIGN(const Token &_tok)
        {
            return isOP(_tok) && (_tok.token_value.op & OP_ASGN_MASK) == OP_ASGN_MASK;
        }

    };  //namespace tools

}  // namespace lex

#endif