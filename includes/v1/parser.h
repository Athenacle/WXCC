/* syntax parser header.
* WangXiaochun
* zjjhwxc(at)gmail.com
* Feb 9, 2013
*
*/

#ifndef PARSER_H
#define PARSER_H

#include <map>

#include "lex/lexer.h"

#include "environment.h"
#include "expr.h"
#include "functions.h"
#include "stmt.h"
#include "tree.h"
#include "type.h"

NAMESPACE_V1_BEGIN

using namespace lex::constants;
using namespace type_operator;


/* marcos for syntax warnings and errors. */
#define PAR_WARNING_EMPTY_TRANSLATION_UNIT 1
#define PAR_WARNING_EMPTY_DECLARATION      2

#define PAR_ERR_NEED_TYPE                  1
#define PAR_ERR_NEED_C_CC                  2
#define PAR_ERR_NEED_C_CS                  3
#define PAR_ERR_NEED_SS                    4
#define PAR_ERR_UNMATCHED_KEY              6

#define PAR_ERR_NEED_VAR_D                 95
#define PAR_ERR_INT_STR                    96
#define PAR_ERR_STR_INT                    97

#define PAR_ERR_INT                        0xfedcba
#define PARER_WE_USE_STRING                0xabcdef

typedef std::pair<Symbol*, Type*> ST;

class Parser
{
    friend class Tree;

private:
    lex::Lex& theLexer;
    mutable lex::types::Token cur;
    mutable lex::types::Token prepre;
    mutable bool isFinish;
    mutable lex::types::Token pre;

    static std::map<KEYWORD, TYPE_OPERATOR> key2to;
    static std::map<OP, char> op2c;
    static std::map<TYPE, const char*> tp2s;
    static std::map<OP, int> op2l;

    static int parserWarningCount;
    static int parserErrorCount;

    void lookAHead(void) const
    {
        prepre = pre;
        pre = cur;
        cur = theLexer.getNextToken();
        //cur = theLexer.getNextToken();
    }

    void pushBack(void) const
    {
        if (!pre) {
            pre = cur;
        } else if (!prepre) {
            prepre = pre;
            pre = cur;
        } else
            assert(0);
        cur = nullptr;
    }

    void setPC(lex::types::Token&& _cur, lex::types::Token&& _pre) const
    {
        cur = _cur;
        pre = _pre;
    }

    int next(void) const;

    Token mkToken(void) const
    {
        return theLexer.getNextToken();
    }

    mutable Token* lastToken;

    //---------------------parser functions------------------------.

    //--declaration
    int c_parser_translation_unit(Env&) const;
    int c_parser_declaration(Env&, scope::Scope) const;
    Type* c_parser_declaration_specifiers(Type*) const;
    Type* c_parser_declaration_type_specifiers(Type*) const;
    Type* c_parser_declaration_type_specifiers_opt(Type* ty) const;
    ST* c_parser_declarator(Type*) const;
    Type* c_parser_pointer(Type*) const;
    Type* c_parser_type_qualifier_list(Type*) const;
    ST* c_parser_direct_declarator(Type*) const;
    Type* c_parser_direct_declatator_opt(Type* ty) const;
    Type* c_parser_parameter_type_list(void) const;

    //--statement
    ST* c_parser_funtion_definition(Env&, Identifier&) const;

    Block* c_parser_compound_statement(Env&) const;
    Block* c_parser_compound_statement(Function&) const;

    Block* c_parser_statement_list(Env&) const;
    Block* c_parser_statement(Env&) const;
    Block* c_parser_jump_statement(Env&) const;
    Block* c_parser_select_statement(Env&) const;
    Block* c_parser_iter_statement(Env&) const;
    Block* c_parser_expr_statement(Env&) const;
    Block* c_parser_label_statement(Env&) const;
    Tree* c_parser_expr_func_call(Env&, Tree*) const;
    Block* c_parser_iter_for(Env&) const;
    Block* c_parser_iter_while(Env&) const;
    //--expression
    Expr* c_parser_expressions(Env&) const;

    Tree* c_parser_base_expressions(Env&, int) const;
    Tree* c_parser_expressions_1(Env&, int) const;
    Tree* c_parser_expressions_2(Env&) const;

    Tree* c_parser_dispatch_comp_assignment(const OP, Tree* leftT, Env&) const;
    Tree* c_parser_expressions_3(Env&, int) const;
    Tree* c_parser_unary_expr(Env&) const;
    Tree* c_parser_postfix_expr(Env&, Tree*) const;
    Tree* c_parser_primary_expr(Env&) const;
    void c_parser_expression_recover(void) const;
    //------------------functions end------------------------------.

    void parserWarning(int warningType, ...) const;
    void parserError(int errorType, ...) const;
    bool need(const OP) const;
    bool need(const TYPE) const;
    int tokenCompare(int) const;

    static void InitKey2to(void);
    static void InitOp2c(void);
    static void InitTp2S(void);
    static void initParser(void);
    static void InitOp2l(void);

    //the operator = and the copy ctor cannot be used.
    Parser& operator=(Parser&);
    Parser(Parser&);
    Parser();

public:
    ~Parser();
    explicit Parser(lex::Lex& lexer) : theLexer(lexer)
    {
        initParser();

        isFinish = false;
        prepre = pre = cur = nullptr;
    }

    void begin_parse(Env& env) const;
};
NAMESPACE_END

#endif
