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
        mgr_->info(FMT("empty source file: {}"), lexer_.filename());
        return;
    }
    pushback(std::move(tok));

    parseDeclarations(unit);
}

bool Parser::parse(TranslationUnit &unit)
{
    parseTranslationUnit(unit);

    return true;
}

Token Parser::next()
{
    if (lookahead_.empty()) {
        return lexer_.getNextToken();
    }
    Token ret(lookahead_.front());
    lookahead_.pop();
    return ret;
}

void Parser::pushback(Token &&tok)
{
    lookahead_.emplace(std::move(tok));
}

Parser &Parser::operator=(Parser &&p) noexcept
{
    lexer_     = std::move(p.lexer_);
    lookahead_ = std::move(p.lookahead_);
    mgr_       = p.mgr_;
    return *this;
}

NAMESPACE_END
