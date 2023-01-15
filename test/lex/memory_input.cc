#include "test.h"
#include "lex/input.h"

using namespace lex;

#define TEST_GET_CHAR(pointer, input)                                                     \
    do {                                                                                  \
        EXPECT_EQ(*(pointer), input.next(LexInputSource::GET_TYPE::GET_CHAR)) << pointer; \
        pointer += 1;                                                                     \
    } while (0)


#define TEST_LOOKAHEAD(pointer, input)                                                      \
    do {                                                                                    \
        ASSERT_EQ(*(pointer), input.next(LexInputSource::GET_TYPE::LOOK_AHEAD)) << pointer; \
    } while (0)


#define BUFS 512

TEST(MemoryInput, MemoryLexInputSource)
{
    auto buf = test::randomString(BUFS);
    auto ptr = buf;

    MemoryLexInputSource mlis;
    mlis.fill(buf, BUFS);

    for (int i = 0; i < 10; i++) {
        TEST_GET_CHAR(ptr, mlis);
    }
    for (int i = 0; i < 40; i++) {
        if (test::random(3) == 0) {
            TEST_LOOKAHEAD(ptr, mlis);
        } else {
            TEST_GET_CHAR(ptr, mlis);
        }
    }

    delete[] buf;
}

TEST(MemoryInput, DualFill)
{
    auto buf = test::randomString(BUFS);
    auto ptr = buf;

    MemoryLexInputSource mlis;
    mlis.fill(buf, BUFS);

    for (int i = 0; i < 20; i++) {
        TEST_GET_CHAR(ptr, mlis);
    }
    for (int i = 0; i < 30; i++) {
        if (test::random(3) == 0) {
            TEST_LOOKAHEAD(ptr, mlis);
        } else {
            TEST_GET_CHAR(ptr, mlis);
        }
    }

    while (*ptr) {
        TEST_GET_CHAR(ptr, mlis);
    }
    ASSERT_EQ(*ptr, '\0');
    delete[] buf;

    buf = test::randomString(BUFS);
    ptr = buf;
    mlis.fill(buf, BUFS);

    for (int i = 0; i < 20; i++) {
        TEST_GET_CHAR(ptr, mlis);
    }
    for (int i = 0; i < 30; i++) {
        if (test::random(3) == 0) {
            TEST_LOOKAHEAD(ptr, mlis);
        } else {
            TEST_GET_CHAR(ptr, mlis);
        }
    }

    delete[] buf;
}


TEST(MemoryInput, RandomFill)
{
    auto buf = test::randomString(1024);

    MemoryLexInputSource mlis;
    mlis.fill(buf, 1024);

    std::string str(buf);
    auto ptr = str.c_str();
    delete[] buf;

    for (int i = 0; i < 1000; i++) {
        auto op = test::random(40);
        if (op >= 37) {
            auto nb = test::randomString(512);
            mlis.fill(nb, 512);
            auto pos = ptr - str.c_str();
            str = str.append(nb);
            ptr = str.c_str() + pos;
            delete[] nb;
        }

        if (op <= 10) {
            TEST_LOOKAHEAD(ptr, mlis);
        } else {
            TEST_GET_CHAR(ptr, mlis);
        }
    }
}
