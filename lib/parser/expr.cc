/* syntax parser- expressions
* WangXiaochun
* zjjhwxc(at)gmail.com
* April 7, 2013
*/

#include "tree.h"
#include "parser.h"
#include "lex/lexer.h"
#include "environment.h"
#include "identifier.h"
#include "expr.h"
#include "system.h"

#include "lex/tools.h"

using namespace lex;
using lex::tools::isASSIGN;
using namespace NS_EXPRE_IR;
using namespace constants;

Expr *Parser::c_parser_expressions(Env &env) const
{
    Expr *exprIR = nullptr;

    try {
        Tree *tr = c_parser_base_expressions(env, 0);
        exprIR = new Expr(tr);
        return exprIR;
    }

    catch (const Exception &ee) {
        c_parser_expression_recover();
    }
    return 0;
}

Tree *Parser::c_parser_base_expressions(Env &env, MAYBE_UNUSED int k) const
{
    //expr -> assignment-expr { ',' assignment-expr }

    //stop: IF ID } 0
    Tree *tp = c_parser_expressions_1(env, 0);
    while (cur == OP_COMMA) {
        MAYBE_UNUSED Tree *ret = nullptr;
        next();
        ret = c_parser_expressions_1(env, 0);
        //tp = new Tree(RIGHT, ret->getType(), NULL, ret);
    }
    return tp;
}

Tree *Parser::c_parser_expressions_1(Env &env, MAYBE_UNUSED int k) const
//assignment statements.
{
    Tree *tp = c_parser_expressions_2(env);
    OP op = cur.token_value.op;
    int level = op2l[op];
    if (op == OP_ASSIGN || (level >= 6 && level <= 8) || (level >= 11 && level <= 13)) {
        if (level == 2) {
            next();
            tp = c_parser_dispatch_comp_assignment(op, tp, env);
        }
    }
    return tp;
}

Tree *Parser::c_parser_dispatch_comp_assignment(const OP op, Tree *leftT, Env &env) const
{
    int _op = op;
    Tree *tr = nullptr;
    Tree *rightT = nullptr;


    rightT = c_parser_expressions_1(env, 0);

    switch (_op) {
        case OP_ASSIGN:
            //rightT = c_parser_expressions_1(env, 0);
            tr = Tree::ASGNtree(leftT, rightT);
            break;
        case OP_ALPLUS:
            tr = Tree::PLUStree(leftT, rightT);
            break;
        case OP_ALMINUS:
            tr = Tree::MINUStree(leftT, rightT);
            tr = Tree::ASGNtree(leftT, tr);
            break;
        case OP_ALMULT:
            tr = Tree::MULTtree(leftT, rightT);
            tr = Tree::ASGNtree(leftT, tr);
            break;
        case OP_ALDIV:
            tr = Tree::DIVtree(leftT, rightT);
            tr = Tree::ASGNtree(leftT, tr);
            break;
        case OP_ALMOD:
            tr = Tree::MODtree(leftT, rightT);
            tr = Tree::ASGNtree(leftT, tr);
            break;
        case OP_ALLEFTSHIFT:
            tr = Tree::SHLtree(leftT, rightT);
            tr = Tree::ASGNtree(leftT, tr);
            break;
        case OP_ALRIGHTSHIFT:
            tr = Tree::SHRtree(leftT, rightT);
            tr = Tree::ASGNtree(leftT, tr);
            break;
        case OP_ALBITAND:
            tr = Tree::BITANDtree(leftT, rightT);
            tr = Tree::ASGNtree(leftT, tr);
            break;
        case OP_ALBITXOR:
            tr = Tree::BITXORtree(leftT, rightT);
            tr = Tree::ASGNtree(leftT, tr);
            break;
        case OP_ALBITOR:
            tr = Tree::BITORtree(leftT, rightT);
            tr = Tree::ASGNtree(leftT, tr);
            break;

        default:
            assert(0);
    }
    return tr;
}

Tree *Parser::c_parser_expressions_2(Env &env) const
//conditional-expr -> binary-expr [ '?' expression ':' conditional-expr]
{
    Tree *binT = c_parser_expressions_3(env, 4);

    if (cur == OP_QUESTION)  // meet the ? OP, parse the optional expr
    {
        Tree *trueT = c_parser_base_expressions(env, 0);
        need(OP_COLON);
        Tree *falseT = c_parser_expressions_2(env);
        binT = Tree::CONDtree(binT, trueT, falseT);
    }

    return binT;
}

