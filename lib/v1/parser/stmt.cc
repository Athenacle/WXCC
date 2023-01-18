/* syntax parser-stmtments.
* WangXiaochun
* zjjhwxc(at)gmail.com
* April 4, 2013
*/

#include "lex/lexer.h"
#include "lex/tools.h"

#include "exception.h"
#include "expr.h"
#include "functions.h"
#include "identifier.h"
#include "parser.h"
#include "symbol.h"
#include "system.h"
#include "type.h"

USING_V1
using namespace lex;
using namespace lex::tools;

namespace
{
    const int PE_DECL_AFTER_STMT = 1;

    const char *stmtErrors[] = {
        "NULL",

        "PE08. Line %d: meet a declaration after statements."
        // PE_DECL_AFTER_STMT
    };
}  // namespace

int Function::parseParameters(void)
{
    int paraNum = 0;
    const Type *paraList = parameterList;
    Identifier *parameter = nullptr;
    for (; paraList != nullptr; paraNum++) {
        if (paraList->getTYOP() == TO_VOID) {
            if (paraNum == 0) {
                break;
            } else {
                isVar = true;
                paraList = &paraList->getParaList();
                continue;
            }
        }
        parameter = new Identifier(*paraList->sym, paraList->base_type);
        local.newIdentifier(parameter);
        paraList = &paraList->getParaList();
    }
    return paraNum;
}

ST *Parser::c_parser_funtion_definition(Env &globalEnv, Identifier &id) const
{
    Env newEnv;
    Env &localEnv = globalEnv.EnterEnv(newEnv);
    ST *retST = nullptr;
    Function fun(id, localEnv);
    fprintf(wxccErr,
            "Function %s Begin. Type: %s.--------------------- \n\n",
            id.getIDname(),
            id.getType().getTYPEName());
    IR *blockBegin = new IR();
    MAYBE_UNUSED Block *retIR = c_parser_compound_statement(fun);
    IR::print(blockBegin);
    fprintf(wxccErr, "\nFunction %s End.----------------------------------\n", id.getIDname());
    need(OP_RIGHTBRACE);
    return retST;
}


Block *Parser::c_parser_compound_statement(Function &func) const
{
    Block *funcIR = c_parser_compound_statement(func.getFuncEnv());
    //pushBack();
    return funcIR;
}

Block *Parser::c_parser_compound_statement(Env &curEnv) const
{
    Env local;
    Env &curE = curEnv.EnterEnv(local);
    Block *blockIR = nullptr;
    Block *ret = new Block();
    ret->codeBegin = new IR("");
    ret->nextList = IRList::makeIRList(*new IR(""));
    ret->breakList = nullptr;
    next();
    for (; isBaseType(cur);) {
        c_parser_declaration(curE, Scope::S_LOCAL);
        next();
    }
    while (!matchOP(cur, OP_RIGHTBRACE)) {
        blockIR = c_parser_statement(curE);
        //pushBack();
        next();
        if (blockIR != nullptr) {
            if (blockIR->nextList != nullptr) {
                ret->nextList = IRList::mergeChain(*ret->nextList, *blockIR->nextList);
            }
            if (blockIR->breakList != nullptr) {
                if (ret->breakList == nullptr)
                    ret->breakList = blockIR->breakList;
                else
                    ret->nextList = IRList::mergeChain(*ret->breakList, *blockIR->breakList);
            }
            if (blockIR->continueList != nullptr) {
                if (ret->continueList == nullptr)
                    ret->continueList = blockIR->continueList;
                else
                    ret->continueList =
                        IRList::mergeChain(*ret->continueList, *blockIR->continueList);
            }
        }
        while (cur == OP_SEMICOLON)
            next();
    }

    pushBack();
    curE.ExitEnv();
    return ret;
}

