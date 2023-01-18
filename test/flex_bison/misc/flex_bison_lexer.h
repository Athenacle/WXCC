
#ifndef TEST_FB_PARSER_H_
#define TEST_FB_PARSER_H_

#if !defined yyFlexLexerOnce
//#undef yyFlexLexer
//#define yyFlexLexer BisonFlex_Lexer
#include <FlexLexer.h>
#endif

#include "bison_parser.tab.h"

#undef YY_DECL
#define YY_DECL test::FlexBison_Parser::symbol_type test::FlexBison_Lexer::next()

#define YY_DECL_IS_OURS
#define YY_NO_UNISTD_H

namespace test
{
    class FlexBison_Lexer : public yyFlexLexer
    {
    private:
        std::string filename;

    public:
        FlexBison_Lexer(std::istream *in) : yyFlexLexer(in) {}

        FlexBison_Lexer(std::istream *in, const std::string &fn) : FlexBison_Lexer(in)
        {
            filename = fn;
        }

        using FlexLexer::yylex;

        virtual FlexBison_Parser::symbol_type next();
    };
}  // namespace test


#endif
