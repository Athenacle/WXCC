/* indentifier header
* WangXiaochun
* zjjhwxc(at)gmail.com
* April 4, 2013
*
*/

#ifndef IDENTIFIER_H
#define IDENTIFIER_H


#include <map>

#include "lex/lexer.h"

#include "semantic.h"
#include "symbol.h"
#include "table.h"
#include "type.h"

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
               scope::Scope _sc,
               scope::StorageType _st,
               int _level,
               Type* _ty = NS_BASE_TYPE::voidType,
               Symbol* _sy = nullptr)
        : Symbol(_tok, _sc, _st, _level, _sy), TemportaryVariable()
    {
        ty = _ty;

        name = _tok->token_value.id_name;
    }
    Identifier(const Symbol& _sy, Type* _ty) : Symbol(_sy), TemportaryVariable(), ty(_ty)
    {
        name = _sy.getIDName();
    }

    Identifier() : Symbol(nullptr, scope::S_LOCAL, scope::ST_AUTO, 0), name(nullptr) {}

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
        return ty->getTYOP() == type_operator::TO_FUNCTION;
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
        return utils::strHash(this->getIDName());
    }
};


#endif