Tree *Parser::c_parser_expressions_3(Env &env, int k) const
{
    // most binary operators. need left-associative operators
    int k1;
    Tree *p = c_parser_unary_expr(env);

    if (cur == T_OPERATOR) {
        for (k1 = op2l[cur.token_value.op]; k1 >= k; k1--) {
            while (op2l[cur.token_value.op] == k1 && !isASSIGN(cur)) {
                OP op = cur.token_value.op;
                Tree *rightT = nullptr;
                MAYBE_UNUSED Tree *leftT = nullptr;
                if (op == OP_LOGOR) {
                    next();
                    Tree *rightT = c_parser_expressions_3(env, 4);
                    Tree *preT = p;
                    p = Tree::ORtree(preT, rightT);
                } else if (op == OP_LOGAND) {
                    next();
                    Tree *rightT = c_parser_expressions_3(env, 4);
                    p = Tree::ANDtree(p, rightT);
                } else {
                    next();
                    rightT = c_parser_expressions_3(env, k1 + 1);
                    p = op2fun(op)(p, rightT);
                }
            }
        }
    }
    return p;
}

void Parser::c_parser_expression_recover(void) const
{
    // when exception occurs, skip all the symbols until next ;
    for (next(); !(cur == OP_SEMICOLON); next())
        ;
}

Tree *Parser::c_parser_unary_expr(Env &env) const
{
    Tree *tp = nullptr;
    TYPE ty = cur.token_type;
    const Type *objectType = nullptr;
    MAYBE_UNUSED size_t size;
    TYPE_OPERATOR to;
    MAYBE_UNUSED Node_OP nop;
    if (ty == T_OPERATOR && !tools::matchOP(cur, OP_LEFTBRACK)) {
        OP _op = cur.token_value.op;
        switch (_op) {
            case OP_MULT:
                next();
                tp = c_parser_unary_expr(env);
                if (tp == nullptr)
                    assert(0);
                objectType = &tp->getType();

                to = objectType->getTYOP();
                if (to == TO_ARRAY || to == TO_POINTER) {
                    objectType = &objectType->getBaseType();
                    tp = Tree::INDRtree(tp, *objectType);
                } else if (to == TO_FUNCTION) {
                    ;  //do nothing.
                } else {
                    assert(0);
                    //�Ƿ��ļ��Ѱַ
                    //e.g: int i; *i;
                }
                break;
            case OP_BITAND:  //lea
                next();
                tp = c_parser_unary_expr(env);
                if (tp == nullptr)
                    assert(0);
                if (tp->getNOP() == NOP_CNST) {
                    assert(0);
                } else {
                    Type *newTy = new Type(TO_POINTER, const_cast<Type *>(&tp->getType()));
                    tp = Tree::LEAtree(tp, *newTy);
                }
                break;
            case OP_MINUS:
                next();
                tp = c_parser_unary_expr(env);
                if (tp->getType().getTYOP() == TO_POINTER) {
                    assert(0);
                    //��ָ��ʹ�� -
                }
                tp = Tree::MINUStree(Tree::CONSTtree(0), tp);
                break;
            case OP_PLUS:
                next();
                tp = c_parser_unary_expr(env);
                if (tp->getType().getTYOP() == TO_POINTER) {
                    assert(0);
                    //��ָ��ʹ�� +
                }
                tp = Tree::PLUStree(Tree::CONSTtree(0), tp);
                break;
            case OP_BITNOT:
                //;break;
            case OP_LOGNOT:
                //;break;
            case OP_INC:
                //;break;
            case OP_DEC:
                //;break;
            case OP_SIZEOF:
                next();
                if (cur == T_ID) {
                    Identifier *id = env.findID(cur.token_value.id_name);
                    if (id == nullptr)
                        assert(0);
                    else {
                        size = id->getType().getSize();
                    }
                } else {
                    if (cur == OP_LEFTBRACK) {
                        next();
                        if (tools::isBaseType(cur)) {
                            Type *ty = c_parser_declaration_type_specifiers(nullptr);
                            size = ty->getSize();
                            delete ty;
                        } else if (cur == T_ID) {
                            Identifier *id = env.findID(cur.token_value.id_name);
                            if (id == nullptr)
                                assert(0);
                            else {
                                size = id->getType().getSize();
                            }
                        }
                        need(OP_RIGHTBRACK);
                    }
                    tp = Tree::CONSTtree(tp->getType().getSize());
                }
                break;
            case OP_SEMICOLON:
                break;
            default:
                break;
        }
    } else {
        tp = c_parser_postfix_expr(env, c_parser_primary_expr(env));
    }
    return tp;
}

