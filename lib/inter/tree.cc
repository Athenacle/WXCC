#include "tree.h"

#include "expr.h"
#include "type.h"

using namespace NS_EXPRE_IR;

namespace
{
    const int UNDECLARED_ID = 1;
    const int NOTARRAY = 2;
    const int NEED_LVALUE = 3;

    const char *treeError[] = {"NULL",

                               "EE.  : undeclared identifier \'%s\'.\n",

                               "EE.  : not an array.\n",

                               "EE.  : need a modifiable l-value."};
};  // namespace

BinFunction NS_EXPRE_IR::op2fun(const OP _op)
{
    switch (_op) {
        case OP_PLUS:
            return Tree::PLUStree;
        case OP_MINUS:
            return Tree::MINUStree;
        case OP_MOD:
            return Tree::MODtree;
        case OP_BITOR:
            return Tree::BITORtree;
        case OP_BITAND:
            return Tree::BITANDtree;
        case OP_MULT:
            return Tree::MULTtree;
        case OP_BITXOR:
            return Tree::BITXORtree;
        case OP_DIV:
            return Tree::DIVtree;
        case OP_LEFTSHIFT:
            return Tree::SHLtree;
        case OP_RIGHTSHIFT:
            return Tree::SHRtree;
        case OP_LE:
            return Tree::LEtree;
        case OP_GT:
            return Tree::GTtree;
        case OP_LT:
            return Tree::LTtree;
        case OP_GE:
            return Tree::GEtree;
        case OP_EQ:
            return Tree::EQtree;
        case OP_NE:
            return Tree::NEtree;
        default:
            assert(0);
    }
    return nullptr;
}

Tree *Tree::IDtree(Env &env, const char *IDname)
{
    Identifier *id = env.findID(IDname);
    if (id == nullptr)
        id = env.findFUNC(IDname);
    if (id == nullptr)
        return nullptr;
    id->useID();
    Tree *tp = new Tree(NOP_ID, id->getType());
#ifdef GD_OUTPUT
    tp->treeString = const_cast<char *>(id->getIDname());
#endif
    tp->setValue(IDname);

    return tp;
}

Tree *Tree::CONSTtree(int i)
{
    Tree *tp = new Tree(NOP_CNST, *NS_BASE_TYPE::intType, nullptr, nullptr);
    tp->setValue(i);
#ifdef GD_OUTPUT
    char buf[20];
    // itoa(i, buf, 10);
    snprintf(buf, 20, "%d", i);
    tp->treeString = strdup(buf);
#endif
    return tp;
}

Tree *Tree::ARRAYtree(Tree *ty, Tree *subscript)
{
    const Type &arrayBaseType = ty->getType();
    int elementSize = arrayBaseType.getBaseType().getSize();
    Tree *offset = Tree::CONSTtree(elementSize);
    offset = Tree::MULTtree(offset, subscript);
    MAYBE_UNUSED const char *multStr = offset->t->toString();
    offset = Tree::PLUStree(ty, offset);
    Tree *ret = Tree::INDRtree(offset, arrayBaseType.getBaseType());
    return ret;
}

Tree *Tree::INDRtree(const Tree *tree, const Type &ty)
{
    Tree *ret = new Tree(NOP_INDR, ty, tree);
#ifdef GD_OUTPUT
    char buf[20];
    const char *indr = tree->t->toString();
    ret->t = new TemportaryVariable();
    sprintf(buf, "\t%s = [ %s ]\n", ret->t->toString(), indr);
    new IR(strdup(buf));
#endif
    return ret;
}

Tree *Tree::ASGNtree(const Tree *left, const Tree *right)
{
    Tree *ret = nullptr;
    const Type &t = left->getType();
    if (t.getTYOP() == TO_ARRAY || t.getTYOP() == TO_FUNCTION) {
        ExprExcetion *ee = new ExprExcetion();
        char *r = ExprExcetion::esprintf(treeError[NEED_LVALUE]);
        ee->setError(r);
        throw ee;
    } else {
        ret = new Tree(NOP_ASGN, left->getType(), left, right);
        char buf[20];
        const char *rightStr = nullptr;
        const char *leftStr = nullptr;
        if (right->t != nullptr)
            rightStr = right->t->toString();
        else
            rightStr = right->toString();

        if (left->t != nullptr)
            leftStr = left->t->toString();
        else
            leftStr = left->toString();

        sprintf(buf, "\t%s = %s\n", leftStr, rightStr);
        IR *ir = new IR(strdup(buf));
        ret->codebegin = ir;
    }
    return ret;
}

