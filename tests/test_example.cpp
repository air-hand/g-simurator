#include <stdexcept>
#include <gtest/gtest.h>

namespace
{

TEST(test_example, foobar)
{
    EXPECT_NE(std::string("foo"), std::string("bar"));
}

TEST(test_example, integer)
{
    EXPECT_EQ(1, 1);
}

}
