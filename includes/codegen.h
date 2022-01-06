/* internal code generator
 * Wang Xiaochun
 * zjjhwxc@gmail.com
 * March 30, 2013
 */

#ifndef CODEGEN_H
#define CODEGEN_H

enum IRs
{
	IR_ADD,
	IR_MULT,
	IR_LOAD,
	IR_IF,
	IR_GOTO,
	IR_LEA,
	IR_CMP,
	IR_MOV,
	IR_CALL,
	IR_RET,
	IR_PUSH,
	IR_POP,
	IR_DIV,
	IR_MINUS,
};

struct IR_Label
{
	int dest_label;
};

#define IR_LENGTH 20

struct IR_Node
{
	char res[IR_LENGTH];
	struct IR_Node *next;
	struct IR_Node *next_chain;
	struct IR_Label *gt;
};

/* if A < B goto 102 {"A < B", }
 * goto 107
 * if C < D goto 104
 * goto 100
 * T := Y + Z
 * X := T
 * goto 100
 */
#endif