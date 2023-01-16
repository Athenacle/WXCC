#ifndef LEX_TOKEN_H_
#define LEX_TOKEN_H_

#include <cstdint>

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
                uint64_t i_value;
            } val;

            Number(const Number &num) : Number()
            {
                type = num.type;
                numberType = num.numberType;
                val.d_value = num.val.d_value;
            }

            Number()
            {
                val.d_value = 0;
            }

            bool operator==(const Number &other) const
            {
                if (type != other.type) {
                    return false;
                }
                return val.d_value == other.val.d_value;
            }
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

            bool operator==(const Token &) const;

            bool inline operator==(constants::OP _op) const
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

            operator bool()
            {
                return token_type != constants::T_NONE;
            }

            Token &operator=(std::nullptr_t)
            {
                token_type = constants::T_NONE;
                return *this;
            }

            Token &operator=(const Token &) = default;

            Token(constants::KEYWORD);
            explicit Token(constants::TYPE, const char *);
            explicit Token(Number *);
            Token(const Token &tok) = default;
            explicit Token(constants::OP);

            Token(Token &&) = default;

            Token()
            {
                token_type = constants::T_NONE;
                token_pos = nullptr;
                token_value.id_name = nullptr;
            }

            ~Token();
        };
    };  //namespace types
}  // namespace lex

#endif