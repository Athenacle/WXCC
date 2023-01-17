#include "lex/lexer.h"

#include <vector>

#include "test.h"

using namespace lex;
using namespace constants;
using namespace types;

TEST(Lexer, keywords)
{
    const char kw[] =
        "auto break case char const continue default do double else enum extern float for goto if "
        "int long register return short signed sizeof static struct switch typedef union unsigned "
        "void volatile while";

    MemoryLexInputSource mlis;
    mlis.fill(kw, sizeof(kw) - 1);

    std::vector<Token> toks = {
        {KEY_AUTO},     {KEY_BREAK},  {KEY_CASE},    {KEY_CHAR},   {KEY_CONST},    {KEY_CONTINUE},
        {KEY_DEFAULT},  {KEY_DO},     {KEY_DOUBLE},  {KEY_ELSE},   {KEY_ENUM},     {KEY_EXTERN},
        {KEY_FLOAT},    {KEY_FOR},    {KEY_GOTO},    {KEY_IF},     {KEY_INT},      {KEY_LONG},
        {KEY_REGISTER}, {KEY_RETURN}, {KEY_SHORT},   {KEY_SIGNED}, {OP_SIZEOF},    {KEY_STATIC},
        {KEY_STRUCT},   {KEY_SWITCH}, {KEY_TYPEDEF}, {KEY_UNION},  {KEY_UNSIGNED}, {KEY_KVOID},
        {KEY_VOLATILE}, {KEY_WHILE}};

    toks.emplace_back();

    Lex l(&mlis);
    for (const auto& t : toks) {
        auto lex = l.getNextToken();
        ASSERT_EQ(lex, t);
    }
    auto lex = l.getNextToken();
    ASSERT_EQ(lex.token_type, T_NONE);
}


TEST(Lexer, keywordsPrefixID)
{
    const char kw[] =
        "auto_ breakk cases char_ constt2 continuee default_op doo doubled else1 enum100 "
        "extern_ float123 forr goto123 if_ "
        "intt long_a registerr returnnef shortted signed123 sizeoff staticc structs "
        "switches typedeff unionion unsignedc123 "
        "voidd volatiled whilee123_";

    MemoryLexInputSource mlis;
    mlis.fill(kw, sizeof(kw) - 1);

    std::vector<std::string> ids;
    test::split(ids, kw, " ");
    std::vector<Token> toks;

    for (const auto& id : ids) {
        Token tok(T_ID, id.c_str());
        toks.emplace_back(std::move(tok));
    }

    Lex l(&mlis);
    for (const auto& t : toks) {
        auto lex = l.getNextToken();
        EXPECT_EQ(lex, t) << t.token_value.string;
    }

    auto lex = l.getNextToken();
    ASSERT_EQ(lex.token_type, T_NONE);
}