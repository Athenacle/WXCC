/* syntax parser-function headers.
 * WangXiaochun
 * zjjhwxc(at)gmail.com
 * April 4, 2013
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "lex/lexer.h"

#include "environment.h"
#include "exception.h"
#include "identifier.h"
#include "parser.h"
#include "symbol.h"
#include "system.h"
#include "type.h"

class Function
{
    Identifier &funName;
    Env &local;
    //idTable parameterTable;
    int parameterCount;

    IR *chain;

    const Type *parameterList;
    const Type &retType;

    bool isVar;  //变参函数
    int parseParameters(void);

public:
    Function(Identifier &id, Env &globaler)
        : funName(id), local(globaler), retType(id.getBaseType())
    {
        IR();
        assert(id.ty->getTYOP() == TO_FUNCTION);
        chain = nullptr;
        isVar = false;
        parameterList = &id.ty->getParaList();
        parameterCount = parseParameters();
        fprintf(wxccErr, "\n");
    }

    Env &getFuncEnv(void) const
    {
        return this->local;
    }

    void setIR(IR *ir)
    {
        this->chain = ir;
    }
};


#endif