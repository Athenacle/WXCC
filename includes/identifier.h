/* indentifier header
* WangXiaochun
* zjjhwxc(at)gmail.com
* April 4, 2013
*
*/

#ifndef IDENTIFIER_H
#define IDENTIFIER_H


#include "lex.h"
#include "type.h"
#include "symbol.h"
#include "Table.h"
#include "semantic.h"

using namespace NS_BASE_TYPE;
//using namespace NS_IR;


class Identifier : public Symbol, TemportaryVariable
{
    friend class Function;
    Type* ty;
    bool isConst;
    bool isVolatile;
    IR* assign;
    const char* name;

    Identifier& operator=(Identifier);


    const Type& getType(void) const
    {
        return *this->ty;
    }

    const Type& getBaseType(void) const
    {
        return this->ty->getBaseType();
    }

public:
    Identifier(Token* _tok,
               scope _sc,
               storage_type _st,
               int _level,
               Type* _ty = NS_BASE_TYPE::voidType,
               Symbol* _sy = NULL)
        : Symbol(_tok, _sc, _st, _level, _sy), TemportaryVariable()
    {
        ty = _ty;

        name = _tok->token_value.id_name;
    }
    Identifier(const Symbol& _sy, Type* _ty) : Symbol(_sy), ty(_ty), TemportaryVariable()
    {
        name = _sy.getIDName();
    }

    Identifier() : Symbol(NULL, S_LOCAL, ST_AUTO, 0), name(NULL) {}

    ~Identifier()
    {
        delete ty;
    }

    const char* getIDName(void) const
    {
        return name;
    }

    const char* getTYPEName(void)
    {
        return ty->getTYPEName();
    }

    bool inline isID(void) const
    {
        return !isFunction();
    }

    bool inline isFunction(void) const
    {
        return ty->getTYOP() == NS_TYPE_OP::TO_FUNCTION;
    }

    int inline getSymbolLine(void) const
    {
        return this->tok->token_pos->line;
    }

    void useID(void)
    {
        useSymbol();
    }

    Type& getType(void)
    {
        return *ty;
    }

    const char* getIDname(void) const
    {
        return this->tok->token_value.string;
    }

    const char* setIDname(const char* n)
    {
        return name = this->tok->token_value.string = n;
    }

    unsigned long getHash(void)
    {
        return NS_LEX_TOOLS::strHash(this->getIDName());
    }
};


#endif