#ifndef LEX_CONSTANTS_H_
#define LEX_CONSTANTS_H_

#include "system.h"

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
            KEY_NONE     = 0x00020000,

            /* STORAGE-CLASS-SPECIFIER */
            KEY_AUTO     = 0x00021001,
            KEY_EXTERN   = 0x00021002,
            KEY_REGISTER = 0x00021003,
            KEY_STATIC   = 0x00021004,
            KEY_TYPEDEF  = 0x00021005,

            /* TYPE-SPECIFIER */
            KEY_CHAR     = 0x00020101,
            KEY_FLOAT    = 0x00020102,
            KEY_INT      = 0x00020103,
            KEY_LONG     = 0x00020104,
            KEY_UNION    = 0x00020105,
            KEY_ENUM     = 0x00020106,
            KEY_SIGNED   = 0x00020107,
            KEY_SHORT    = 0x00020108,
            KEY_UNSIGNED = 0x00020109,
            KEY_STRUCT   = 0x0002010A,
            KEY_DOUBLE   = 0x0002010B,
            KEY_TVOID    = 0x0002010C,

            /* TYPE-QUALIFIER */
            KEY_VOLATILE = 0x00020011,
            KEY_CONST    = 0x00020012,

            /* KEYWORD_OTHERS*/
            KEY_CONTINUE = 0x00020001,
            KEY_DEFAULT  = 0x00020002,
            KEY_DO       = 0x00020003,
            KEY_BREAK    = 0x00020004,
            KEY_CASE     = 0x00020005,
            KEY_ELSE     = 0x00020006,
            KEY_FOR      = 0x00020007,
            KEY_GOTO     = 0x00020008,
            KEY_IF       = 0x00020009,
            KEY_RETURN   = 0x0002000A,
            KEY_SIZEOF   = 0x0002000B,
            KEY_SWITCH   = 0x0002000C,
            KEY_KVOID    = 0x0002000D,
            KEY_WHILE    = 0x0002000E,
        };

        const unsigned long TYPE_STORAGE_CLASS_SPECIFIER_MASK = 0x00021000;
        const unsigned long TYPE_SPECIFIER_MASK               = 0x00020100;
        const unsigned long TYPE_QUALIFIER_MASK               = 0x00020010;
        const unsigned long TYPE_SSQ     = (0x00021000 | 0x00020100 | 0x00020010);
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
        const unsigned long OP_MASK      = 0x00040000;
        const unsigned long OP_ASGN_MASK = 0x00041000;

        /*
	    * NUMBER_TYPE
	    * 0x0011  [	0		0		0		0]
	    *         FL/INT    U/S	    LONG    INT/CHAR
	    */

        const uint32_t NT_NONE           = 0X0011ffff;
        const uint32_t NT_SI             = 0x00110001; /* SIGNED	INT  */
        const uint32_t NT_UI             = 0x00110101; /* UNSIGNED	INT  */
        const uint32_t NT_SL             = 0x00110011; /* SIGNED	LONG */
        const uint32_t NT_UL             = 0x00110111; /* UNSIGNED	LONG */
        const uint32_t NT_CH             = 0x00110103; /* UNSIGNED	CHAR */
        const uint32_t NT_FL             = 0x00111000; /* FLOAT	     */
        const uint32_t NT_DB             = 0x00111010; /* DOUBLE	     */

#define NT_LONG_LONG_MASK   (0x0011'0020)
#define NT_LONG_MASK        (0x0011'0010)
#define NT_SHORT_MASK       (0x0011'00f0)

#define NT_SIGNED_MASK      (0x0011'0200)
#define NT_UNSIGNED_MASK    (0x0011'0100)

#define NT_FLOAT_MASK       (0x0011'1000)
#define NT_DOUBLE_MASK      (0x0011'2000)
#define NT_LONG_DOUBLE_MASK (0x0011'1010)
#define NT_CHAR_MASK        (0x0011'0001)
#define NT_LD               (NT_LONG_DOUBLE_MASK)
        class NumberType
        {
            uint32_t _type;

        public:
            NumberType() : _type(NT_SI) {}

            NumberType(uint32_t type) : _type(type) {}

            bool operator==(const uint32_t other) const
            {
                return _type == other;
            }

            uint32_t type() const
            {
                return _type;
            }

            void setLongLong()
            {
                _type |= NT_LONG_LONG_MASK;
            }
            bool isLongLong() const
            {
                return (_type & NT_LONG_LONG_MASK) == NT_LONG_LONG_MASK;
            }

            void setLong()
            {
                _type |= NT_LONG_MASK;
            }
            bool isLong() const
            {
                return (_type & NT_LONG_MASK) == NT_LONG_MASK;
            }

            bool isSigned() const
            {
                return (_type & NT_SIGNED_MASK) == NT_SIGNED_MASK;
            }
            void setSigned()
            {
                _type &= NT_SIGNED_MASK;
            }

            bool isUnsigned()
            {
                return (_type & NT_UNSIGNED_MASK) == NT_UNSIGNED_MASK;
            }
            void setUnsigned()
            {
                _type |= NT_UNSIGNED_MASK;
            }

            bool isFloating() const
            {
                return (_type & NT_FLOAT_MASK) == NT_FLOAT_MASK;
            }

            bool isInteger() const
            {
                return !isFloating();
            }

            void setShort()
            {
                _type |= NT_SHORT_MASK;
            }

            bool isShort() const
            {
                return (_type & NT_SHORT_MASK) == NT_SHORT_MASK;
            }
        };

        NumberType &operator|=(NumberType &, unsigned long);

    }  // namespace constants
}  // namespace lex

#endif