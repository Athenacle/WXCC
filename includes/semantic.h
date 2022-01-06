/*
* semantic analyzer header.
* Wang Xiaochun
* zjjhwxc(at)gmail.com
* March 9, 2013
*
*/

#include "system.h"


#ifndef SEMANTIC_H
#define SEMANTIC_H


extern int semErrorCount;

void semError(int errType, ...);
void semWarning(int errType,...);
#define SEM_INVAILD_USE		1

#define USE_MY_STR_SSD		0xabcdef

class TemportaryVariable
{
private:
	int tempID;
	static int currentID;
	char idString[4];
public:
	TemportaryVariable();
	virtual ~TemportaryVariable();
	virtual const char* toString();

	void print(FILE *fp = wxccErr) const
	{
		fprintf(fp, "%s ", idString);
	}
};

class Label
{
	static int labelNum;
	char labelString[8];
public:
	
	Label();

	static char* nextLabel()
	{
		labelNum--;
		return strdup(Label().toString());
	}

	void print(FILE *fp = wxccErr) const;
	const char* toString(void) const
	{
		return labelString;
	}
};

class IR
{
public:

	IR *nextIR;
	IR *preIR;
	Label *dest;
	static IR *currentIR;
	std::string IRstring;
	
	
	
	const IR* operator += (IR* ir)
	{
		return ir;
	}

	const char* getIRstring(void) const
	{
		return IRstring.c_str();
	}

	void reSetString(char* str)
	{
		IRstring = str;
	}

	static void print(IR*, FILE *dest = wxccErr);
	
	IR(const char*, bool = true);
	IR(Label&, bool = true);
	IR();
	~IR(){}
	
	static IR* appendIR(IR *, IR *);
	static IR* appendIR(IR *ir)
	{
		currentIR->nextIR = ir;
		ir->preIR = currentIR;
		ir->nextIR = NULL;
		currentIR = ir;
		return currentIR;
	}
	static IR* appendIRBlock(IR*, IR*);
	void print(const char*  format, ...);
	static void reSetCurrent(void);
	static void reSetCurrent(IR*);
	static IR* reSetCurrentFirst(IR*);
};
using namespace std;	//for vector

class IRList
{
public:
	static IRList* mergeChain(IRList&, const IRList&);
	static IRList* mergeChain(IRList&, IR*);
	static void backPatch(IRList&, Label&);
	static void backPatch(IR&, Label&);
	static IRList* makeIRList(IR&);
	int getIRcount(void) const
	{
		return irCount;
	}
private:
	int irCount;
	IRList(IR&);
	vector<IR*> list;
	static char *patch(IR&, const char* l, int length);
};


class Block
{
	friend void backPatch(Label* &first, Label* &second);
	friend Label *mergeChain(Label* &first,Label* &second);
public:
	IR *codeBegin;
	Label *codeEnd;
	IRList *nextList;
	IRList *continueList;
	IRList *breakList;

	Block()
		:codeBegin(NULL),codeEnd(NULL),nextList(NULL),
			continueList(NULL),breakList(NULL)
	{}
};


#endif
