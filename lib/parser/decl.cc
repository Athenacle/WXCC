/* syntax parser-declaration.
 * WangXiaochun
 * zjjhwxc(at)gmail.com
 * April 4, 2013
 */

#include "system.h"
#include "lex/lexer.h"
#include "type.h"
#include "symbol.h"
#include "parser.h"
#include "exception.h"
#include "identifier.h"

#include "lex/tools.h"

using namespace lex;
using namespace constants;
using namespace scope;
using namespace ::lex::tools;
using namespace type_operator;

namespace
{
    const int DE_NEED_COMMA = 1;
    const int DE_INVALID_DECL = 2;
    const int DE_MORE_DECL_WITH_FUNC = 3;
    const int DE_LACK_FUNC_PROTO = 4;

    const char *declError[] = {
        "NULL",

        "DE01. Line %d: may lose comma in the function prototype.\n",
        // DE_NEED_COMMA

        "DE02. Line %d: invalid declaration of \'%s\'.\n",
        //DE_INVALID_DECL

        "DE03. Line %d: cannot declare any more variables with a function definition.\n",
        // DE_MORE_DECL_WITH_FUNC

        "DE04. Line %d: found a '{' in the file scope. Lack the function prototype?\n"
        // DE_LACK_FUNC_PROTO
    };

    const int DW_LACK_PARA_LIST = 1;
    const char *declWarning[] = {"NULL",

                                 "DW01. Line %d: lack of parameter list. Assert as void.\n"};
};  // namespace

int Parser::c_parser_translation_unit(Env &env) const
{
    int lexRet;
    int n = 0;
    lexRet = next();
    Env *curEnv = &env;
    for (; lexRet != EOT; n++) {
        if (matchQualifier(*cur) || isBaseType(*cur)) {
            c_parser_declaration(*curEnv, scope::S_GLOBAL);
        } else if (matchOP(*cur, OP_LEFTBRACE)) {
            Env nextEnv;
            env = env.EnterEnv(nextEnv);
        }
        lexRet = next();
    }
    if (n == 0)
        parserWarning(PAR_WARNING_EMPTY_TRANSLATION_UNIT);
    return 0;
}

int Parser::c_parser_declaration(Env &env, scope::Scope sc) const
{
    int declared = 0;
    isFinish = false;
    pushBack();
    Type *ty = c_parser_declaration_specifiers(nullptr);
    ST *nST = nullptr;
meet_comma:
    nST = c_parser_declarator(ty);
    if (nST == nullptr) {
        parserError(PAR_ERR_NEED_VAR_D, cur->token_pos->line);
        goto typeErrFinish;
    } else {
        nST->first->setScope(sc, env.getLevel());
        const TypeException &tyChk = Type::check(*nST->second);
        Identifier *id = nullptr;
        if (tyChk.no_exception()) {
            id = new Identifier(*nST->first, nST->second);
            env.newIdentifier(id);
            declared++;
        } else {
            parserError(PAR_ERR_STR_INT, tyChk.toString(), cur->token_pos->line);
        }
        next();
        if (matchOP(*cur, OP_EQ)) {
            c_parser_expressions(env);
        } else if (matchOP(*cur, OP_COMMA)) {
            goto meet_comma;
        } else if (matchOP(*cur, OP_LEFTBRACE)) {
            if (declared != 1)
                parserError(PAR_ERR_INT, declError[DE_MORE_DECL_WITH_FUNC], cur->token_pos->line);
            else {
                MAYBE_UNUSED ST *retST = c_parser_funtion_definition(env, *id);
            }
        } else {
            pushBack();
            if (!need(OP_SEMICOLON)) {
            typeErr:
                next();
                if (matchOP(*cur, OP_SEMICOLON))
                    goto typeErrFinish;
                if (!isBaseType(*cur)) {
                    goto typeErr;
                }
                if (isID(*cur))
                    goto meet_comma;
                //错误处理准则： 不断丢弃输入，直至遇到分号或标识符
            }
        }
    }
typeErrFinish:
    isFinish = true;
    return declared;
}

Type *Parser::c_parser_declaration_specifiers(Type *ty) const
{
    ty = c_parser_type_qualifier_list(ty);
    return c_parser_declaration_type_specifiers(ty);
}

Type *Parser::c_parser_declaration_type_specifiers_opt(Type *ty) const
{
    next();
    if (isBaseType(*cur)) {
        Type *nextType = c_parser_declaration_type_specifiers_opt(ty);
        ty = new Type(key2to[cur->token_value.keyword], nextType);
    } else {
        pushBack();
    }
    return ty;
}

Type *Parser::c_parser_declaration_type_specifiers(Type *ty) const
{
    next();
    Token to(*cur);
    //next();
    if (!isBaseType(*cur))  //whether the cur is the base type.int,char,etc.
    {
        parserError(PAR_ERR_NEED_TYPE);
    } else {
        Type *nextType = c_parser_declaration_type_specifiers_opt(ty);
        Type *retType = new Type(key2to[to.token_value.keyword], nextType);
        const TypeException &checkRet = Type::check(*retType);
        if (checkRet.no_exception()) {
            return retType;
        } else {
            parserError(PARER_WE_USE_STRING, checkRet.toString());
        }
    }
    return NS_BASE_TYPE::intType;
}

