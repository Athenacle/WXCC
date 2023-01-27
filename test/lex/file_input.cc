#include "lex/input.h"

#include "test.h"

using namespace lex;

#define FILENAME "lex_input_test_file.t"

namespace
{
    const uint32_t TOTAL_SIZE = 4096 * 3 + 2048;
}

class FileInput : public testing::Test
{
protected:
    const char* buf;

    static void SetUpTestSuite() {}

    static void TearDownTestSuite() {}

    void SetUp() override
    {
        buf    = test::randomString(TOTAL_SIZE);
        auto f = fopen(FILENAME, "wb+");
        ASSERT_NE(f, nullptr);

        if (f) {
            auto w = fwrite(buf, 1, TOTAL_SIZE, f);
            ASSERT_EQ(w, TOTAL_SIZE);
            fclose(f);
        }
    }

    void TearDown() override
    {
        std::remove(FILENAME);
        delete[] buf;
    }
};


#define TEST_GET_CHAR(pointer, input)                                                     \
    do {                                                                                  \
        EXPECT_EQ(*(pointer), input.next(LexInputSource::GET_TYPE::GET_CHAR)) << pointer; \
        pointer += 1;                                                                     \
    } while (0)


#define TEST_LOOKAHEAD(pointer, input)                                                      \
    do {                                                                                    \
        ASSERT_EQ(*(pointer), input.next(LexInputSource::GET_TYPE::LOOK_AHEAD)) << pointer; \
    } while (0)


TEST_F(FileInput, FileInputSource)
{
    FileLexInputSource flis;
    auto ptr = buf;
    ASSERT_TRUE(flis.openFile(FILENAME));

    for (uint32_t i = 0; i < TOTAL_SIZE; i++) {
        if (test::random(3) == 0) {
            TEST_LOOKAHEAD(ptr, flis);
        } else {
            TEST_GET_CHAR(ptr, flis);
        }
    }
}

TEST_F(FileInput, AllFill)
{
    FileLexInputSource flis;
    auto ptr = buf;
    ASSERT_TRUE(flis.openFile(FILENAME));
    while (flis.fillBuffer()) {
    }

    while (*ptr) {
        TEST_GET_CHAR(ptr, flis);
    }
}

TEST_F(FileInput, RandomFillFileInputSource)
{
    FileLexInputSource flis;
    auto ptr = buf;
    ASSERT_TRUE(flis.openFile(FILENAME));

    for (uint32_t i = 0; i < TOTAL_SIZE; i++) {
        auto op = test::random(30);
        if (op < 10) {
            TEST_LOOKAHEAD(ptr, flis);
        } else {
            TEST_GET_CHAR(ptr, flis);
        }
        if (op >= 29) {
            flis.fillBuffer();
        }
    }
}
