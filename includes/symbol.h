/* symbol table header
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 23, 2013
*
*/

#ifndef SYMBOL_H
#define SYMBOL_H

#include "lex/lexer.h"

#include <iterator>
using std::iterator;
using namespace lex::types;
using namespace lex::constants;

#undef HASH_TABLE_SIZE
#define HASH_TABLE_SIZE 256

namespace scope
{
    enum Scope { S_NONE = 0x00080000, S_CONSTANTS, S_LABELS, S_GLOBAL, S_PARAMETER, S_LOCAL };

    enum StorageType {
        ST_NONE = 0x00100000, /* used for different kinds of constants.*/
        ST_AUTO,
        ST_REGISTER,
        ST_STATIC,
        ST_EXTERN,
        ST_TYPEDEF
    };
};  // namespace scope


class Symbol
{
protected:
    Token *tok;                /* token		*/
    scope::Scope sc;           /* enum scope		*/
    scope::StorageType sclass; /* enum storage_type	*/
    int level;                 /* scope level		*/
    Symbol *up;                /* link			*/
    int ref;                   /* reference times	*/
    union {
        int i_v;
        float f_v;
        double d_v;
        char *string;
    } value;
    /* symbol which can be put into a table only can be identifiers.
	* but identifiers only have a name, without values.
	*/
    Symbol *next;
    void setValue(Token *);
    void useSymbol(void)
    {
        ref++;
    }

public:
    Symbol(Token *, scope::Scope, scope::StorageType, int, Symbol * = nullptr);

    Symbol(const Symbol &sy)
    {
        tok = sy.tok;
        sc = sy.sc;
        sclass = sy.sclass;
        level = sy.level;
        up = sy.up;
        ref = sy.ref;
        value.i_v = sy.value.i_v;
    }

    virtual ~Symbol();
    void setScope(scope::Scope _sc, int _lev)
    {
        sc = _sc;
        level = _lev;
    }

    char *getIDName(void) const
    {
        return value.string;
    }

    Symbol &operator=(Symbol &sy)
    {
        tok = sy.tok;
        sc = sy.sc;
        sclass = sy.sclass;
        level = sy.level;
        up = sy.up;
        ref = sy.ref;
        value.i_v = sy.value.i_v;
        return *this;
    }
};

#endif