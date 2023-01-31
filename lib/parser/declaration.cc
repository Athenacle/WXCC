
#include "environment/identifier.h"
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
using namespace environment;
using namespace lex::tools;


namespace
{
    bool checkQualifier(const Token &tok, const Type *t, ErrorManager *mgr)
    {
        assert(is(tok, T_KEY));
        auto kw   = tok.token_value.keyword;
        auto &pos = tok.token_pos;
        if (t->qualifierContainer().none()) {
            return true;
        }

        if (auto exists = t->findQualifier(); exists == kw) {
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

    class Declarator
    {
        PointerType *pointer_;
    };

}  // namespace

void Parser::parseDeclarations(TranslationUnit &unit)
{
    auto specifier = parseDeclarationSpecifiers(unit);
}

Type *Parser::parseDeclarationSpecifiers(TranslationUnit & /*unused*/)
{
    Type *ret = new Type;
    auto &ttl = ret->qualifierContainer();
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
            && checkQualifier(peek, ret, mgr_)) {
            if (kw == KEY_CONST) {
                ttl.set<Q_CONST>(true);
            } else if (kw == KEY_VOLATILE) {
                ttl.set<Q_VOLATILE>(true);
            } else if (kw == KEY_TYPEDEF) {
                ttl.set<Q_TYPEDEF>(true);
            } else if ((peek == KEY_EXTERN)) {
                ttl.set<Q_EXTERN>(true);
            } else if ((peek == KEY_STATIC)) {
                ttl.set<Q_STATIC>(true);
            } else if ((peek == KEY_AUTO)) {
                ttl.set<Q_AUTO>(true);
            } else if ((peek == KEY_REGISTER)) {
                ttl.set<Q_REGISTER>(true);
            } else {
                UNREACHABLE
            }
        } else if (isBaseType(peek)) {
            ret->addBaseType(keywordToTypeOperator(kw));
        } else {
            pushback(std::move(peek));
            break;
        }
    }

    auto msgs = ret->checkType();

    for (const auto &msg : msgs) {
        const auto &[level, str] = msg;
        mgr_->output(level, pos, str);
    }

    return ret;
}  // namespace


NAMESPACE_END
