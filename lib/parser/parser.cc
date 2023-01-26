#include "parser/parser.h"

#include "lex/tools.h"

NAMESPACE_BEGIN
using namespace parser;
using namespace syntax;
using namespace utils;
using namespace lex;
using namespace lex::types;
using namespace lex::constants;
using namespace lex::tools;


void Parser::parseTranslationUnit(TranslationUnit &unit)
{
    Token tok(next());
    if (tok == T_NONE) {
        // empty translation unit
        mgr_->info("empty source file: {}", lexer_.filename());
        return;
    } else {
        pushback(std::move(tok));

        auto specifier = parseDeclarationSpecifiers(unit);
    }
}

bool Parser::parse(TranslationUnit &unit)
{
    parseTranslationUnit(unit);

    return true;
}

Token Parser::next()
{
    if (lookahead_.size() == 0) {
        return lexer_.getNextToken();
    } else {
        Token ret(lookahead_.front());
        lookahead_.pop();
        return ret;
    }
}

void Parser::pushback(Token &&tok)
{
    lookahead_.emplace(std::move(tok));
}

Parser &Parser::operator=(Parser &&p)
{
    lexer_ = std::move(p.lexer_);
    lookahead_ = std::move(p.lookahead_);
    mgr_ = std::move(p.mgr_);
    return *this;
}

NAMESPACE_END
