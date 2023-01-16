/* IR 
* WangXiaochun
* zjjhwxc(at)gmail.com
* April 4, 2013
*
*/
#include "semantic.h"

#include "stmt.h"
#include "system.h"
#include "tree.h"

using namespace std;

std::map<OP, Tree *(*)(Env &, Tree *, Tree *)> op2fun;


int TemportaryVariable::currentID = 0;

TemportaryVariable::TemportaryVariable()
{
    tempID = ++TemportaryVariable::currentID;
    idString[0] = 't';
    sprintf(idString, "t%d", currentID);
}

const char *TemportaryVariable::toString()
{
    return idString;
}

TemportaryVariable::~TemportaryVariable()
{
    ;
}

int Label::labelNum = 0;

Label::Label()
{
    sprintf(labelString, "L%d", labelNum++);
    if (labelNum >= 1000)
        assert(0);
}

void Label::print(FILE *fp) const
{
    fprintf(fp, "%s: \n", labelString);
}

IR *IR::currentIR = nullptr;

IR::IR()
{
    nextIR = nullptr;
    preIR = nullptr;
    currentIR = this;
}

IR::IR(const char *str, bool append)
{
    this->IRstring = str;
    if (append)
        appendIR(this);
}


IRList *IRList::makeIRList(IR &ir)
{
    return new IRList(ir);
}

IRList::IRList(IR &ir)
{
    irCount = 1;
    list.push_back(&ir);
}


IRList *IRList::mergeChain(IRList &irlist, IR *ir)
{
    irlist.list.push_back(ir);
    irlist.irCount++;
    return &irlist;
}


IRList *IRList::mergeChain(IRList &irlistFirst, const IRList &irlistSecond)
{
    int secondCount = irlistSecond.list.size();
    int i = 0;
    for (; i < secondCount; i++) {
        IR *ir = irlistSecond.list.at(i);
        irlistFirst.list.push_back(ir);
        irlistFirst.irCount++;
    }
    return &irlistFirst;
}


void IRList::backPatch(IRList &irlist, Label &label)
{
    int secondCount = irlist.list.size();
    int i = 0;
    const char *labelStr = label.toString();
    MAYBE_UNUSED int len = strlen(labelStr);
    for (; i < secondCount; i++) {
        IR &ir = *irlist.list.at(i);
        ir.dest = &label;
        //IRList::patch(ir, labelStr, len);
    }
}


void IRList::backPatch(IR &ir, Label &la)
{
    ir.dest = &la;
}


char *IRList::patch(IR &ir, const char *l, int length)
{
    const char *str = ir.getIRstring();
    int strLength = strlen(str);
    char *buffer = new char[strLength + length + 1];
    sprintf(buffer, str, l);
    ir.reSetString(buffer);
    return buffer;
}


void IR::print(IR *begin, FILE *dest)
{
    assert(begin->preIR == nullptr);
    IR *tp = begin;
    /*while (tp != NULL)
	{
	fprintf(dest, "%s", tp->IRstring);
	tp = tp->nextIR;
	}*/
    do {
        tp = tp->nextIR;
        if (tp == nullptr)
            break;
        fprintf(dest, tp->IRstring.c_str(), tp->dest);
    } while (1);
}


IR::IR(Label &l, bool append)
{
    const char *pl = l.toString();
    char *buf = new char[strlen(pl) + 10];
    sprintf(buf, "%s : \n", pl);
    this->IRstring = buf;
    if (!append)
        appendIR(this);
}

IR *IR::appendIR(IR *first, IR *second)
{
    second->nextIR = first->nextIR;
    if (first->nextIR != nullptr)
        first->nextIR->preIR = second;
    second->preIR = first;
    first->nextIR = second;
    //first = second;
    return first;
}


IR *IR::appendIRBlock(IR *place, IR *block)
{
    IR *placeNext = place->nextIR;
    IR *blockEnd = block;

    block->preIR->nextIR = nullptr;

    while (blockEnd->nextIR != nullptr)
        blockEnd = blockEnd->nextIR;
    place->nextIR = block;
    block->preIR = place;
    blockEnd->nextIR = placeNext;
    placeNext->preIR = blockEnd;
    block->preIR->nextIR = nullptr;
    reSetCurrent();
    return block->preIR;
}


void IR::reSetCurrent(void)
{
    IR *newCur = currentIR;
    while (newCur->nextIR != nullptr)
        newCur = newCur->nextIR;
    currentIR = newCur;
}


IR *IR::reSetCurrentFirst(IR *last)
{
    IR *newCur = last;
    while (newCur->preIR != nullptr)
        newCur = newCur->preIR;
    return newCur;
}
