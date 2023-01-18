#ifndef STMT_H
#define STMT_H

#include "semantic.h"
NAMESPACE_V1_BEGIN

class Stmt : public Block
{
public:
    Stmt() : Block() {}
};

class Stmt_Jump : public Stmt
{
public:
    Label *trueEntry;
    Label *falseEntry;
    Stmt_Jump() : trueEntry(nullptr), falseEntry(nullptr) {}
};

class Stmt_Select : public Stmt
{
};

class Stmt_Iter : public Stmt
{
};

class Stmt_Block : public Stmt
{
};


enum STMT_TYPE { ST_LABELED, ST_COMPOUND, ST_EXPR, ST_SELECT, ST_JUMP };

NAMESPACE_END

#endif
