#include <gtest/gtest.h>

import utils;

namespace
{

namespace container = sim::utils::container;

TEST(test_ring_buffer, push_pop)
{
    container::RingBuffer<int> buffer(3);
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

TEST(test_ring_buffer, push_condition)
{
    const uint8_t capacity = 3;
    container::RingBuffer<int> buffer(3);
    buffer.push(100);
    buffer.push(41, [&capacity](const auto& container) { return container.size() == capacity; });
    buffer.push(42);

    EXPECT_EQ(buffer.size(), 2);
    EXPECT_EQ(buffer.pop(), 100);
    EXPECT_EQ(buffer.pop(), 42);
}

}