Block *Parser::c_parser_select_statement(Env &env) const
{
    Stmt_Jump *stmt = new Stmt_Jump();
    if (cur == KEY_IF) {
        need(OP_LEFTBRACK);
        next();
        Expr *condExpr = c_parser_expressions(env);
        if (condExpr->getTree().getNOP() == NOP_ID) {
            condExpr = new Expr(Tree::EQtree(&condExpr->getTree(), Tree::CONSTtree(0)));
        }
        pushBack();
        need(OP_RIGHTBRACK);
        next();
        IR *tmpCur = IR::currentIR;
        Label *trueLabel = new Label();
        Block *trueStmt = c_parser_statement(env);
        IR *ir = new IR(*trueLabel);
        IR::appendIR(tmpCur, ir);
        IRList::backPatch(*condExpr->getTree().trueList, *trueLabel);

        //stmt->breakList = IRList::mergeChain(*condExpr->getTree().trueList, *IRList::makeIRList(*ir));
        stmt->nextList = condExpr->getTree().falseList;
        stmt->continueList = trueStmt->continueList;
        stmt->breakList = trueStmt->breakList;
        do {
            next();
        } while (cur == OP_SEMICOLON);
        if (cur == KEY_ELSE) {
            Label *la = new Label();
            MAYBE_UNUSED Label *lb = new Label();
            next();
            IR *irBefore = new IR("\tgoto %s \n", false);
            IR *falseGoto = new IR("\tgoto %s \n", false);
            IR *tmpIR = IR::currentIR;
            Block *falseStmt = c_parser_statement(env);
            //IR::appendIR(tmpCur->preIR, irBefore);
            IR *ir = new IR(*la, true);
            IR::appendIR(tmpIR->preIR, falseGoto);
            IR::appendIR(tmpIR->preIR, ir);
            IRList::mergeChain(*falseStmt->nextList, *IRList::makeIRList(*falseGoto));
            MAYBE_UNUSED IRList *listBeforeElse = IRList::makeIRList(*irBefore);
            IRList::backPatch(*condExpr->getTree().falseList, *la);
            IRList *tmp = IRList::mergeChain(*falseStmt->nextList, irBefore);
            stmt->nextList = IRList::mergeChain(*tmp, *falseStmt->nextList);

            if (falseStmt->breakList != nullptr) {
                if (stmt->breakList != nullptr) {
                    stmt->breakList = IRList::mergeChain(*stmt->breakList, *falseStmt->breakList);
                } else {
                    stmt->breakList = falseStmt->breakList;
                }
            }
            if (falseStmt->continueList != nullptr) {
                if (stmt->continueList != nullptr) {
                    stmt->continueList =
                        IRList::mergeChain(*stmt->continueList, *falseStmt->continueList);
                } else {
                    stmt->continueList = falseStmt->continueList;
                }
            }

        } else {
            stmt->nextList = condExpr->getTree().falseList;
            next();
            if (trueStmt != nullptr && trueStmt->nextList != nullptr)
                stmt->nextList =
                    IRList::mergeChain(*condExpr->getTree().falseList, *trueStmt->nextList);
        }
    } else if (cur == KEY_SWITCH) {
        assert(0);
    }

    return stmt;
}

Block *Parser::c_parser_statement(Env &curEnv) const
{
    Block *stmt = nullptr;
    if (matchOP(cur, OP_LEFTBRACE))  //meet '{' , a new block.
        return c_parser_compound_statement(curEnv);

stmt_list:
    if (isSelect(cur)) {
        stmt = c_parser_select_statement(curEnv);
        Label *endOfSelect = new Label();
        IR *ir = new IR(*endOfSelect);
        IR::appendIR(ir);
        IRList::backPatch(*stmt->nextList, *endOfSelect);
        pushBack();

    } else if (isJump(cur)) {
        stmt = c_parser_jump_statement(curEnv);

    } else if (isIter(cur)) {
        stmt = c_parser_iter_statement(curEnv);
    } else if (matchOP(cur, OP_RIGHTBRACE)) {
        goto comp_stmt_finish;  // meet '}', exit current block;
    } else if (isLabel(cur)) {
        stmt = c_parser_label_statement(curEnv);
    } else if (matchOP(cur, OP_SEMICOLON))  // a statements finishes
    {
        next();
        goto stmt_list;
    } else if (isBaseType(cur))  // declaration after statements.
    {
        parserError(PAR_ERR_INT_STR, stmtErrors[PE_DECL_AFTER_STMT], cur.token_pos.line);
    meet_type:
        //error recover. ignore all the base types.
        next();
        if (isBaseType(cur)) {
            goto meet_type;
        } else {
            goto stmt_list;
        }
    } else {
        if (matchKEY(cur, KEY_ELSE)) {
            parserError(PAR_ERR_UNMATCHED_KEY);
            next();
        }

        stmt = c_parser_expressions(curEnv);
        pushBack();
        do {
            next();
        } while (cur == OP_SEMICOLON);
        pushBack();
    }


comp_stmt_finish:

    return stmt;
}

Block *Parser::c_parser_iter_statement(Env &env) const
//for(;;);
//while(){}
//do{}while();
{
    Block *ret = new Block();
    if (matchKEY(cur, KEY_FOR)) {
        return c_parser_iter_for(env);
    } else if (matchKEY(cur, KEY_WHILE)) {
        return c_parser_iter_while(env);
    } else if (matchKEY(cur, KEY_DO)) {
    }

    return ret;
}

Block *Parser::c_parser_iter_for(Env &env) const
/*  for (a = 0; a < 10; a = a + 1)
 *  {	b = b + 1;		}
 *  c = 0;
 *
 */

/*	a = 0
 * L0:
 *	if a < 10 goto L1
 *	goto L2
 * L1:
 *	b = b + 1
 *	goto L0
 * L2:
 *	c = 0
 */

