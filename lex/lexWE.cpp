/* lexer warnings and errors message.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 21, 2013
*
*/

#include "lex.h"

namespace{
	const char *lexWarnings[] =
	{
		"NULL",
		"LW01: penitential floating point number overflow.\n",
		"LW02: illegal token \"%s\" after # has been found. This line will be ignored.\n",
		/* LEX_WARNING_ILL_MACRO_TOKEN */

		"LW03: the octal and hex number has not been implemented in this version of WXCC.\n"
		"\t\t\t\t\t\tThe number %s will be ignored.\n"
	};

	const char *const lexErrors[] =
	{
		"NULL",
		"LE01: a newline character exist in the string constant or char constant.\n",
		/* LEX_ERROR_STRING_MEET_NEWLINE*/

		"LE02: string constant or char constant do not terminal before see a End-Of-File.\n",
		/* LEX_ERROR_STRING_MEET_EOF	*/

		"LE03: illegal suffix %s after a number.\n",
		/* LEX_ERROR_ILL_SUFFIX		*/

		"LE04: illegal character \'%c\' exist in a HEX number constant.\n",
		/* LEX_ERROR_ILL_CHAR_IN_HEX	*/

		"LE05: illegal token: \"%c\".\n",
		/* LEX_ERROR_ILL_TOKEN */

		"LE06: integer number constant \"%s\" too big.\n",
		/* LEX_ERROR_ICON_TOO_BIG	*/

		"LE07: string too long, cut the front.\n",
		/* LEX_ERROR_STRING_TOO_LONG	*/

		"LE08: char constant too long. \n",
		/* LEX_ERROR_CHAR_CON_TOO_LONG	*/

		"LE09: invalid escape character sequence.\n",
		/* LEX_ERROR_ILL_CHAR_ESC */

		"LE10: invalid floating number suffix: \"%s\".\n",
		/* LEX_ERROR_ILL_FLO_SUFFIX */

		"LE11: invalid floating number.\n",
		/* LEX_ERROR_ILL_FLO_NUMBER */

		"LE12: floating number \"%s\" overflowed.\n",
		/* LEX_ERROR_FLOATING_OVERFLOW */

		"LE13: single precision floating number \"%s\" overflowed.\n",
		/* LEX_ERROR_FLT_OVERFLOW */

		"LE14: illegal character \'%c\' exist in an OCT number constant.\n"
		/*LEX_ERROR_ILL_CHAR_IN_OCT	*/
	};
};

#ifdef LEX_DEBUG
//#define LEX_DEBUG_ASM_BP
#endif

int Lex::lexWarningCount = 0;
int Lex::lexErrorCount = 0;

void Lex::lexerWarning(int warningType, const char *message, ...) const
{
	lexWarningCount++;
#if defined LEX_DEBUG_ASM_BP && defined WIN32
	__asm{
		int 3
	}
#endif
	fprintf(wxccErr, "Warn: file: %s, line: %d.>> ", currentPos->filename,
		currentPos->line + 1);
	switch (warningType)
	{
	case LEX_WARNING_ILL_MACRO_TOKEN :
		fprintf(wxccErr, lexWarnings[LEX_WARNING_ILL_MACRO_TOKEN], message);
		break;
	case 3:
		fprintf(wxccErr, lexWarnings[3], message);
		break;
	default:
		assert(static_cast<unsigned int>(0));
	}
}

void Lex::lexerError(int errorType, const char *message, ...) const
{
	lexErrorCount++;
#if defined LEX_DEBUG_ASM_BP && defined WIN32
	__asm{
		int 3
	}
#endif
	fprintf(wxccErr, "Error.file: %s, line: %d.>> ", currentPos->filename,
		currentPos->line + 1);
	switch (errorType)
	{
	case LEX_ERROR_STRING_MEET_NEWLINE:		/* 1 */
		fprintf(wxccErr, "%s", lexErrors[LEX_ERROR_STRING_MEET_NEWLINE]);
		break;
	case LEX_ERROR_STRING_MEET_EOF:			/* 2 */
		fprintf(wxccErr, "%s", lexErrors[LEX_ERROR_STRING_MEET_EOF]);
		break;
	case LEX_ERROR_ILL_SUFFIX :			/* 3 */
		fprintf(wxccErr, lexErrors[LEX_ERROR_ILL_SUFFIX], message);
		break;
	case LEX_ERROR_ICON_TOO_BIG:			/* 6 */
		fprintf(wxccErr, lexErrors[LEX_ERROR_ICON_TOO_BIG], message);
		break;
	case LEX_ERROR_ILL_TOKEN:			/* 5 */
		fprintf(wxccErr, lexErrors[LEX_ERROR_ILL_TOKEN], *message);
		break;
	case LEX_ERROR_STRING_TOO_LONG:
		fprintf(wxccErr, "%s", lexErrors[LEX_ERROR_STRING_TOO_LONG]);
		break;
	case LEX_ERROR_ILL_CHAR_IN_HEX:
		fprintf(wxccErr, lexErrors[LEX_ERROR_ILL_CHAR_IN_HEX], *message);
		break;
	case LEX_ERROR_ILL_CHAR_IN_OCT:
		fprintf(wxccErr, lexErrors[LEX_ERROR_ILL_CHAR_IN_OCT], *message);
		break;
	case LEX_ERROR_CHAR_CON_TOO_LONG:
		fprintf(wxccErr, "%s", lexErrors[LEX_ERROR_CHAR_CON_TOO_LONG]);
		break;
	case LEX_ERROR_ILL_FLO_SUFFIX:
		fprintf(wxccErr, lexErrors[LEX_ERROR_ILL_FLO_SUFFIX], message);
		break;
	case LEX_ERROR_ILL_FLO_NUMBER:
		fprintf(wxccErr, "%s", lexErrors[LEX_ERROR_ILL_FLO_SUFFIX]);
		break;
	case LEX_ERROR_FLOATING_OVERFLOW:
		fprintf(wxccErr, lexErrors[LEX_ERROR_FLOATING_OVERFLOW], message);
		break;
	default:
		assert(static_cast<unsigned int>(0));
	}
}