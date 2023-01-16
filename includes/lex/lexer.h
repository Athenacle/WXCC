/* lexer header.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
*/

#ifndef LEX_HEADER
#define LEX_HEADER

#include "system.h"

#include "lex/constants.h"
#include "lex/token.h"
#include "lex/input.h"

#define BLOCK_SIZE     4096
#define LEX_BLOCK_SIZE (BLOCK_SIZE + 2) /* 4098 */
#undef LEX_BUFFER_SIZE
#define LEX_BUFFER_SIZE (LEX_BLOCK_SIZE * 2) /* 8196 */

#undef TMPBUFFER_SIZE
#define TMPBUFFER_SIZE 512

/* LEX ERROR AND WARNING NUMBER.
* all error and warning string is defined in the file lexWarningError.c
*/

#define LEX_ERROR_STRING_MEET_NEWLINE     1
#define LEX_ERROR_STRING_MEET_EOF         2
#define LEX_ERROR_ILL_SUFFIX              3
#define LEX_ERROR_ILL_CHAR_IN_HEX         4
#define LEX_ERROR_ILL_TOKEN               5
#define LEX_ERROR_ICON_TOO_BIG            6
#define LEX_ERROR_STRING_TOO_LONG         7
#define LEX_ERROR_CHAR_CON_TOO_LONG       8
#define LEX_ERROR_ILL_CHAR_ESC            9
#define LEX_ERROR_ILL_FLO_SUFFIX          10
#define LEX_ERROR_ILL_FLO_NUMBER          11
#define LEX_ERROR_FLOATING_OVERFLOW       12
#define LEX_ERROR_FLT_OVERFLOW            13
#define LEX_ERROR_ILL_CHAR_IN_OCT         14

#define LEX_WARNING_ILL_MACRO_TOKEN       2
#define LEX_WARNING_OCT_HEX_NOT_IMPLEMENT 3

#define EOT                               (static_cast<int>(0xdeadbeef))
/* EOT means End  Of Token.
* It indicates the end of input source file.
* Magic Number
*/

namespace lex
{
    class Lex
    {
        friend types::Token::Token(constants::KEYWORD);
        friend types::Token::Token(constants::TYPE, const char *);
        friend types::Token::Token(types::Number *);
        //friend types::Token::Token(Token &tok);

        using Token = types::Token;

    private:
        Lex(const Lex &) = delete;
        Lex &operator=(const Lex &) = delete;

        // static types::Token *currentToken;
        static types::Position *currentPos;

        LexInputSource *source;

        types::Token *newTok;

        static char tmpBuffer[TMPBUFFER_SIZE];

        static int lexWarningCount;
        static int lexErrorCount;

        ///// lex functions.
        char getNextChar(int);
        void skipBlank(void);
        void getBufferUntilSp(void);
        void resynch(void);
        int getAChar(char *, int *);
        types::Number *tryParseFloatNumber(char *, types::Number **);
        types::Number *tryParseDecNumber(char *);
        types::Number *tryParseHexNumber(char *);
        types::Number *tryParseOctNumber(char *);

        Token newNumber(char *, int);
        Token number(char);
        Token newIdentifier(const char *, int);
        Token identifier(char *);
        Token letter(char);
        Token parseLetter(char);
        Token operators(char);
        Token parseOperator(char);
        Token parseString(void);
        Token parseCharConstant(void);
        /////  lex functions end.

        void lexerWarning(int warningType, const char *message, ...) const;
        void lexerError(int warningType, const char *message, ...) const;

    public:
#ifdef LEX_DEBUG
        extern FILE *lexDebugOut;
        void printToken(Token tok);
        void lexDebugInit(void);
#endif
        // types::Token *getToken(void)
        // {
        //     return currentToken;
        // }
        types::Position *getPos(void)
        {
            return currentPos;
        }

        // types::Token *getLastToken(void)
        // {
        //     return new types::Token(*currentToken);
        // }

        Lex(LexInputSource *source);

        Token getNextToken(void);
    };
}  // namespace lex

#endif