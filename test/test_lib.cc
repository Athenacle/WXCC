#include <random>

#include "test.h"

namespace
{
    std::random_device rd;
}

const char* test::randomString(uint32_t length)
{
    char* mem = new char[length + 1];
    mem[length] = 0;

    for (uint32_t i = 0; i < length; i++) {
        char c = '\n';
        for (; c == '\n';)
            c = (rd() % ('~' - '!')) + '!';
        mem[i] = c;
    }

    return mem;
}

uint32_t test::random()
{
    return rd();
}

uint32_t test::random(uint32_t mod)
{
    return test::random() % mod;
}