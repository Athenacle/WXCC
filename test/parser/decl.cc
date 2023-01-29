#include "parser/parser.h"

#include "test.h"

USING_V2
// NOLINTNEXTLINE
using testing::_;

using namespace lex;
using namespace parser;

class DeclarationTest : public testing::Test
{
protected:
    lex::MemoryLexInputSource mlis;
    ErrorManagerMock* m;
    lex::Lex lexer;
    parser::Parser parser;
    syntax::TranslationUnit unit;
    ErrorManagerMock mock_;


    DeclarationTest() : m(nullptr), lexer(&mlis), parser(lexer, m) {}

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


TEST_F(DeclarationTest, Conflict_Storage_Class_Specifier)
{
    std::vector<std::string> scs = {"extern", "static", "register", "auto", "typedef"};
    for (auto& first : scs) {
        for (auto& sec : scs) {
            ErrorManagerMock mock;
            MemoryLexInputSource is;
            Lex scanner(&is);
            Parser pp(scanner, &mock);

            std::string ss = first + " " + sec + " int i;\n";
            is.fill(ss);
            auto level = first == sec ? utils::ErrorManager::WARNING : utils::ErrorManager::ERROR;

            if (first == sec) {
                EXPECT_CALL(mock, output(_, _, _))
                    .WillOnce(ErrorMessageMatcher(level, "duplicate storage-class-specifier"));
            } else {
                EXPECT_CALL(mock, output(_, _, _))
                    .WillOnce(ErrorMessageMatcher(level, "conflict storage-class-specifier"));
            }
            pp.parse(unit);
        }
    }
}


TEST_F(DeclarationTest, Storage_Class_Specifier_Check)
{
    std::vector<std::string> scs = {"unsigned signed i;",
                                    "signed unsigned i;",
                                    "char int i;",
                                    "char long i;",
                                    "char short i;",
                                    "char float i;",
                                    "char double i;",
                                    "int float i;",
                                    "long float i;"};
    for (auto& first : scs) {
        ErrorManagerMock mock;
        MemoryLexInputSource is;
        Lex scanner(&is);
        Parser pp(scanner, &mock);
        is.fill(first);

        EXPECT_CALL(mock, output(_, _, _))
            .WillOnce(ErrorMessageMatcher(utils::ErrorManager::ERROR,
                                          "both '.+' and '.+' in declaration specifiers"));
        pp.parse(unit);
    }
}
