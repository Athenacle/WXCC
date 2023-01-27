#include "parser/parser.h"

#include "test.h"

USING_V2
// NOLINTNEXTLINE
using testing::_;

class DeclarationTest : public testing::Test
{
protected:
    lex::MemoryLexInputSource mlis;
    lex::Lex lexer;
    parser::Parser parser;
    syntax::TranslationUnit unit;
    ErrorManagerMock mock_;

    ErrorManagerMock *m;

    DeclarationTest() : lexer(&mlis), parser(lexer, &mock_)
    {
        m = nullptr;
    }

    void SetUp() override
    {
        m = new ErrorManagerMock;
        parser::Parser p(lexer, m);
        parser = std::move(p);
    }

    void TearDown() override
    {
        delete m;
    }
};


TEST_F(DeclarationTest, Duplicate_Storage_Class_Specifier)
{
    char case1[] = "extern static int i;";
    mlis.fill(case1);
    EXPECT_CALL(*m, output(utils::ErrorManager::ERROR, _, _)).WillOnce(testing::Return());


    parser.parse(unit);
}
