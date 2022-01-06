/* lexer debuggger
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 23, 2013
*
*/
#include "lex.h"
#ifdef LEX_DEBUG

static char *lexDebugInfo_OP[] =
{
	"P_NONE",
	"P_NOT",
	"P_MOD",
	"P_XOR",
	"P_BITAND",
	"P_MULT",
	"P_PLUS",
	"P_MINUS",
	"P_ASSIGN",
	"P_BITNOT",
	"P_BITOR",
	"P_DIV",
	"P_QUESTION",
	"P_ALPLUS",
	"P_ALMINUS",
	"P_ALMULT",
	"P_ALDIV",
	"P_ALMOD",
	"P_ALLEFTSHIFT",
	"P_ALRIGHTSHIFT",
	"P_ALBITAND",
	"P_ALBITXOR",
	"P_ALBITOR",
	"P_POINTER",
	"P_INC",
	"P_DEC",
	"P_LEFTSHIFT",
	"P_RIGHTSHIFT",
	"P_LT",
	"P_GT",
	"P_LE",
	"P_GE",
	"P_EQ",
	"P_NE",
	"P_LOGAND",
	"P_LOGOR",
	"P_LEFTBRACK",
	"P_RIGHTBRACK",
	"P_LEFTSQBRAC",
	"P_RIGHTSQBRAC",
	"P_LEFTBRACE",
	"P_RIGHTBRACE",
	"P_COMMA",
	"P_COLON",
	"P_SEMICOLON",
	"P_EPPIPSIS",
	"P_PERIOD"
};

static char *lexDebugInfo_KEY[] =
{
	"KEY_NONE",
	"KEY_AUTO",
	"KEY_BREAK",
	"KEY_CASE",
	"KEY_CHAR",
	"KEY_CONST",
	"KEY_CONTINUE",
	"KEY_DEFAULT",
	"KEY_DO",
	"KEY_DOUBLE",
	"KEY_ELSE",
	"KEY_ENUM",
	"KEY_EXTERN",
	"KEY_FLOAT",
	"KEY_FOR",
	"KEY_GOTO",
	"KEY_IF",
	"KEY_INT",
	"KEY_LONG",
	"KEY_RETURN",
	"KEY_REGISTER",
	"KEY_SHORT",
	"KEY_SIGNED",
	"KEY_SIZEOF",
	"KEY_STATIC",
	"KEY_STRUCT",
	"KEY_SWITCH",
	"KEY_TYPEDEF",
	"KEY_UNION",
	"KEY_UNSIGNED",
	"KEY_VOID",
	"KEY_VOLATILE",
	"KEY_WHILE"
};

static char *lexDebugInfo_TYPE[] =
{
	"T_ID",
	"T_KEY",
	"T_STRING",
	"T_ICON",
	"T_FCON",
	"T_CHARCON"
};

extern FILE *lexDebugOut = NULL;
//#define DEBUG_STDOUT

void lexDebugInit(void)
{
#ifdef DEBUG_STDOUT
	lexDebugOut = stdout;
#else
	lexDebugOut = fopen("lexDebugInfo.txt","w");
	assert(lexDebugOut);
#endif
}

void printToken(Token tok)
{
	switch (tok.token_type)
	{
	case T_ID:fprintf(lexDebugOut, "Token type:T_ID ");break;
	case T_KEY:fprintf(lexDebugOut, "Token type:T_KEY ");break;
	case T_STRING:fprintf(lexDebugOut,"Token type:T_STRING ");break;
	case T_INT_CON:fprintf(lexDebugOut,"Token type:T_INT_CON ");break;
	case T_FLOAT_CON:fprintf(lexDebugOut,"Token type:T_FLOAT ");break;
	case T_CHAR_CON:fprintf(lexDebugOut,"Token type:T_CHAR_CON ");break;
	case T_NONE:fprintf(lexDebugOut,"Token type:T_NONE. ");break;
	case T_OPERATOR:fprintf(lexDebugOut,"Token type:T_OP. ");break;
	}
	fprintf(lexDebugOut, "at %s, line %d, place %d", tok.token_pos->filename,
		tok.token_pos->line + 1,tok.token_pos->place + 1);
	switch(tok.token_type)
	{
	case T_ID :
		fprintf(lexDebugOut,"\t\tidentifier name: %s\n",
			tok.token_value.id_name);
		break;
	case T_KEY:
		fprintf(lexDebugOut,"\t\tkeyword    name: %s\n",
			lexDebugInfo_KEY[tok.token_value.keyword
			- KEY_NONE]);
		break;
	case T_STRING:
		fprintf(lexDebugOut,"\t\tstring         : %s\n",
			tok.token_value.string);
		break;
	case T_INT_CON:case T_CHAR_CON:
		{
			char *numType = NULL;
			switch(tok.token_value.numVal.numberType)
			{
			case NT_UI:
				numType = "unsigned int";
				break;
			case NT_UL:
				numType = "unsigned long";
				break;
			case NT_SI:
				numType = "signed int";
				break;
			case NT_SL:
				numType = "signed long";
				break;
			case NT_CH:
				numType = "char";
				break;
			case NT_NONE:
				numType = "error";
				break;
			default: assert(0);
			}
			fprintf(lexDebugOut,"\t\tinteger   value: %d",
				tok.token_value.numVal.val.i_value);
			fprintf(lexDebugOut," Number Type: %s.\n",numType);
		}
		break;
	case T_FLOAT_CON:
		{
			char *numType = NULL;
			switch(tok.token_value.numVal.numberType)
			{
			case NT_FL:
				numType = "float";
				break;
			case NT_DB:
				numType  = "double";
				break;
			case NT_NONE:
				numType = "error";
				break;
			default: assert(0);
			}
			fprintf(lexDebugOut,"\t\tfloating  value: %lf",
				tok.token_value.numVal.val.d_value);
			fprintf(lexDebugOut," Number Type: %s.\n",numType);
		}
		break;
	case T_OPERATOR:
		fprintf(lexDebugOut,"\t\toperator  value: %s\n",
			lexDebugInfo_OP[tok.token_value.op - OP_NONE]);
		break;
	}
	//fflush(lexDebugOut);
}

#endif