Tree *Tree::LEAtree(Tree *tr, Type &ty)
{
    Tree *ret = new Tree(NOP_LEA, ty, tr);
#ifdef IR_DEBUG
    strcpy(ret->nodeName, "LEA");
#endif
    return ret;
}

void Tree::emit(const char *string, FILE *out)
{
    fprintf(out, "%s", string);
}

Tree *Tree::PLUStree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_ADD, retT, left, right);
    ret->t = new TemportaryVariable();
#ifdef GD_OUTPUT
    char buf[20];
    const char *rightStr, *leftStr;
    if (right->t != nullptr)
        rightStr = right->t->toString();
    else
        rightStr = right->toString();

    if (left->t != nullptr)
        leftStr = left->t->toString();
    else
        leftStr = left->toString();


    sprintf(buf, "\t%s = %s + %s\n", ret->t->toString(), leftStr, rightStr);
    new IR(strdup(buf));
#endif
    return ret;
}

Tree *Tree::MINUStree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_SUB, retT, left, right);
    ret->t = new TemportaryVariable();
#ifdef GD_OUTPUT
    char buf[20];
    const char *rightStr, *leftStr;
    if (right->t != nullptr)
        rightStr = right->t->toString();
    else
        rightStr = right->toString();

    if (left->t != nullptr)
        leftStr = left->t->toString();
    else
        leftStr = left->toString();


    sprintf(buf, "\t%s = %s - %s\n", ret->t->toString(), leftStr, rightStr);
    new IR(strdup(buf));
#endif
    return ret;
}

Tree *Tree::CONDtree(Tree *, const Tree *, const Tree *)
{
    // __asm{int 3}
    return 0;
}


Tree *Tree::MODtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_MOD, retT, left, right);
    ret->t = new TemportaryVariable();
#ifdef GD_OUTPUT
    char buf[20];
    const char *rightStr, *leftStr;
    if (right->t != nullptr)
        rightStr = right->t->toString();
    else
        rightStr = right->toString();

    if (left->t != nullptr)
        leftStr = left->t->toString();
    else
        leftStr = left->toString();


    sprintf(buf, "\t%s = %s %% %s\n", ret->t->toString(), leftStr, rightStr);
    new IR(strdup(buf));
#endif
    return ret;
}
Tree *Tree::BITORtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_BOR, retT, left, right);
    ret->t = new TemportaryVariable();
#ifdef GD_OUTPUT
    char buf[20];
    const char *rightStr, *leftStr;
    if (right->t != nullptr)
        rightStr = right->t->toString();
    else
        rightStr = right->toString();

    if (left->t != nullptr)
        leftStr = left->t->toString();
    else
        leftStr = left->toString();


    sprintf(buf, "\t%s = %s | %s\n", ret->t->toString(), leftStr, rightStr);
    new IR(strdup(buf));
#endif
    return ret;
}

Tree *Tree::BITANDtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_ADD, retT, left, right);
    ret->t = new TemportaryVariable();
#ifdef GD_OUTPUT
    char buf[20];
    const char *rightStr, *leftStr;
    if (right->t != nullptr)
        rightStr = right->t->toString();
    else
        rightStr = right->toString();

    if (left->t != nullptr)
        leftStr = left->t->toString();
    else
        leftStr = left->toString();


    sprintf(buf, "\t%s = %s & %s\n", ret->t->toString(), leftStr, rightStr);
    new IR(strdup(buf));
#endif
    return ret;
}

Tree *Tree::MULTtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_MUL, retT, left, right);
    ret->t = new TemportaryVariable();
#ifdef GD_OUTPUT
    char buf[20];
    const char *rightStr, *leftStr;
    if (right->t != nullptr)
        rightStr = right->t->toString();
    else
        rightStr = right->toString();

    if (left->t != nullptr)
        leftStr = left->t->toString();
    else
        leftStr = left->toString();


    sprintf(buf, "\t%s = %s * %s\n", ret->t->toString(), leftStr, rightStr);
    new IR(strdup(buf));
