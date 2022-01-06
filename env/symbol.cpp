﻿/* symbol table header
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 23, 2013
*
*/

#include "tools.h"
#include "symbol.h"


void Symbol::setValue(Token *_tok)
{
    if (_tok->token_type == T_INT_CON)
        value.i_v = _tok->token_value.numVal->val.i_value;
    else if (_tok->token_type == T_FLOAT_CON) {
        if (_tok->token_value.numVal->numberType == NT_FL)
            value.f_v = _tok->token_value.numVal->val.d_value;
        else if (_tok->token_value.numVal->numberType == NT_DB)
            value.d_v = _tok->token_value.numVal->val.d_value;
        else {
            assert(0);
        }
    } else if (_tok->token_type == T_ID) {
        value.string = const_cast<char *>(_tok->token_value.string);
    } else {
        ;
    }
}

Symbol::Symbol(Token *_tok, scope _sc, storage_type _st, int _level, Symbol *_sy)
{
    tok = _tok;
    sc = _sc;
    sclass = _st;
    up = _sy;
    ref = 0;
    level = _level;
    next = NULL;
    setValue(_tok);
}

Symbol::~Symbol()
{
    delete tok;
    delete up;
    delete next;
}
