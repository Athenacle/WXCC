//#include "../includes/Env.h"


#include "lex/lexer.h"

#include "exception.h"
#include "identifier.h"
#include "parser.h"
#include "semantic.h"
#include "symbol.h"
#include "system.h"
#include "table.h"
#include "type.h"

USING_V1

using namespace scope;

namespace
{
    const int teErrRedecl = 1;

    const char* tableError[] = {
        "NULL",

        "TE01. Line: %d. redeclare of variable \'%s\'. \n"
        "\t\tLast declaration is at line %d.\n",
        // TE_ERR_REDECL
    };
}  // namespace

Env& Env::ExitEnv()
{
    Env* ret = this->up;
    this->down = nullptr;
    if (this->up != nullptr) {
        this->up->down = nullptr;
        ret->scope_level--;
    }
    localFunc.exitBlock();
    localID.exitBlock();

    return *ret;
}


void Env::newIdentifier(Identifier* id)
{
    if (id->isID()) {
        Identifier* idret = localID.existCurrentScope(id->getIDName());
        if (idret == nullptr) {
            localID.PutIntoTable(id, id->getIDName());
            idCounts++;
        } else {
            semError(USE_MY_STR_SSD,
                     tableError[teErrRedecl],
                     id->getSymbolLine(),
                     id->getIDName(),
                     (idret)->getSymbolLine());
        }
    } else {
        Identifier* funcRet = localFunc.existCurrentScope(id->getIDName());
        if (funcRet == nullptr) {
            localFunc.PutIntoTable(id, id->getIDName());
            funcCounts++;
        } else {
            semError(USE_MY_STR_SSD,
                     tableError[teErrRedecl],
                     id->getSymbolLine(),
                     id->getIDName(),
                     (funcRet)->getSymbolLine());
        }
    }
}

Env& Env::EnterEnv(Env& downEnv)
{
    down = &downEnv;
    downEnv.up = this;

    localFunc.enterBlock(&(downEnv.localFunc));
    localID.enterBlock(&(downEnv.localID));

    downEnv.scope_level++;
    downEnv.sc = S_LOCAL;
    return downEnv;
}
