#ifndef EXPR_H
#define EXPR_H

#include "type.h"
#include "tree.h"
#include "exception.h"
#include "semantic.h"

class Expr : public Block
{
    Tree *exprTree;

    void printIR(void) {}

public:
    Expr(Tree *tree = NULL) : Block()
    {
        exprTree = tree;
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


#endif