#include "environment/identifier.h"

NAMESPACE_BEGIN

using namespace environment;
using namespace lex::constants;

Identifier::Identifier(const Token& tok, Type&& t)
    : idName_(tok.token_value.id_name), pos_(tok.token_pos), idType_(std::move(t))
{
    assert(tok.token_type == T_ID);
}


NAMESPACE_END
