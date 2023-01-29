#include "utils/bit_container.h"

#include <bitset>

#include "test.h"

using namespace std;
USING_V2
using utils::BitsContainer;

TEST(Utils, BitContainerRandomTest)
{
    static constexpr size_t BITS = 513;

    BitsContainer<BITS> bc;
    bitset<BITS> bs;

    EXPECT_TRUE(bc.none());
    EXPECT_TRUE(bs.none());
    EXPECT_FALSE(bc.all());
    EXPECT_FALSE(bc.any());

    for (size_t i = 0; i < BITS * 4; i++) {
        auto off = test::random(BITS);
        EXPECT_EQ(bc.test(off), bs.test(off));
        bc.set(off, (off & 1) == 0);
        bs.set(off, (off & 1) == 0);
        EXPECT_EQ(bc.count(), bs.count());
    }
    EXPECT_EQ(bc.any(), bs.any());
    EXPECT_EQ(bc.all(), bs.all());

    for (size_t i = 0; i < BITS; i++) {
        EXPECT_EQ(bc.test(i), bs.test(i));
        bc.set(i, true);
        bs.set(i, true);
        EXPECT_EQ(bc.count(), bs.count());
    }
    EXPECT_TRUE(bs.all());
    EXPECT_TRUE(bc.all());
}