ST *Parser::c_parser_declarator(Type *ty) const
{
    Type *newTy = c_parser_pointer(ty);
    ST *retST = c_parser_direct_declarator(newTy);

    return retST;
}

Type *Parser::c_parser_pointer(Type *ty) const
{
    Type *pType = ty;
cpp:
    next();
    if (matchOP(*cur, OP_MULT)) {
        pType = new Type(TO_POINTER, pType);
        pType = c_parser_type_qualifier_list(pType);
        goto cpp;
    } else if (matchQualifier(*cur)) {
        pushBack();
        pType = c_parser_type_qualifier_list(pType);
    } else {
        pushBack();
    }
    return pType;
}

Type *Parser::c_parser_type_qualifier_list(Type *ty) const
{
    Type *retType = ty;
cptql:
    next();

    if (matchQualifier(*cur)) {
        TYPE_OPERATOR qType;
        if (cur->token_value.keyword == KEY_CONST)
            qType = TO_CONST;
        else
            qType = TO_VOLATILE;
        retType = new Type(qType, retType);
        goto cptql;
    } else {
        pushBack();
    }
    return retType;
}

ST *Parser::c_parser_direct_declarator(Type *ty) const
{
    ST *retST = nullptr;
    Token *curTo = nullptr;
    Type *retTY = nullptr;
    next();
    if (isID(*cur)) {
        curTo = mkToken();
        retTY = c_parser_direct_declatator_opt(ty);
        Symbol *sy = new Symbol(curTo, S_GLOBAL, ST_AUTO, S_GLOBAL);
        retST = new ST(sy, retTY);
    } else {
        if (matchOP(*cur, OP_LEFTBRACK)) {
            retST = c_parser_declarator(ty);
            need(OP_RIGHTBRACK);
            retTY = c_parser_direct_declatator_opt(ty);
            retTY = new Type(retST->second->getTYOP(), retTY);
            retST->second = retTY;
        } else if (matchOP(*cur, OP_LEFTBRACK)) {
            retTY = c_parser_parameter_type_list();
            retTY = new Type(TO_FUNCTION, retTY);
        }
    }
    return retST;
}

Type *Parser::c_parser_direct_declatator_opt(Type *ty) const
{
    Type *retTY = ty;
    next();
    if (matchOP(*cur, OP_LEFTSQBRAC))  // DD' -> '[' INT ']' DD'
    {
        if (need(T_INT_CON)) {
            int arrSize = cur->token_value.numVal->val.i_value;
            retTY = new Type(TO_ARRAY, ty, arrSize);
        }
        need(OP_RIGHTSQBRAC);
        retTY = c_parser_direct_declatator_opt(retTY);
    } else if (matchOP(*cur, OP_LEFTBRACK))  // DD' -> '('
    {
        Type *funcType = retTY;
        retTY = c_parser_parameter_type_list();
        retTY = new Type(TO_FUNCTION, funcType, 0, retTY, NO_LIMIT);
    } else {
        retTY = ty;
        pushBack();
    }

    return retTY;
}

//PARAMETER-DECLARATION:		形参声明
//	DECLARATION-SPECIFIERS DECLARATOR
//	DECLARATION-SPECIFIERS [ ABSTRACT-DECLARATOR ]

Type *Parser::c_parser_parameter_type_list(void) const
{
    Type *parameterList = nullptr;
    next();
    if (matchOP(*cur, OP_RIGHTBRACK))  // no parameter. eg. void foo();
    {
        parameterList = NS_BASE_TYPE::voidType;
        parserWarning(PARER_WE_USE_STRING, declWarning[DW_LACK_PARA_LIST], cur->token_pos->line);
    } else {
        if (matchKEY(*cur, KEY_KVOID) || matchKEY(*cur, KEY_TVOID)) {
            parameterList = NS_BASE_TYPE::voidType;
            need(OP_RIGHTBRACK);
        } else
        //DECLARATION-SPECIFIERS DECLARATOR
        {
            pushBack();
        paraList:
            Type *retTY = c_parser_declaration_specifiers(nullptr);
            ST *retST = c_parser_declarator(retTY);
            if (retST != nullptr) {
                retST->second =
                    new Type(TO_FUNCPARA, retST->second, 0, parameterList, 4, 4, retST->first);
                //retST->second->setSym(retST->first);
                //Type::setAsPara(*(retST->second));
                if (parameterList == nullptr)
                    parameterList = retST->second;
                else
                    parameterList->nextPara(retST->second);
            }
            next();
            if (matchOP(*cur, OP_COMMA)) {
                goto paraList;
            } else if (matchQualifier(*cur) || isBaseType(*cur)) {
                parserError(PARER_WE_USE_STRING, declError[DE_NEED_COMMA], cur->token_pos->line);
            } else if (matchOP(*cur, OP_RIGHTBRACK)) {
                ;
            }
        }
    }

    return parameterList;
}