
#include "lex/print.h"
#include "lex/tools.h"
#include "parser/parser.h"

USING_V2

NAMESPACE_BEGIN
using namespace parser;
using namespace syntax;
using namespace utils;
using namespace semantic;
using semantic::TypeOperator;
using namespace lex;
using namespace lex::types;
using namespace lex::constants;
using namespace lex::tools;


namespace
{
    KEYWORD findQualifier(const Type::QualifierContainer &t)
    {
        assert(!t.none());
        if (t.test<Type::Q_AUTO>()) {
            return KEY_AUTO;
        }
        if (t.test<Type::Q_EXTERN>()) {
            return KEY_EXTERN;
        }
        if (t.test<Type::Q_REGISTER>()) {
            return KEY_REGISTER;
        }
        if (t.test<Type::Q_STATIC>()) {
            return KEY_STATIC;
        }
        if (t.test<Type::Q_TYPEDEF>()) {
            return KEY_TYPEDEF;
        }
        UNREACHABLE
        return KEY_NONE;
    }

    bool checkQualifier(const Token &tok, const Type::QualifierContainer &t, ErrorManager *mgr)
    {
        assert(is(tok, T_KEY));
        auto kw   = tok.token_value.keyword;
        auto &pos = tok.token_pos;
        if (t.none()) {
            return true;
        }

        if (auto exists = findQualifier(t); exists == kw) {
            mgr->warning(pos, FMT("duplicate storage-class-specifier '{}' found. ignore."), kw);
        } else {
            mgr->error(
                pos,
                FMT("conflict storage-class-specifier '{}' found, previous is '{}'. ignore."),
                kw,
                exists);
        }

        return true;
    }

    TypeOperator keywordToTypeOperator(KEYWORD kw)
    {
#define CASE_BASE(key, op)         \
    case key:                      \
        return (TypeOperator::op); \
        break;

#define CASE_BASE_RAW(type) CASE_BASE(KEY_##type, TO_##type)
        switch (kw) {
            CASE_BASE_RAW(CHAR);
            CASE_BASE_RAW(FLOAT);
            CASE_BASE_RAW(INT);
            CASE_BASE_RAW(LONG);
            CASE_BASE_RAW(UNION);
            CASE_BASE_RAW(ENUM);
            CASE_BASE_RAW(SIGNED);
            CASE_BASE_RAW(UNSIGNED);
            CASE_BASE_RAW(STRUCT);
            CASE_BASE_RAW(DOUBLE);
            CASE_BASE_RAW(SHORT);
            CASE_BASE(KEY_TVOID, TO_VOID);
            CASE_BASE(KEY_KVOID, TO_VOID);
            default:
                UNREACHABLE
#undef CASE_BASE_RAW
#undef CASE_BASE
        }
        return TypeOperator::TO_NONE;
    }
}  // namespace

Type Parser::parseDeclarationSpecifiers(TranslationUnit & /*unused*/)
{
    Type ret;
    auto &ttl = ret.qualifierContainer();
    Position pos;


    while (true) {
        Token peek = next();
        pos        = peek.token_pos;

        if (!is(peek, T_KEY)) {
            pushback(std::move(peek));
            break;
        }

        auto kw = peek.token_value.keyword;

        if ((matchQualifier(peek) || isStorageClassSpecifier(peek))
            && checkQualifier(peek, ttl, mgr_)) {
            if (kw == KEY_CONST) {
                ttl.set<Type::Q_CONST>(true);
            } else if (kw == KEY_VOLATILE) {
                ttl.set<Type::Q_VOLATILE>(true);
            } else if (kw == KEY_TYPEDEF) {
                ttl.set<Type::Q_TYPEDEF>(true);
            } else if ((peek == KEY_EXTERN)) {
                ttl.set<Type::Q_EXTERN>(true);
            } else if ((peek == KEY_STATIC)) {
                ttl.set<Type::Q_STATIC>(true);
            } else if ((peek == KEY_AUTO)) {
                ttl.set<Type::Q_AUTO>(true);
            } else if ((peek == KEY_REGISTER)) {
                ttl.set<Type::Q_REGISTER>(true);
            } else {
                UNREACHABLE
            }
        } else if (isBaseType(peek)) {
            ret.addBaseType(keywordToTypeOperator(kw));
        } else {
            pushback(std::move(peek));
            break;
        }
    }

    auto msgs = ret.checkType();

    for (const auto &msg : msgs) {
        const auto &[level, str] = msg;
        mgr_->output(level, pos, str);
    }

    return ret;
}  // namespace


NAMESPACE_END
