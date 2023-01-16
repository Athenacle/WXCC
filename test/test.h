
#include <gmock/gmock.h>
#include <gtest/gtest.h>


namespace test
{
    const char* randomString(uint32_t length);

    uint32_t random();

    uint32_t random(uint32_t mod);

#ifdef HAVE_FLEX_BISON
    const char* randomSourceCode();
#endif
}  // namespace test