#include "parser/parser.h"

#include "test.h"

USING_V2
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

protected:
    DeclarationTest() : mlis(), lexer(&mlis), parser(lexer, &mock_)
    {
        m = nullptr;
    }

    virtual void SetUp() override
    {
        m = new ErrorManagerMock;
        parser::Parser p(lexer, m);
        parser = std::move(p);
    }

    virtual void TearDown() override
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
