#include "flex_bison/misc/fb_fwd.h"
#include "flex_bison/misc/flex_bison.h"
#include "lex/lexer.h"

#include "test.h"

using namespace lex;

class FlexBisonLexer : public testing::Test
{
    std::ifstream yyin;

protected:
    static const char* buf;

    static const char* filename;

    static uint32_t tokenCount;

    test::FlexBison_Lexer* flexin;

    FileLexInputSource flis;
    Lex lexer;

    static void SetUpTestSuite()
    {
        tokenCount = test::random(10000) + 5000;

        filename   = "flex_bison_random_lexer_test";

        buf        = test::randomSourceCode(tokenCount);
        auto len   = strlen(buf);

        auto f     = fopen(filename, "wb+");
        ASSERT_NE(f, nullptr);

        if (f) {
            auto w = fwrite(buf, 1, strlen(buf), f);
            ASSERT_EQ(len, w);
            fclose(f);
        }
    }

    static void TearDownTestSuite()
    {
        std::remove(filename);
        delete[] buf;
        buf = nullptr;
    }

    void SetUp() override
    {
        flis.openFile(filename);
        lexer.setSource(&flis);

        ASSERT_TRUE(test::openFile(yyin, filename));
        flexin     = new test::FlexBison_Lexer(&yyin);

        yyfilename = filename;
    }

    void TearDown() override
    {
        yyReset();
        delete flexin;
        flexin = nullptr;
    }
};
const char* FlexBisonLexer::filename;
const char* FlexBisonLexer::buf     = nullptr;
uint32_t FlexBisonLexer::tokenCount = 0;

TEST_F(FlexBisonLexer, randomLexer)
{
    MAYBE_UNUSED uint32_t i = 0;
    while (true) {
        Token fb(flex(flexin));
        Token lexerTok(lexer.getNextToken());

        EXPECT_EQ(fb, lexerTok);

        if (fb.token_type == constants::T_NONE) {
            break;
        } else {
            EXPECT_EQ(fb.token_pos.line, lexerTok.token_pos.line);
            EXPECT_LE(std::abs(lexerTok.token_pos.place - fb.token_pos.place), 1);
        }
        i++;
    }
}