#endif
    return ret;
}

Tree *Tree::BITXORtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_XOR, retT, left, right);
    ret->t = new TemportaryVariable();
#ifdef GD_OUTPUT
    char buf[20];
    const char *rightStr, *leftStr;
    if (right->t != nullptr)
        rightStr = right->t->toString();
    else
        rightStr = right->toString();

    if (left->t != nullptr)
        leftStr = left->t->toString();
    else
        leftStr = left->toString();


    sprintf(buf, "\t%s = %s ^ %s\n", ret->t->toString(), leftStr, rightStr);
    new IR(strdup(buf));
#endif
    return ret;
}

Tree *Tree::DIVtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_DIV, retT, left, right);
    ret->t = new TemportaryVariable();
#ifdef GD_OUTPUT
    char buf[20];
    const char *rightStr, *leftStr;
    if (right->t != nullptr)
        rightStr = right->t->toString();
    else
        rightStr = right->toString();

    if (left->t != nullptr)
        leftStr = left->t->toString();
    else
        leftStr = left->toString();


    sprintf(buf, "\t%s = %s / %s\n", ret->t->toString(), leftStr, rightStr);
    new IR(strdup(buf));
#endif
    return ret;
}

Tree *Tree::SHLtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_SHL, retT, left, right);
    ret->t = new TemportaryVariable();
#ifdef GD_OUTPUT
    char buf[20];
    const char *rightStr, *leftStr;
    if (right->t != nullptr)
        rightStr = right->t->toString();
    else
        rightStr = right->toString();

    if (left->t != nullptr)
        leftStr = left->t->toString();
    else
        leftStr = left->toString();


    sprintf(buf, "\t%s = %s <<%s\n", ret->t->toString(), leftStr, rightStr);
    new IR(strdup(buf));
#endif
    return ret;
}

Tree *Tree::SHRtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_SHR, retT, left, right);
    ret->t = new TemportaryVariable();
#ifdef GD_OUTPUT
    char buf[20];
    const char *rightStr, *leftStr;
    if (right->t != nullptr)
        rightStr = right->t->toString();
    else
        rightStr = right->toString();

    if (left->t != nullptr)
        leftStr = left->t->toString();
    else
        leftStr = left->toString();


    sprintf(buf, "\t%s = %s >> %s\n", ret->t->toString(), leftStr, rightStr);
    new IR(strdup(buf));
#endif
    return ret;
}

Tree *Tree::LEtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_LE, retT, left, right);
    char buf[40];
    sprintf(buf, "\tif %s <= %s goto %%s \n", left->toString(), right->toString());
    IR *trueIR = new IR(strdup(buf));
    IR *falseIR = new IR("\tgoto %s \n");
    ret->codebegin = trueIR;
    ret->trueList = IRList::makeIRList(*trueIR);
    ret->falseList = IRList::makeIRList(*falseIR);
    return ret;
}

Tree *Tree::GTtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_GREAT, retT, left, right);
    char buf[40];
    sprintf(buf, "\tif %s > %s goto %%s \n", left->toString(), right->toString());
    IR *trueIR = new IR(strdup(buf));
    IR *falseIR = new IR("\tgoto %s \n");
    ret->codebegin = trueIR;
    ret->trueList = IRList::makeIRList(*trueIR);
    ret->falseList = IRList::makeIRList(*falseIR);
    return ret;
}


Tree *Tree::LTtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_LESS, retT, left, right);
    char buf[40];
    sprintf(buf, "\tif %s < %s goto %%s \n", left->toString(), right->toString());
    IR *trueIR = new IR(strdup(buf));
    IR *falseIR = new IR("\tgoto %s \n");
    ret->codebegin = trueIR;
    ret->trueList = IRList::makeIRList(*trueIR);
    ret->falseList = IRList::makeIRList(*falseIR);
    return ret;
}

