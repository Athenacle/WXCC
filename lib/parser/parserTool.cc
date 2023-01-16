/* syntax parser tools.
* WangXiaochun
* zjjhwxc(at)gmail.com
* Feb 9, 2013
*
*/
#include "system.h"
#include "lex/lexer.h"
#include "parser.h"

using namespace lex;
using namespace types;

int Parser::tokenCompare(int compareTo) const
{
    Token &to = cur;
    //TODO: THIS FUNCTION NEED TO BE TESTED.
    if ((compareTo & TYPE_SSQ) == TYPE_SSQ)
        return to.token_type == T_KEY
               && (to.token_value.keyword & compareTo) == (compareTo & (int)TYPE_SSQ);

    if ((compareTo & KEYWORD_MASK) == KEYWORD_MASK) {
        return to.token_type == T_KEY && to.token_value.keyword == compareTo;
    }
    if ((compareTo & OP_MASK) == OP_MASK) {
        return to.token_type == T_OPERATOR && to.token_value.keyword == compareTo;
    }

    return compareTo == to.token_type;
}