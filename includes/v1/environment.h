/* Environment header.
 * WangXiaochun
 * zjjhwxc(at)gmail.com
 * April 4, 2013
 */

#ifndef ENV_H
#define ENV_H

#include <cstring>

#include "identifier.h"
#include "symbol.h"
#include "table.h"

NAMESPACE_V1_BEGIN

using namespace scope;
using namespace NS_TABLE;

typedef Table<Identifier*, const char*> idTable;
typedef Table<Symbol*, const char*> strTable;


class Env
{
    Env* up;    // more global
    Env* down;  // more local

    Scope sc;
    int scope_level;

    int idCounts;
    int funcCounts;

    idTable localID;
    idTable localFunc;

    Env& setDownEnv(Env& env)
    {
        this->down = &env;
        return *this;
    }

public:
    Env(Env* _up = nullptr, Env* _down = nullptr, Scope _sc = S_LOCAL, int _level = 0)
        : localID(), localFunc()
    {
        up = _up;
        down = _down;
        sc = _sc;
        scope_level = _level + 1;

        idCounts = 0;
        funcCounts = 0;
    }

    Env& EnterEnv(Env& downEnv);

    Env& ExitEnv(void);

    void newIdentifier(Identifier* id);

    void setGlobal(void)
    {
        this->scope_level = 0;
        this->sc = S_GLOBAL;
    }

    bool isGlobal(void)
    {
        return this->scope_level == 0;
    }

    int getLevel(void) const
    {
        return this->scope_level;
    }

    Identifier* findID(const char* name)
    {
        return this->localID.existAllScope(name);
    }

    Identifier* findFUNC(const char* name)
    {
        return this->localFunc.existAllScope(name);
    }
};

namespace NS_TABLE
{
    template <>
    class CMP<Identifier*, const char*>
    {
    public:
        static bool cmp(Identifier* id, const char* str)
        {
            return strcmp(id->getIDName(), str) == 0;
        }
    };
}  // namespace NS_TABLE
NAMESPACE_END

#endif
