/* lexer debuggger
* Wang Xiaochun
* zjjhwxc@gmail.com
* January 23, 2013
*
*/
#include "lex/output.h"

#include <fmt/format.h>

#include <algorithm>
#include <unordered_map>

#include "lex/lexer.h"

using namespace lex;
using namespace lex::constants;
using namespace lex::types;

namespace
{
    void buildKeymap(std::unordered_map<KEYWORD, std::string> &keys)
    {
        auto func = [](std::string::traits_type::char_type c) { return std::tolower(c); };

#define KEYWORDS_ADD(k)                                         \
    do {                                                        \
        std::string kv(#k);                                     \
        std::transform(kv.begin(), kv.end(), kv.begin(), func); \
        keys.emplace(KEY_##k, kv);                              \
    } while (false)

        keys.emplace(KEY_KVOID, "void");
        // clang-format off
        KEYWORDS_ADD(AUTO);     KEYWORDS_ADD(BREAK);    KEYWORDS_ADD(CASE);
        KEYWORDS_ADD(CHAR);     KEYWORDS_ADD(CONST);    KEYWORDS_ADD(CONTINUE);
        KEYWORDS_ADD(DEFAULT);  KEYWORDS_ADD(DO);       KEYWORDS_ADD(DOUBLE);
        KEYWORDS_ADD(ELSE);     KEYWORDS_ADD(ENUM);     KEYWORDS_ADD(EXTERN);
        KEYWORDS_ADD(FLOAT);    KEYWORDS_ADD(FOR);      KEYWORDS_ADD(GOTO);
        KEYWORDS_ADD(IF);       KEYWORDS_ADD(INT);      KEYWORDS_ADD(LONG);
        KEYWORDS_ADD(REGISTER); KEYWORDS_ADD(RETURN);   KEYWORDS_ADD(SHORT);
        KEYWORDS_ADD(SIGNED);   KEYWORDS_ADD(SIZEOF);   KEYWORDS_ADD(STATIC);
        KEYWORDS_ADD(STRUCT);   KEYWORDS_ADD(SWITCH);   KEYWORDS_ADD(TYPEDEF);
        KEYWORDS_ADD(UNION);    KEYWORDS_ADD(UNSIGNED); KEYWORDS_ADD(VOLATILE);
        KEYWORDS_ADD(WHILE);
        // clang-format on

#undef KEYWORDS_ADD
    }

    const std::string &dispatchKeywords(KEYWORD key)
    {
        static std::unordered_map<KEYWORD, std::string> keys;
        if (unlikely(keys.size() == 0)) {
            buildKeymap(keys);
        }
        return keys[key];
    }

    void buildOperators(std::unordered_map<OP, std::tuple<std::string, const char *>> &ops)
    {
#define OPERATOR_ADD(op, c)                            \
    do {                                               \
        auto msg = fmt::format("{:<13}\t{}", #op, c);  \
        ops.emplace(OP_##op, std::make_tuple(msg, c)); \
    } while (false)

        // clang-format off
        OPERATOR_ADD(LOGNOT, "!");          OPERATOR_ADD(MOD, "%");         OPERATOR_ADD(BITXOR, "^");
        OPERATOR_ADD(BITAND, "&");          OPERATOR_ADD(MULT, "*");        OPERATOR_ADD(PLUS, "+");
        OPERATOR_ADD(MINUS, "-");           OPERATOR_ADD(BITNOT, "~");      OPERATOR_ADD(BITOR, "|");
        OPERATOR_ADD(DIV, "/");             OPERATOR_ADD(QUESTION, "?");    OPERATOR_ADD(POINTER, "->");
        OPERATOR_ADD(INC, "++");            OPERATOR_ADD(DEC, "--");        OPERATOR_ADD(LEFTSHIFT, "<<");
        OPERATOR_ADD(RIGHTSHIFT, ">>");     OPERATOR_ADD(LT, "<");          OPERATOR_ADD(GT, ">");
        OPERATOR_ADD(LE, "<=");             OPERATOR_ADD(GE, ">=");         OPERATOR_ADD(EQ, "==");
        OPERATOR_ADD(NE, "!=");             OPERATOR_ADD(LOGAND, "&&");     OPERATOR_ADD(LOGOR, "||");
        OPERATOR_ADD(LEFTBRACK, "(");       OPERATOR_ADD(RIGHTBRACK, ")");  OPERATOR_ADD(LEFTSQBRAC, "[");
        OPERATOR_ADD(RIGHTSQBRAC, "]");     OPERATOR_ADD(LEFTBRACE, "{");   OPERATOR_ADD(RIGHTBRACE, "}");
        OPERATOR_ADD(COMMA, ",");           OPERATOR_ADD(COLON, ":");       OPERATOR_ADD(SEMICOLON, ";");
        OPERATOR_ADD(EPPIPSIS, "...");      OPERATOR_ADD(PERIOD, ".");      OPERATOR_ADD(ASSIGN, "=");
        OPERATOR_ADD(ALPLUS, "+=");         OPERATOR_ADD(ALMINUS, "-=");    OPERATOR_ADD(ALMULT, "*=");
        OPERATOR_ADD(ALDIV, "/=");          OPERATOR_ADD(ALMOD, "%=");      OPERATOR_ADD(ALLEFTSHIFT, "<<=");
        OPERATOR_ADD(ALRIGHTSHIFT, ">>=");  OPERATOR_ADD(ALBITAND, "&=");   OPERATOR_ADD(ALBITXOR, "^=");
        OPERATOR_ADD(ALBITOR, "|=");        OPERATOR_ADD(SIZEOF, "sizeof");
        // clang-format on
    }

    const std::string &dispatchOperator(OP op)
    {
        static std::unordered_map<OP, std::tuple<std::string, const char *>> ops;
        if (unlikely(ops.size() == 0)) {
            buildOperators(ops);
        }

        return std::get<0>(ops[op]);
    }

    const char *TokenType(const Token &tok)
    {
#define TYPE(t, s) \
    case t:        \
        return s

        switch (tok.token_type) {
            TYPE(T_ID, "Identifier");
            TYPE(T_CHAR_CON, "Charater Constant");
            TYPE(T_FLOAT_CON, "Floating Point Constant");
            TYPE(T_INT_CON, "Integer Constant");
            TYPE(T_KEY, "KEYWORD");
            TYPE(T_OPERATOR, "Operator");
            TYPE(T_STRING, "String Constant");
            default:
                assert(0);
        }
    }

    std::string TokenValue(const Token &tok)
    {
        switch (tok.token_type) {
            case T_ID:
            case T_STRING:
                return tok.token_value.string;
            case T_INT_CON:
            case T_CHAR_CON:
            case T_FLOAT_CON:
                return fmt::format("{}", *tok.token_value.numVal);
            case T_KEY:
                return dispatchKeywords(tok.token_value.keyword);
            case T_OPERATOR:
                return dispatchOperator(tok.token_value.op);
            default:
                assert(0);
        }
    }
}  // namespace

namespace fmt
{

    template <>
    struct formatter<Number> {
        constexpr auto parse(fmt::format_parse_context &ctx)
        {
            return ctx.begin();
        }
        template <typename FormatContext>
        auto format(const Number &t, FormatContext &ctx) const
        {
            const char *numType = "unknown";

            switch (t.numberType.type()) {
                case NT_FL:
                    numType = "float";
                    break;
                case NT_DB:
                    numType = "double";
                    break;
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
                default:
                    assert(0);
            }
            return fmt::format_to(ctx.out(),
                                  "{:<13}\t{}",
                                  numType,
                                  t.type == T_FLOAT_CON ? t.val.d_value : t.val.i_value);
        }
    };


    template <>
    struct formatter<Token> {
        constexpr auto parse(fmt::format_parse_context &ctx)
        {
            return ctx.begin();
        }


        template <typename FormatContext>
        auto format(const Token &t, FormatContext &ctx) const
        {
            return fmt::format_to(ctx.out(), "{:<20}| {}", TokenType(t), TokenValue(t));
        }
    };
}  // namespace fmt

int LexOutput::output(void)
{
    int count = 0;
    do {
        Token tok = lexer.getNextToken();
        if (!tok) {
            break;
        }
        count += 1;
        fmt::print(fp, "#{:<5}{}\n", count, tok);
    } while (true);
    return 0;
}

LexOutput::LexOutput(lex::Lex &l, FILE *f) : lexer(l), fp(f) {}
