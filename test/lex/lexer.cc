#include "lex/lexer.h"

#include <vector>

#include "test.h"

using namespace lex;
using namespace constants;

TEST(Lexer, keywords)
{
    const char kw[] =
        "auto break case char const continue default do double else enum extern float for goto if "
        "int long register return short signed sizeof static struct switch typedef union unsigned "
        "void volatile while";

    MemoryLexInputSource mlis;
    mlis.fill(kw, sizeof(kw) - 1);

    std::vector<lex::types::Token> toks = {
        {KEY_AUTO},     {KEY_BREAK},  {KEY_CASE},    {KEY_CHAR},   {KEY_CONST},    {KEY_CONTINUE},
        {KEY_DEFAULT},  {KEY_DO},     {KEY_DOUBLE},  {KEY_ELSE},   {KEY_ENUM},     {KEY_EXTERN},
        {KEY_FLOAT},    {KEY_FOR},    {KEY_GOTO},    {KEY_IF},     {KEY_INT},      {KEY_LONG},
        {KEY_REGISTER}, {KEY_RETURN}, {KEY_SHORT},   {KEY_SIGNED}, {KEY_SIZEOF},   {KEY_STATIC},
        {KEY_STRUCT},   {KEY_SWITCH}, {KEY_TYPEDEF}, {KEY_UNION},  {KEY_UNSIGNED}, {KEY_KVOID},
        {KEY_VOLATILE}, {KEY_WHILE}};

    toks.emplace_back();

    Lex l(&mlis);
    for (const auto& t : toks) {
        auto lex = l.getNextToken();
        ASSERT_EQ(lex, t);
    }
}