/*	a = 0
 * L0:				condBeginList
 *	if a < 10 goto L1
 *	goto L2
 * L1:				bodyBeginList
 *	b = b + 1
 * L2:
 *	a = a + 1		incrList
 *
 *	goto L0			backList
 * L3:				retList
 *	c = 0
 */
{
    MAYBE_UNUSED IRList *condBeginList, *bodyBeginList, *retList, *incrList, *backList;
    Block *ret = new Block();
    ret->nextList = IRList::makeIRList(*new IR(""));
    need(OP_LEFTBRACK);
    next();
    Expr *preExpr = c_parser_expressions(env);
    pushBack();
    need(OP_SEMICOLON);
    next();
    ret->codeBegin = preExpr->codeBegin;
    Label *condLabel = new Label();
    IR *condIR = new IR("%s: \n");
    condBeginList = IRList::makeIRList(*condIR);
    Expr *condExpr = c_parser_expressions(env);
    pushBack();
    need(OP_SEMICOLON);
    next();
    Label *incrLabel = new Label();
    Label *stmtLabel = new Label();
    IR *incrIR = new IR("%s : \n");
    incrList = IRList::makeIRList(*incrIR);
    IR *tmpCurrent = IR::currentIR;
    IR::currentIR = new IR();
    Expr *afterExpr = c_parser_expressions(env);
    pushBack();
    need(OP_RIGHTBRACK);
    next();
    IR *retIR = new IR("\tgoto %s \n");
    backList = IRList::makeIRList(*retIR);
    IRList::backPatch(*backList, *condLabel);
    IR *afterExprIR = IR::reSetCurrentFirst(afterExpr->getTree().codebegin)->nextIR;
    IR::currentIR = tmpCurrent;
    MAYBE_UNUSED IR *stmtIR = new IR(*stmtLabel);
    MAYBE_UNUSED Block *stmt = c_parser_statement(env);
    IR *bodyBeginIR = new IR("%s : \n");

    bodyBeginList = IRList::makeIRList(*bodyBeginIR);
    Label *afterBeginLable = new Label();
    IR::currentIR->nextIR = afterExprIR;
    afterExprIR->preIR = tmpCurrent;
    IR::reSetCurrent();
    Label *forEndLabel = new Label();

    IRList *nextStmt = IRList::makeIRList(*new IR("%s : \n"));
    IRList::backPatch(*condExpr->getTree().trueList, *condLabel);
    backList = ret->nextList =
        IRList::mergeChain(*condExpr->nextList, *condExpr->getTree().trueList);
    IRList::backPatch(*nextStmt, *forEndLabel);
    IRList::backPatch(*incrList, *incrLabel);
    IRList::backPatch(*backList, *condLabel);
    IRList::backPatch(*bodyBeginList, *afterBeginLable);
    IRList::backPatch(*condExpr->getTree().falseList, *forEndLabel);
    IRList::backPatch(*condExpr->getTree().trueList, *incrLabel);
    IRList::backPatch(*condBeginList, *condLabel);
    return ret;
}

Block *Parser::c_parser_jump_statement(Env &env) const
{
    Block *ret = new Block();
    if (matchKEY(cur, KEY_BREAK)) {
        IR *brLa = new IR("%s : \n");
        IR *brIR = new IR("\tgoto %s \n");
        IRList *brList = IRList::makeIRList(*brIR);
        ret->breakList = brList;
        ret->nextList = IRList::makeIRList(*brLa);
    } else if (matchKEY(cur, KEY_CONTINUE)) {
        IR *conLa = new IR("%s : \n");
        IR *conIR = new IR("\tgoto %s \n");
        IRList *conList = IRList::makeIRList(*conIR);
        ret->continueList = conList;
        ret->nextList = IRList::makeIRList(*conLa);
    } else if (matchKEY(cur, KEY_RETURN)) {
        assert(cur == KEY_RETURN);
        c_parser_expressions(env);
        new IR("\tret");
    }
    need(OP_SEMICOLON);

    return ret;
}


Block *Parser::c_parser_iter_while(Env &env) const
/*
 * while(a == 0)
 * {
 *	a = a + 1;
 * }
 * b = b + 1;
 *
 */

/*
 *L0:
 *	if a = 0 goto L1
 *	goto L2
 *L1
 *	a = a + 1
 *	goto L0
 *L2:
 *	b = b + 1
 */
{
    MAYBE_UNUSED IRList *f;
    Label *condLabel, *bodyLabel, *endLabel;
    IR *condIR, *bodyIR, *backIR, *endIR;
    Expr *condExpr;
    Block *stmt;
    assert(cur == KEY_WHILE);
    Block *ret = new Block();

    condLabel = new Label();  // L0:
    condIR = new IR("%s : \n");
    IRList::backPatch(*condIR, *condLabel);
    need(OP_LEFTBRACK);
    next();
    condExpr = c_parser_expressions(env);
    pushBack();
    need(OP_RIGHTBRACK);
    next();
    bodyLabel = new Label();  // L1:
    bodyIR = new IR("%s : \n");
    IRList::backPatch(*bodyIR, *bodyLabel);
    stmt = c_parser_statement(env);

    backIR = new IR("\tgoto %s \n");
    IRList::backPatch(*backIR, *condLabel);

    endLabel = new Label();
    endIR = new IR("%s : \n");

    if (stmt->breakList != nullptr) {
        IRList::backPatch(*stmt->breakList, *endLabel);
    }
    if (stmt->continueList != nullptr) {
        IRList::backPatch(*stmt->continueList, *condLabel);
    }
    IRList::backPatch(*endIR, *endLabel);
    IRList::backPatch(*condExpr->getTree().trueList, *bodyLabel);
    IRList::backPatch(*condExpr->getTree().falseList, *endLabel);
    return ret;
}
