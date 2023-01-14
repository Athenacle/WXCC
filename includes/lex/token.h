#ifndef LEX_TOKEN_H_
#define LEX_TOKEN_H_

#include "lex/constants.h"

namespace lex
{
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
}  // namespace lex

#endif