Tree *Parser::c_parser_postfix_expr(Env &env, Tree *tr) const
{
    Tree *retTree = tr;
    for (;;) {
        switch (cur.token_value.op) {
            case OP_INC:
                break;
            case OP_DEC:
                break;
            case OP_LEFTSQBRAC: {
                next();
                Tree *sub = c_parser_expressions_1(env, 0);
                MAYBE_UNUSED const char *arrayName = tr->getIDName();
                retTree = Tree::ARRAYtree(retTree, sub);
                pushBack();
                need(OP_RIGHTSQBRAC);
                break;
            }
            case OP_LEFTBRACK:
                retTree = c_parser_expr_func_call(env, tr);
                FALLTHROUGH
            case OP_PERIOD:
            case OP_POINTER:
            default:
                return retTree;
        }
        next();
    }

    return retTree;
}


Tree *Parser::c_parser_expr_func_call(Env &env, Tree *funcID) const
{
    const Type *paraList = &funcID->getType().getParaList();
    next();
    char buf[20];
    for (; paraList != nullptr; paraList = &paraList->getParaList()) {
        const Tree *arg = c_parser_expressions_2(env);
        const char *argString = nullptr;
        if (arg->t != nullptr) {
            argString = arg->t->toString();
        } else {
            argString = arg->toString();
        }
        sprintf(buf, "\tpara %s\n", argString);
        new IR(strdup(buf));
        if (cur == OP_COMMA) {
            next();
            continue;
        } else if (cur == OP_RIGHTBRACK) {
            next();
            break;
        }
    }
    sprintf(buf, "\tcall %s\n", funcID->getIDName());

    new IR(strdup(buf));
    return funcID;
}


Tree *Parser::c_parser_primary_expr(Env &env) const
{
    Tree *tp = nullptr;
    const char *pc;
    switch (cur.token_type) {
        case T_ID:
            pc = cur.token_value.id_name;
            tp = Tree::IDtree(env, pc);
            if (tp == nullptr) {
                char buf[40];
                sprintf(buf, "EE01 %%d: undeclared identifier %s\n", pc);
                parserError(PAR_ERR_INT_STR, buf, cur.token_pos->line);
            }
            break;
        case T_INT_CON:
            tp = Tree::CONSTtree(cur.token_value.numVal->val.i_value);
            break;
        case T_FLOAT_CON:
            break;
        case T_STRING:
            break;
        case T_OPERATOR: {
            if (tools::matchOP(cur, OP_LEFTBRACK)) {
                next();
                Expr *expr = c_parser_expressions(env);
                if (Tree::isCmpTree(&expr->getTree())) {
                    assert(0);
                }
                tp = const_cast<Tree *>(&expr->getTree());
                pushBack();
                need(OP_RIGHTBRACK);
                break;
            }
        }
    }
    next();
    return tp;
}

char *ExprExcetion::esprintf(const char *format, ...)
{
    const char *p = format;
    va_list ap;
    va_start(ap, format);
    char buffer[100];
    char *buf = buffer;

    for (; *p != '\0'; p++, buf++) {
        if (*p == '%') {
            if (*(p + 1) == 's') {
                const char *s = va_arg(ap, const char *);
                buf += sprintf(buf, "%s", s);
                buf--;
                p++;
                continue;
            } else if (*(p + 1) == 'd') {
                int i = va_arg(ap, int);
                sprintf(buf, "%d", i);
                buf++;
                p++;
                continue;
            }
        }
        *buf = *p;
    }
    *(buf) = '\0';
    char *ret = new char[strlen(buffer) + 1];
    strcpy(ret, buffer);
    return ret;
}
