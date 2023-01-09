/* syntax parser tools.
* WangXiaochun
* zjjhwxc(at)gmail.com
* Feb 9, 2013
*
*/
#include "system.h"
#include "lex.h"
#include "parser.h"

using namespace NS_LEX_CONSTANTS;

int Parser::tokenCompare(int compareTo) const
{
    /*__asm{
	int 3
	}*/
    NS_LEX_PODS::Token *to = theLexer.getToken();
    //TODO: THIS FUNCTION NEED TO BE TESTED.
    if ((compareTo & TYPE_SSQ) == TYPE_SSQ)
        return to->token_type == T_KEY
               && (to->token_value.keyword & compareTo) == (compareTo & TYPE_SSQ);

    if ((compareTo & KEYWORD_MASK) == KEYWORD_MASK) {
        return to->token_type == T_KEY && to->token_value.keyword == compareTo;
    }
    if ((compareTo & OP_MASK) == OP_MASK) {
        return to->token_type == T_OPERATOR && to->token_value.keyword == compareTo;
    }

    return compareTo == to->token_type;
}