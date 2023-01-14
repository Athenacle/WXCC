/* syntax parser.
* WangXiaochun
* zjjhwxc(at)gmail.com
* Feb 9, 2013
*
*/

#include "system.h"
#include "lex/lexer.h"
#include "type.h"
#include "symbol.h"
#include "parser.h"
#include "exception.h"

#include "lex/tools.h"

using namespace lex;
using namespace constants;
using namespace scope;
using namespace utils;
using namespace type_operator;
using namespace lex::tools;

int currentLevel = S_GLOBAL;
std::map<KEYWORD, TYPE_OPERATOR> Parser::key2to;
std::map<OP, char> Parser::op2c;
std::map<TYPE, const char*> Parser::tp2s;
std::map<OP, int> Parser::op2l;

void Parser::initParser(void)
{
    InitKey2to();
    InitOp2c();
    InitTp2S();
    InitOp2l();
}

void Parser::begin_parse(Env& env) const
{
    c_parser_translation_unit(env);
}

bool Parser::need(const OP op) const
{
    next();
    if (matchOP(*cur, op))
        return true;
    else {
        if (isOP(*cur))
            parserError(
                PAR_ERR_NEED_C_CC, cur->token_pos->line, op2c[op], op2c[cur->token_value.op]);
        else if (isBaseType(*cur)) {
            parserError(PAR_ERR_NEED_C_CS, op2c[op], "type");
        } else if (isID(*cur)) {
            parserError(
                PAR_ERR_NEED_C_CC, cur->token_pos->line, op2c[op], op2c[cur->token_value.op]);
        } else {
            parserError(PAR_ERR_NEED_C_CS, op2c[op], "others");
        }
        pushBack();
        return false;
    }
}

bool Parser::need(const TYPE _ty) const
{
    next();
    if (is(*cur, _ty))
        return true;
    else {
        if (isOP(*cur))
            parserError(PAR_ERR_NEED_C_CS, tp2s[_ty], op2c[cur->token_value.op]);
        else if (isBaseType(*cur)) {
            parserError(PAR_ERR_NEED_SS, tp2s[_ty], tp2s[cur->token_type]);
        } else
            parserError(PAR_ERR_NEED_C_CS, "others");

        pushBack();
        return false;
    }
}

Parser::~Parser()
{
    //delete
}

void Parser::InitTp2S(void)
{
    tp2s[T_FLOAT_CON] = "floating point constant.";
    tp2s[T_ID] = "identifier.";
    tp2s[T_KEY] = "keyword.";
    tp2s[T_STRING] = "string.";
    tp2s[T_INT_CON] = "int constant.";
    tp2s[T_CHAR_CON] = "char constant.";
}


void Parser::InitKey2to(void)
{
    key2to[KEY_CHAR] = TO_CHAR;
    key2to[KEY_DOUBLE] = TO_DOUBLE;
    key2to[KEY_KVOID] = TO_VOID;
    key2to[KEY_FLOAT] = TO_FLOAT;
    key2to[KEY_INT] = TO_INT;
    key2to[KEY_SHORT] = TO_SHORT;
    key2to[KEY_SIGNED] = TO_SIGNED;
    key2to[KEY_UNSIGNED] = TO_UNSIGNED;
    key2to[KEY_CONST] = TO_CONST;
    key2to[KEY_VOLATILE] = TO_VOLATILE;
}

void Parser::InitOp2c(void)
{
    op2c[OP_SEMICOLON] = ';';
    op2c[OP_COMMA] = ',';
    op2c[OP_MULT] = '*';
    op2c[OP_LEFTBRACK] = '(';
    op2c[OP_RIGHTBRACK] = ')';
    op2c[OP_RIGHTBRACE] = '}';
    op2c[OP_LEFTBRACE] = '{';
    op2c[OP_RIGHTSQBRAC] = ']';
    op2c[OP_LEFTSQBRAC] = '[';
}

int Parser::next(void) const
{
    int ret = 0;
    if (prepre != nullptr) {
        pre = prepre;
        cur = pre;
        prepre = nullptr;
    } else if (pre != nullptr) {
        cur = pre;
        pre = nullptr;
    } else {
        ret = theLexer.getNextToken();
        cur = theLexer.getToken();
        if (ret == EOT && isFinish == false)
            fatalError(
                "Meeting the EOF of the"
                " file before compile finish.\n");
    }
    return ret;
}

void Parser::InitOp2l(void)
{
    op2l[OP_SEMICOLON] = 0;
    op2l[OP_RIGHTBRACK] = 0;
    op2l[OP_LEFTBRACK] = 0;
    op2l[OP_LEFTSQBRAC] = 0;
    op2l[OP_RIGHTSQBRAC] = 0;
    op2l[OP_LEFTBRACE] = 0;
    op2l[OP_EPPIPSIS] = 0;
    op2l[OP_PERIOD] = 0;
    op2l[OP_COMMA] = 1;
    op2l[OP_ALPLUS] = 2;
    op2l[OP_ALMINUS] = 2;
    op2l[OP_ALMULT] = 2;
    op2l[OP_ALDIV] = 2;
    op2l[OP_ALMOD] = 2;
    op2l[OP_ALLEFTSHIFT] = 2;
    op2l[OP_ALRIGHTSHIFT] = 2;
    op2l[OP_ALBITAND] = 2;
    op2l[OP_ALBITXOR] = 2;
    op2l[OP_ALBITOR] = 2;
    op2l[OP_ASSIGN] = 2;
    op2l[OP_COLON] = 3;
    op2l[OP_QUESTION] = 3;
    op2l[OP_LOGOR] = 4;
    op2l[OP_LOGAND] = 5;
    op2l[OP_BITOR] = 6;
    op2l[OP_BITXOR] = 7;
    op2l[OP_BITAND] = 8;
    op2l[OP_EQ] = 9;
    op2l[OP_NE] = 9;
    op2l[OP_LT] = 10;
    op2l[OP_GT] = 10;
    op2l[OP_LE] = 10;
    op2l[OP_GE] = 10;
    op2l[OP_LEFTSHIFT] = 11;
    op2l[OP_RIGHTSHIFT] = 11;
    op2l[OP_PLUS] = 12;
    op2l[OP_MINUS] = 12;
    op2l[OP_MOD] = 13;
    op2l[OP_MULT] = 13;
    op2l[OP_DIV] = 13;
    op2l[OP_POINTER] = 14;
    op2l[OP_INC] = 14;
    op2l[OP_DEC] = 14;
    op2l[OP_LOGNOT] = 14;
    op2l[OP_BITNOT] = 14;
}

Block* Parser::c_parser_label_statement(Env&) const
{
    return 0;
}


Block* Parser::c_parser_statement_list(Env& curEnv) const
{
    return c_parser_statement(curEnv);
}
