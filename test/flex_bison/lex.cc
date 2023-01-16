#include "flex_bison/misc/fb_fwd.h"
#include "flex_bison/misc/flex_bison.h"
#include "lex/lexer.h"

#include "test.h"

using namespace lex;

class FlexBisonLexer : public testing::Test
{
protected:
    static const char* buf;

    static const char* filename;

    FILE* yyfp;

    FileLexInputSource flis;
    Lex l;

    static void SetUpTestSuite()
    {
        filename = "flex_bison_random_lexer_test";

        buf = test::randomSourceCode();
        auto len = strlen(buf);

        auto f = fopen(filename, "wb+");
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

    virtual void SetUp() override
    {
        flis.openFile(filename);
        l.setSource(&flis);

        yyfp = fopen(filename, "rb");
        yyin = yyfp;
        yyfilename = filename;
    }

    virtual void TearDown() override
    {
        yyReset();
        if (yyfp) {
            fclose(yyfp);
        }
    }
};
const char* FlexBisonLexer::filename;
const char* FlexBisonLexer::buf = nullptr;

TEST_F(FlexBisonLexer, randomLexer)
{
    int i = 0;
    while (true) {
        Token fb(flex());
        Token lexer(l.getNextToken());

        EXPECT_EQ(fb, lexer);

        if (fb.token_type == constants::T_NONE) {
            break;
        }
#if 0
        if (fb.token_pos.place != lexer.token_pos.place) {
            std::cout << "mis. fb place " << fb.token_pos.place << " | lexer place "
                      << lexer.token_pos.place << " line: " << fb.token_pos.line << std::endl;
        }
        i++;
#endif
    }
}
