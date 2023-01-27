#ifndef EXPR_H
#define EXPR_H

#include "exception.h"
#include "semantic.h"
#include "tree.h"
#include "type.h"

NAMESPACE_V1_BEGIN
class Expr : public Block
{
    Tree *exprTree;

    void printIR(void) {}

public:
    Expr(Tree *tree = nullptr) : Block()
    {
        exprTree       = tree;
        /*const char *pc = Label::nextLabel();
		char *buf = new char[strlen(pc) + 5];
		sprintf(buf, "%s : \n", pc);
		IR *ir = new IR(buf);*/
        this->nextList = IRList::makeIRList(*new IR(""));
    }


    ~Expr() {}

    IR *appendIR(IR *)
    {
        return 0;
    }

    Tree *setTree(Tree *tree)
    {
        exprTree = tree;
        return tree;
    }

    const Tree &getTree(void)
    {
        return *exprTree;
    }
};
NAMESPACE_END

#endif
