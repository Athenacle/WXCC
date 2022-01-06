/*
* syntax parser warnings and errors.
* Wang Xiaochun
* zjjhwxc(at)gmail.com
* March 5, 2013
*
*/

#include "system.h"
#include "lex.h"
#include "parser.h"

int Parser::parserWarningCount = 0;
int Parser::parserErrorCount = 0;

namespace{
	char *parserWarnings[] =
	{
		"NULL",

		"PW01: empty source file.\n"
		/* PAR_WARNING_EMPTY_TRANSLATION_UNIT */

		"PW02: empty declaration.\n"
		/* PAR_WARNING_EMPTY_DECLARATION */
	};
	char *parserErrors[] =
	{
		"NULL",

		"PE01: Line: %d : need \"type\".\n",
		// PAR_ERR_NEED_TYPE

		"PE02: Line: %d : need \'%c\' but meet a \'%c\'.\n",
		// PAR_ERR_NEED_C_CC

		"PE02: Line: %d : need \'%c\' but meet a \'%s\'.\n",
		// PAR_ERR_NEED_C_CS

		"PE02: Line: %d : need \'%s\' but meet a \'%s\'.\n",
		// PAR_ERR_NEED_SS

		"PE03. Line: %d : need a variable.\n",
		// PAR_ERR_NEED_VAR

		"PE04. Line : %d : unmatched keyword \'%s\'.\n"
		// PAR_ERR_UNMATCHED_KEY
	};
};

#define PARSER_DEBUG_ASM_BP

void Parser::parserWarning(int warningType, ...) const
{
	parserWarningCount++;
#if defined PARSER_DEBUG_ASM_BP && defined WIN32
	__asm{
		int 3
	}
#endif

	switch (warningType)
	{
	case PARER_WE_USE_STRING:
		goto use_string_warn;
	case PAR_WARNING_EMPTY_TRANSLATION_UNIT:
	case PAR_WARNING_EMPTY_DECLARATION:
	
		fprintf(wxccErr, parserWarnings[warningType]);
		break;
	default:assert(0);
	}
	return;
use_string_warn:
	va_list ap;
	va_start(ap, warningType);
	const char *str = va_arg(ap, const char *);
	int i = va_arg(ap, int);
	fprintf(wxccErr, str, i);
	va_end(ap);
	return;
}

void Parser::parserError(int errorType,...) const
{
	
	parserErrorCount++;
#if defined PARSER_DEBUG_ASM_BP && defined WIN32
	__asm{
		int 3
	}
#endif
	switch (errorType)
	{
	case PARER_WE_USE_STRING:
		goto use_string_err;
	case PAR_ERR_NEED_TYPE:
	case PAR_ERR_NEED_VAR_D:
	
		{
			va_list ap;
			va_start(ap, errorType);
			fprintf(wxccErr, parserErrors[errorType], va_arg(ap,int));
			va_end(ap);
			break;
		}
	case PAR_ERR_NEED_C_CC:
		{
			va_list ap;
			va_start(ap, errorType);
			int c1 = va_arg(ap, int);
			char c2 = va_arg(ap, char);
			char c3 = va_arg(ap, char);
			fprintf(wxccErr, parserErrors[errorType], 
				c1, c2, c3);
			va_end(ap);
			break;
		}
	case PAR_ERR_NEED_C_CS:
		{
			va_list ap;
			
			va_start(ap, errorType);
			char c = va_arg(ap, char);
			const char *str = va_arg(ap, const char*);
			int i =  va_arg(ap, int);
			fprintf(wxccErr, parserErrors[errorType],c ,str,i);
			va_end(ap);
			break;
		}
	case PAR_ERR_INT_STR:
		{
			va_list ap;
			va_start(ap, errorType);
			int i =  va_arg(ap, int);
			const char *str = va_arg(ap, const char*);
			fprintf(wxccErr, parserErrors[errorType], i, str);
			va_end(ap);
			break;
		}
	case PAR_ERR_STR_INT:
		{
			va_list ap;
			va_start(ap, errorType);
			const char *str = va_arg(ap, const char*);
			int i =  va_arg(ap, int);
			fprintf(wxccErr, str, i);
			va_end(ap);
			break;
		}
	case PAR_ERR_UNMATCHED_KEY:
		{
			va_list ap;
			va_start(ap, errorType);
			fprintf(wxccErr, parserErrors[PAR_ERR_UNMATCHED_KEY],110,"else");
			va_end(ap);
			break;
		}
	default:assert(0);
	}
	return;
use_string_err:
	va_list ap;
	va_start(ap, errorType);
	const char *str = va_arg(ap, const char *);
	int i = va_arg(ap, int);
	fprintf(wxccErr, str, i);
	va_end(ap);
	return;
}