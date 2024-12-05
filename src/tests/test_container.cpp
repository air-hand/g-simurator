#include <gtest/gtest.h>

import utils;

namespace
{

namespace container = sim::utils::container;

TEST(test_bounded_buffer, push_pop)
{
    container::BoundedBuffer<int> buffer(3);
    buffer.push(100);
    buffer.push(41);
    buffer.push(42);

    EXPECT_EQ(buffer.size(), 3);

    buffer.push(43);
    EXPECT_EQ(buffer.size(), 3);

    EXPECT_EQ(buffer.pop(), 41);
    EXPECT_EQ(buffer.pop(), 42);
    EXPECT_EQ(buffer.pop(), 43);

    EXPECT_EQ(buffer.size(), 0);
}

}
