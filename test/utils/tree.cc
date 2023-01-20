#include "utils/tree.h"

#include "environment/identifier.h"
#include "utils/bit_container.h"

#include "test.h"

USING_V2
using namespace utils;

TEST(Utils, Tree)
{
    BitsContainer<10> tree;
    tree.set<4>(true);
    tree.test<6>();
}
