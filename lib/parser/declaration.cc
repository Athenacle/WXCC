
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
        assert(!t.allZero());
        if (t.test<Type::Q_AUTO>()) {
            return KEY_AUTO;
        }
        if (t.test<Type::Q_EXTERN>()) {
            return KEY_EXTERN;
        } else if (t.test<Type::Q_REGISTER>()) {
            return KEY_REGISTER;
        } else if (t.test<Type::Q_STATIC>()) {
            return KEY_STATIC;
        } else if (t.test<Type::Q_TYPEDEF>()) {
            return KEY_TYPEDEF;
        }
    }

    bool checkQualifier(const Token &tok, const Type::QualifierContainer &t, ErrorManager *mgr)
    {
        assert(is(tok, T_KEY));
        auto kw = tok.token_value.keyword;
        auto &pos = tok.token_pos;
        if (t.allZero()) {
            return true;
        }
        if (auto exists = findQualifier(t); exists == kw) {
            mgr->warning(pos, "duplicate storage-class-specifier '{}' found. ignore.", kw);
        } else {
            mgr->error(pos,
                       "conflit storage-class-specifier '{}' found, previous is '{}'. ignore.",
                       kw,
                       exists);
        }

        return false;
    }
}  // namespace

Type *Parser::parseDeclarationSpecifiers(TranslationUnit & /*unused*/)
{
    Type::QualifierContainer ttl;
    Type *ret = nullptr;


    while (true) {
        Token peek = next();
        if (!is(peek, T_KEY)) {
            pushback(std::move(peek));
            break;
        }

        auto kw = peek.token_value.keyword;

        if (matchQualifier(peek)) {
            if (ret == nullptr) {
                ret = new Type;
            }

            if (kw == KEY_CONST) {
                ret->addBaseType(TypeOperator::TO_CONST);
            } else if (kw == KEY_VOLATILE) {
                ret->addBaseType(TypeOperator::TO_VOLATILE);
            }
        } else if (isStorageClassSpecifier(peek)) {
            if (kw == KEY_TYPEDEF) {
                if (checkQualifier(peek, ttl, mgr_)) {
                    ttl.set<Type::Q_TYPEDEF>(true);
                }
            } else if ((peek == KEY_EXTERN)) {
                if (checkQualifier(peek, ttl, mgr_)) {
                    ttl.set<Type::Q_EXTERN>(true);
                }
            } else if ((peek == KEY_STATIC)) {
                if (checkQualifier(peek, ttl, mgr_)) {
                    ttl.set<Type::Q_STATIC>(true);
                }
            } else if ((peek == KEY_AUTO)) {
                if (checkQualifier(peek, ttl, mgr_)) {
                    ttl.set<Type::Q_AUTO>(true);
                }
            } else if ((peek == KEY_REGISTER)) {
                if (checkQualifier(peek, ttl, mgr_)) {
                    ttl.set<Type::Q_REGISTER>(true);
                }
            } else {
                assert(0);
            }
        }
    }


    return ret;
}


NAMESPACE_END
