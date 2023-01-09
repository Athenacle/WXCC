/* lexer header.
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 20, 2013
*
*/

#ifndef LEX_HEADER
#define LEX_HEADER

#include "system.h"

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
    namespace constants
    {
        enum TYPE {
            T_NONE = 0x00010000,
            T_KEY,
            T_STRING,
            T_INT_CON,
            T_FLOAT_CON,
            T_ID,
            T_OPERATOR,
            T_CHAR_CON
        };

        /*
	* enum KEYOWRD
	* 0x0002
	*      [0]     1: STORAGE-CLASS-SPECIFIER: auto register static extern typedef
	*              0: others
	*
	*      [0]     1: TYPE-SPECIFIER
	*              0: others
	*
	*      [0]     1: TYPE-QUALIFIER: const volatile
	*              0: other
	*
	*      [0]
	*
	*/

        enum KEYWORD {
            KEY_NONE = 0x00020000,

            /* STORAGE-CLASS-SPECIFIER */
            KEY_AUTO = 0x00021001,
            KEY_EXTERN = 0x00021002,
            KEY_REGISTER = 0x00021003,
            KEY_STATIC = 0x00021004,
            KEY_TYPEDEF = 0x00021005,

            /* TYPE-SPECIFIER */
            KEY_CHAR = 0x00020101,
            KEY_FLOAT = 0x00020102,
            KEY_INT = 0x00020103,
            KEY_LONG = 0x00020104,
            KEY_UNION = 0x00020105,
            KEY_ENUM = 0x00020106,
            KEY_SIGNED = 0x00020107,
            KEY_SHORT = 0x00020108,
            KEY_UNSIGNED = 0x00020109,
            KEY_STRUCT = 0x0002010A,
            KEY_DOUBLE = 0x0002010B,
            KEY_TVOID = 0x0002010C,

            /* TYPE-QUALIFIER */
            KEY_VOLATILE = 0x00020011,
            KEY_CONST = 0x00020012,

            /* KEYWORD_OTHERS*/
            KEY_CONTINUE = 0x00020001,
            KEY_DEFAULT = 0x00020002,
            KEY_DO = 0x00020003,
            KEY_BREAK = 0x00020004,
            KEY_CASE = 0x00020005,
            KEY_ELSE = 0x00020006,
            KEY_FOR = 0x00020007,
            KEY_GOTO = 0x00020008,
            KEY_IF = 0x00020009,
            KEY_RETURN = 0x0002000A,
            KEY_SIZEOF = 0x0002000B,
            KEY_SWITCH = 0x0002000C,
            KEY_KVOID = 0x0002000D,
            KEY_WHILE = 0x0002000E,
        };

        const unsigned long TYPE_STORAGE_CLASS_SPECIFIER_MASK = 0x00021000;
        const unsigned long TYPE_SPECIFIER_MASK = 0x00020100;
        const unsigned long TYPE_QUALIFIER_MASK = 0x00020010;
        const unsigned long TYPE_SSQ = (0x00021000 | 0x00020100 | 0x00020010);
        const unsigned long KEYWORD_MASK = (0x00020000);

        enum OP {
            OP_NONE = 0x00040000,
            OP_LOGNOT,   /* ! */
            OP_MOD,      /* % */
            OP_BITXOR,   /* ^ */
            OP_BITAND,   /* & */
            OP_MULT,     /* * */
            OP_PLUS,     /* + */
            OP_MINUS,    /* - */
            OP_BITNOT,   /* ~ */
            OP_BITOR,    /* | */
            OP_DIV,      /* / */
            OP_QUESTION, /* ? */

            OP_POINTER,     /* -> */
            OP_INC,         /* ++ */
            OP_DEC,         /* -- */
            OP_LEFTSHIFT,   /* << */
            OP_RIGHTSHIFT,  /* >> */
            OP_LT,          /* < */
            OP_GT,          /* > */
            OP_LE,          /* <= */
            OP_GE,          /* >= */
            OP_EQ,          /* == */
            OP_NE,          /* != */
            OP_LOGAND,      /* && */
            OP_LOGOR,       /* || */
            OP_LEFTBRACK,   /* ( */
            OP_RIGHTBRACK,  /* ) */
            OP_LEFTSQBRAC,  /* [ */
            OP_RIGHTSQBRAC, /* ] */
            OP_LEFTBRACE,   /* { */
            OP_RIGHTBRACE,  /* } */
            OP_COMMA,       /* , */
            OP_COLON,       /* : */
            OP_SEMICOLON,   /* ; */
            OP_EPPIPSIS,    /* ... */
            OP_PERIOD,      /* . */


            OP_ASSIGN = 0x00041000, /* = */
            OP_ALPLUS,              /* += */
            OP_ALMINUS,             /* -= */
            OP_ALMULT,              /* *= */
            OP_ALDIV,               /* /= */
            OP_ALMOD,               /* %= */
            OP_ALLEFTSHIFT,         /* <<= */
            OP_ALRIGHTSHIFT,        /* >>= */
            OP_ALBITAND,            /* &= */
            OP_ALBITXOR,            /* ^= */
            OP_ALBITOR,             /* |= */

            OP_SIZEOF,

        };
        const unsigned long OP_MASK = 0x00040000;
        const unsigned long OP_ASGN_MASK = 0x00041000;

        /*
	* NUMBER_TYPE
	* 0x0011  [	0		0		0		0]
	*		FL/INT		U/S		LONG		INT/CHAR
	*/

        const uint32_t NT_NONE = 0X0011ffff;
        const uint32_t NT_SI = 0x00110001; /* SIGNED	INT  */
        const uint32_t NT_UI = 0x00110101; /* UNSIGNED	INT  */
        const uint32_t NT_SL = 0x00110011; /* SIGNED	LONG */
        const uint32_t NT_UL = 0x00110111; /* UNSIGNED	LONG */
        const uint32_t NT_CH = 0x00110103; /* UNSIGNED	CHAR */
        const uint32_t NT_FL = 0x00111000; /* FLOAT	     */
        const uint32_t NT_DB = 0x00111010; /* DOUBLE	     */
#define NT_LONG_MASK     (0X00110010)
#define NT_UNSIGNED_MASK (0x00110100)
#define NT_LD            (NT_DB) /* LONG DOUBLE */
        class NumberType
        {
            uint32_t _type;

        public:
            NumberType() : _type(NT_NONE) {}

            NumberType(uint32_t type) : _type(type) {}

            bool operator==(const uint32_t other) const
            {
                return _type == other;
            }

            uint32_t type() const
            {
                return _type;
            }
        };

        NumberType &operator|=(NumberType &, unsigned long);

    };  // namespace constants

    namespace types
    {
        class Number
        {
        public:
            constants::TYPE type;             /* T_INT_CON T_FLOAT_CON T_CHAR_CON */
            constants::NumberType numberType; /* enum NumberType */
            union {
                double d_value;
                int i_value;
            } val;
            Number(const Number &num) : type(num.type), numberType(num.numberType)
            {
                val.d_value = num.val.d_value;
            }
            Number() {}
        };

        struct Position {
            char const *filename;
            int line;
            int place;
            Position(const Position &pos) : filename(pos.filename), line(pos.line), place(pos.place)
            {
            }
            Position(const char *file, int l = 1, int p = 0) : filename(file), line(l), place(p) {}
        };

        class Token
        {
        public:
            constants::TYPE token_type;
            Position *token_pos;
            union token_value {
                Number *numVal;
                const char *id_name;
                const char *string;
                constants::KEYWORD keyword;
                constants::OP op;
            } token_value;

            bool inline operator==(constants::TYPE ty)
            {
                return token_type == ty;
            }


            bool inline operator==(constants::OP _op)
            {
                return token_type == constants::T_OPERATOR && token_value.op == _op;
            }

            bool inline operator==(constants::KEYWORD _key)
            {
                return token_type == constants::T_KEY && token_value.keyword == _key;
            }

            bool inline operator!=(constants::KEYWORD _key)
            {
                return !this->operator==(_key);
            }

            bool inline operator!=(constants::OP _op)
            {
                return !this->operator==(_op);
            }

            explicit Token(constants::KEYWORD);
            explicit Token(constants::TYPE, const char *);
            explicit Token(Number *);
            explicit Token(Token &tok);
            Token()
            {
                token_type = constants::T_NONE;
                token_pos = nullptr;
                token_value.id_name = nullptr;
            }
        };
    };  //namespace types

    namespace tools
    {
        using types::Token;
        using namespace constants;

        unsigned long strHash(const char *);

        bool inline is(const Token &_tok, TYPE _ty)
        {
            return _tok.token_type == _ty;
        }

        bool inline isNumCon(const Token &_tok)
        {
            return _tok.token_type == T_INT_CON || _tok.token_type == T_FLOAT_CON
                   || _tok.token_type == T_CHAR_CON;
        }

        bool inline matchKEY(const Token &_tok, KEYWORD k)
        {
            return is(_tok, T_KEY) && _tok.token_value.keyword == k;
        }

        bool inline isOP(const Token &_tok)
        {
            return is(_tok, T_OPERATOR);
        }

        bool inline isBaseType(const Token &_tok)
        {
            return is(_tok, T_KEY)
                   && ((_tok.token_value.keyword & TYPE_SPECIFIER_MASK) == TYPE_SPECIFIER_MASK
                       || matchKEY(_tok, KEY_TVOID) || matchKEY(_tok, KEY_KVOID));
        }

        bool inline matchOP(const Token &_tok, OP _op)
        {
            return isOP(_tok) && _tok.token_value.op == _op;
        }

        bool inline matchQualifier(const Token &_tok)
        {
            return _tok.token_type == T_KEY
                   && (_tok.token_value.keyword == KEY_CONST
                       || _tok.token_value.keyword == KEY_VOLATILE);
        }

        bool inline isID(const Token &_tok)
        {
            return is(_tok, T_ID);
        }

        bool inline isIter(const Token &_tok)
        {
            return is(_tok, T_KEY)
                   && (_tok.token_value.keyword == KEY_FOR || _tok.token_value.keyword == KEY_WHILE
                       || _tok.token_value.keyword == KEY_DO);
        }

        bool inline isJump(const Token &_tok)
        {
            return is(_tok, T_KEY)
                   && (_tok.token_value.keyword == KEY_RETURN
                       || _tok.token_value.keyword == KEY_BREAK
                       || _tok.token_value.keyword == KEY_CONTINUE
                       || _tok.token_value.keyword == KEY_GOTO);
        }

        bool inline isSelect(const Token &_tok)
        {
            return is(_tok, T_KEY)
                   && (_tok.token_value.keyword == KEY_SWITCH
                       || _tok.token_value.keyword == KEY_IF);
        }

        bool inline isLabel(const Token &_tok)
        {
            return is(_tok, T_KEY)
                   && (_tok.token_value.keyword == KEY_CASE
                       || _tok.token_value.keyword == KEY_DEFAULT);
        }

        bool inline isASSIGN(const Token &_tok)
        {
            return isOP(_tok) && (_tok.token_value.op & OP_ASGN_MASK) == OP_ASGN_MASK;
        }

    };  //namespace tools

    class Lex
    {
        friend types::Token::Token(constants::KEYWORD);
        friend types::Token::Token(constants::TYPE, const char *);
        friend types::Token::Token(types::Number *);
        friend types::Token::Token(Token &tok);

    private:
        Lex(const Lex &);
        Lex &operator=(const Lex &);

        static types::Token *currentToken;
        static types::Position *currentPos;

        types::Token *newTok;

        FILE *sourceFile;
        const char *start;
        const char *pointer;

        static char buffer[LEX_BUFFER_SIZE + 4];
        static char tmpBuffer[TMPBUFFER_SIZE];
        int initLex(const char *_filename);
        int fillBuffer(void);

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
        int newNumber(char *, int);
        int number(char);
        int newIdentifier(const char *, int);
        int identifier(char *);
        constants::KEYWORD letter(char);
        int parseLetter(char);
        int operators(char);
        int parseOperator(char);
        int parseString(void);
        int parseCharConstant(void);
        /////  lex functions end.

        void lexerWarning(int warningType, const char *message, ...) const;
        void lexerError(int warningType, const char *message, ...) const;

    public:
#ifdef LEX_DEBUG
        extern FILE *lexDebugOut;
        void printToken(Token tok);
        void lexDebugInit(void);
#endif
        types::Token *getToken(void)
        {
            return currentToken;
        }

        types::Position *getPos(void)
        {
            return currentPos;
        }

        types::Token *getLastToken(void)
        {
            return new types::Token(*currentToken);
        }

        Lex(const char *filename);

        int getNextToken(void);
    };
}  // namespace lex

#endif