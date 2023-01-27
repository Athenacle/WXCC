/* symbol table header
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 23, 2013
*
*/

#include "symbol.h"

#include "tools.h"

USING_V1
using namespace scope;

void Symbol::setValue(Token *tok)
{
    if (tok->token_type == T_INT_CON) {
        value.i_v = tok->token_value.numVal->val.i_value;
    } else if (tok->token_type == T_FLOAT_CON) {
        if (tok->token_value.numVal->numberType == NT_FL) {
            value.f_v = tok->token_value.numVal->val.d_value;
        } else if (tok->token_value.numVal->numberType == NT_DB) {
            value.d_v = tok->token_value.numVal->val.d_value;
        } else {
            assert(0);
        }
    } else if (tok->token_type == T_ID) {
        value.string = const_cast<char *>(tok->token_value.string);
    } else {
        ;
    }
}

Symbol::Symbol(Token *tok, Scope sc, StorageType st, int level, Symbol *sy)
{
    tok    = tok;
    sc     = sc;
    sclass = st;
    up     = sy;
    ref    = 0;
    level  = level;
    next   = nullptr;
    setValue(tok);
}

Symbol::~Symbol()
{
    delete tok;
    delete up;
    delete next;
}
