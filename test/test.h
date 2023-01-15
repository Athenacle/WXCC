
#include <gtest/gtest.h>
#include <gmock/gmock.h>


namespace test
{
    const char* randomString(uint32_t length);

    uint32_t random();

    uint32_t random(uint32_t mod);
}  // namespace test