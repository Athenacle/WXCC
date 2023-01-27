/*
* semantic analyzer header.
* Wang Xiaochun
* zjjhwxc(at)gmail.com
* March 9, 2013
*
*/

#ifndef TREE_H
#define TREE_H

#include "system.h"

#include "lex/lexer.h"

#include "environment.h"
#include "type.h"

NAMESPACE_V1_BEGIN
namespace NS_EXPRE_IR
{
    enum Node_OP {
        NOP_ADD,
        NOP_MUL,
        NOP_NE,
        NOP_LE,
        NOP_GE,
        NOP_SHL,
        NOP_SUB,
        NOP_DIV,
        NOP_EQ,
        NOP_LESS,
        NOP_GREAT,
        NOP_SHR,
        NOP_LAND,
        NOP_XOR,
        NOP_NEG,
        NOP_LEA,
        NOP_LOR,
        NOP_NOT,
        NOP_BAND,
        NOP_BOR,
        NOP_MOD,

        NOP_CAST,
        NOP_INDR,
        NOP_COND,
        NOP_CNST,
        NOP_ARG,
        NOP_CALL,
        NOP_JMP,
        NOP_ASGN,
        NOP_ID
    };

    enum Node_TYPE {
        NT_BI,
        NT_WI,
        NT_DI,
        NT_QI,
        NT_SF,
        NT_DF,
        NT_LF,
    };

    enum Node_S { NS_U, US_S, US_NONE };
}  // namespace NS_EXPRE_IR

using namespace NS_EXPRE_IR;

class Tree  // only used for Expr
{
public:
    static void emit(const char *string, FILE *out = wxccErr);

    static void initOP2fun(void);

    const Type &getType(void) const
    {
        return resultType;
    }

    void printTree(FILE *fp = wxccErr) const;

    Tree(Node_OP _op, const Type &ty, const Tree *_left = nullptr, const Tree *_right = nullptr)
        : left(_left), right(_right), resultType(ty), nodeOP(_op)
    {
        conditionT = nullptr;
        x.i_val    = 0;
        t          = nullptr;
    }

    Tree(Node_OP _op, const Tree *_left = nullptr, const Tree *_right = nullptr)
        : left(_left), right(_right), resultType(_left->getType()), nodeOP(_op)
    {
        conditionT = nullptr;
        x.i_val    = 0;
        sign       = US_S;
        t          = nullptr;
    }

    Node_OP getNOP(void) const
    {
        return this->nodeOP;
    }

    void setValue(const char *str)
    {
        x.string = str;
        sign     = US_S;
    }

    void setValue(int i)
    {
        x.i_val = i;
        sign    = US_S;
    }

    const char *getIDName(void) const
    {
        return this->x.string;
    }

    void setValue(unsigned int i)
    {
        x.i_val = static_cast<int>(i);
        sign    = US_S;
    }

    void printTempValue(void) const
    {
        t->print();
    }

    const char *toString(void) const;

    const char *tmpString(void) const;

    //tree functions.
    static Tree *VALUEtree(Tree *);
    static Tree *PLUStree(const Tree *, const Tree *);
    static Tree *MINUStree(const Tree *, const Tree *);
    static Tree *MODtree(const Tree *, const Tree *);
    static Tree *BITORtree(const Tree *, const Tree *);
    static Tree *BITANDtree(const Tree *, const Tree *);
    static Tree *MULTtree(const Tree *, const Tree *);
    static Tree *BITXORtree(const Tree *, const Tree *);
    static Tree *DIVtree(const Tree *, const Tree *);
    static Tree *SHLtree(const Tree *, const Tree *);
    static Tree *SHRtree(const Tree *, const Tree *);
    static Tree *LEtree(const Tree *, const Tree *);
    static Tree *GTtree(const Tree *, const Tree *);
    static Tree *LTtree(const Tree *, const Tree *);
    static Tree *GEtree(const Tree *, const Tree *);
    static Tree *EQtree(const Tree *, const Tree *);
    static Tree *EQtree(const Tree *, const Tree *, Block &);
    static Tree *NEtree(const Tree *, const Tree *);
    static Tree *ORtree(Tree *, Tree *);
    static Tree *ANDtree(Tree *, Tree *);
    static Tree *ARRAYtree(Tree *, Tree *);
    static Tree *IDtree(Env &, const char *);
    static Tree *LEAtree(Tree *, Type &);
    static Tree *CONDtree(Tree *, const Tree *, const Tree *);
    static Tree *ASGNtree(const Tree *, const Tree *);
    static Tree *CONSTtree(int i);
    static Tree *INDRtree(const Tree *, const Tree *);
    static Tree *INDRtree(const Tree *, const Type &);
    static void printLogicTree(const Tree *);
    static IR *emitIR(const Tree *);

    IR *codebegin;

    IRList *trueList;
    IRList *falseList;

    TemportaryVariable *t;

    static bool isLogTree(const Tree *);
    static bool isCmpTree(const Tree *);

protected:
    const Tree *left;
    const Tree *right;
    const Tree *conditionT;

    const Type &resultType;
    Node_OP nodeOP;
    Node_S sign;
    Node_TYPE nodeType;

    IR *thisIR;

    union val {
        int i_val;
        double d_val;
        const char *string;
    } x;

    char *outString[20];

#ifdef IR_DEBUG
    char nodeName[12];
#endif

private:
    void setS(Node_S s)
    {
        this->sign = s;
    }
    void setT(Node_TYPE t)
    {
        this->nodeType = t;
    }

    char *treeString;
};

namespace NS_EXPRE_IR
{
    typedef Tree *(*BinFunction)(const Tree *, const Tree *);

    BinFunction op2fun(const OP _op);
}  // namespace NS_EXPRE_IR

NAMESPACE_END

#endif
