/* syntax parser tools.
* WangXiaochun
* zjjhwxc(at)gmail.com
* Feb 9, 2013
*
*/
#include "system.h"

#include "lex/lexer.h"

#include "parser.h"

USING_V1
using namespace lex;
using namespace types;

int Parser::tokenCompare(int compareTo) const
{
    Token &to = cur;
    //TODO: THIS FUNCTION NEED TO BE TESTED.
    if ((compareTo & TYPE_SSQ) == TYPE_SSQ) {
        return static_cast<int>(to.token_type == T_KEY
                                && (to.token_value.keyword & compareTo)
                                       == (compareTo & (int)TYPE_SSQ));
    }

    if ((compareTo & KEYWORD_MASK) == KEYWORD_MASK) {
        return static_cast<int>(to.token_type == T_KEY && to.token_value.keyword == compareTo);
    }
    if ((compareTo & OP_MASK) == OP_MASK) {
        return static_cast<int>(to.token_type == T_OPERATOR && to.token_value.keyword == compareTo);
    }

    return static_cast<int>(compareTo == to.token_type);
}
