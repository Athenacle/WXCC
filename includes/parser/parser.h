#ifndef PARSER_PARSER_H_
#define PARSER_PARSER_H_

#include "system.h"

#include <queue>

#include "lex/lexer.h"
#include "semantic/type.h"
#include "syntax/translation_unit.h"
#include "utils/bit_container.h"
#include "utils/error_manager.h"

NAMESPACE_BEGIN
namespace parser
{
    class Declarator;

    class Parser
    {
        using Token           = lex::types::Token;
        using TranslationUnit = syntax::TranslationUnit;
        using Type            = semantic::Type;

        lex::Lex &lexer_;
        utils::ErrorManager *mgr_;
        std::queue<Token> lookahead_;

        Parser() = delete;

        Token next();

        void pushback(Token &&);

        // declarations
        Type *parseDeclarationSpecifiers(TranslationUnit &);
        void parseDeclarations(TranslationUnit &);
        void parseExternalDeclaration(TranslationUnit &);
        void parseDeclarator(TranslationUnit &, Type *);
        void parseInitDeclaratorList(TranslationUnit &, Type *);
        void parsePointer(TranslationUnit &, Type *, Declarator *);

        void parseTranslationUnit(TranslationUnit &);

    public:
        Parser(lex::Lex &l, V2::utils::ErrorManager *m) : lexer_(l), mgr_(m) {}

        Parser &operator=(Parser &&) noexcept;

        bool parse(TranslationUnit &);
    };
}  // namespace parser
NAMESPACE_END

#endif
