//#include "../includes/Env.h"


#include "system.h"
#include "lex.h"
#include "type.h"
#include "symbol.h"
#include "parser.h"
#include "exception.h"
#include "identifier.h"
#include "table.h"
#include "semantic.h"


namespace
{
    const int TE_ERR_REDECL = 1;

    const char* tableError[] = {
        "NULL",

        "TE01. Line: %d. redeclare of variable \'%s\'. \n"
        "\t\tLast declaration is at line %d.\n",
        // TE_ERR_REDECL
    };
};  // namespace

Env& Env::ExitEnv(void)
{
    Env* ret = this->up;
    this->down = NULL;
    if (this->up != NULL) {
        this->up->down = NULL;
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
        if (idret == NULL) {
            localID.PutIntoTable(id, id->getIDName());
            idCounts++;
        } else {
            semError(USE_MY_STR_SSD,
                     tableError[TE_ERR_REDECL],
                     id->getSymbolLine(),
                     id->getIDName(),
                     (idret)->getSymbolLine());
        }
    } else {
        Identifier* funcRet = localFunc.existCurrentScope(id->getIDName());
        if (funcRet == NULL) {
            localFunc.PutIntoTable(id, id->getIDName());
            funcCounts++;
        } else {
            semError(USE_MY_STR_SSD,
                     tableError[TE_ERR_REDECL],
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