Tree *Tree::GEtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_GE, retT, left, right);
    char buf[40];
    sprintf(buf, "\tif %s >= %s goto %%s \n", left->toString(), right->toString());
    IR *trueIR = new IR(strdup(buf));
    IR *falseIR = new IR("\tgoto %s \n");
    ret->codebegin = trueIR;
    ret->trueList = IRList::makeIRList(*trueIR);
    ret->falseList = IRList::makeIRList(*falseIR);
    return ret;
}


Tree *Tree::NEtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_DIV, retT, left, right);
    char buf[40];
    sprintf(buf, "\tif %s <> %s goto %%s \n", left->toString(), right->toString());
    IR *trueIR = new IR(strdup(buf));
    IR *falseIR = new IR("\tgoto %s \n");
    ret->codebegin = trueIR;
    ret->trueList = IRList::makeIRList(*trueIR);
    ret->falseList = IRList::makeIRList(*falseIR);
    return ret;
}

Tree *Tree::EQtree(const Tree *left, const Tree *right)
{
    const Type &retT = Type::binaryOp(left->getType(), right->getType());
    Tree *ret = new Tree(NOP_EQ, retT, left, right);
    char buf[40];
    sprintf(buf, "\tif %s = %s goto %%s \n", left->toString(), right->toString());
    IR *trueIR = new IR(strdup(buf));
    IR *falseIR = new IR("\tgoto %s \n");
    ret->codebegin = trueIR;
    ret->trueList = IRList::makeIRList(*trueIR);
    ret->falseList = IRList::makeIRList(*falseIR);
    return ret;
}

Tree *Tree::ORtree(Tree *left, Tree *right)
{
    if (!isCmpTree(left) && !isLogTree(left)) {
        left = Tree::EQtree(left, Tree::CONSTtree(0));
    }
    if (!isCmpTree(right) && !isLogTree(right)) {
        right = Tree::EQtree(right, Tree::CONSTtree(0));
    }
    Tree *ret = new Tree(NOP_LOR, *NS_BASE_TYPE::intType, left, right);
    Label *la = new Label();
    MAYBE_UNUSED IR *ir = new IR(*la);

    IR *firstGOTO = new IR("%s : \n", false);
    IR::appendIR(right->codebegin->preIR, firstGOTO);
    IRList *firstGOTOLabel = IRList::makeIRList(*firstGOTO);
    IRList::mergeChain(*left->falseList, *firstGOTOLabel);

    IRList::backPatch(*left->falseList, *la);
    ret->trueList = IRList::mergeChain(*left->trueList, *right->trueList);
    ret->falseList = right->falseList;
    ret->codebegin = left->codebegin;
    return ret;
}

Tree *Tree::ANDtree(Tree *left, Tree *right)
{
    if (!isCmpTree(left) && !isLogTree(left)) {
        left = Tree::EQtree(left, Tree::CONSTtree(0));
    }
    if (!isCmpTree(right) && !isLogTree(right)) {
        right = Tree::EQtree(right, Tree::CONSTtree(0));
    }
    Tree *ret = new Tree(NOP_LAND, *NS_BASE_TYPE::intType, left, right);
    Label *la = new Label();

    IR *firstGOTO = new IR("%s : \n", false);
    IR::appendIR(right->codebegin->preIR, firstGOTO);
    IRList *firstGOTOLabel = IRList::makeIRList(*firstGOTO);
    IRList::mergeChain(*left->trueList, *firstGOTOLabel);

    IRList::backPatch(*left->trueList, *la);
    ret->trueList = right->trueList;
    ret->falseList = IRList::mergeChain(*left->falseList, *right->falseList);
    return ret;
}

const char *Tree::tmpString(void) const
{
    return t->toString();
}

const char *Tree::toString(void) const
{
    return this->treeString;
}

bool Tree::isCmpTree(const Tree *t)
{
    switch (t->getNOP()) {
        case NOP_EQ:     //==
        case NOP_NE:     //!=
        case NOP_LE:     //<=
        case NOP_GE:     //>=
        case NOP_LESS:   //<
        case NOP_GREAT:  //>
            return true;
        default:
            return false;
    }
}

bool Tree::isLogTree(const Tree *t)
{
    return t->getNOP() == NOP_LAND || t->getNOP() == NOP_LOR;
}
