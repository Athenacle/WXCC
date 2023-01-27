#ifndef ENVIRONMENT_IDENTIFIER_H_
#define ENVIRONMENT_IDENTIFIER_H_

#include "system.h"

#include "lex/token.h"
#include "semantic/type.h"

NAMESPACE_BEGIN

namespace environment
{
    class Identifier
    {
        using Type     = semantic::BaseType;
        using Position = lex::types::Position;
        using Token    = lex::types::Token;

    private:
        std::string idName_;

        Position pos_;

        Type idType_;

    public:
        Identifier() = default;

        Identifier(const Token& tok, Type&& t);

        const Type& type() const
        {
            return idType_;
        }

        Type& type()
        {
            return idType_;
        }
        const Position& position() const
        {
            return pos_;
        }
    };
}  // namespace environment

NAMESPACE_END

#endif
