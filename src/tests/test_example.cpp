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

TEST(test_example, pointer)
{
    EXPECT_EQ(nullptr, nullptr);
    if constexpr (std::is_pointer_v<std::unique_ptr<int>>) {
        ASSERT_FALSE(true);
    }
}

}
