#include <gtest/gtest.h>

import std;
import utils;

namespace
{

TEST(test_fmt, no_args)
{
    const auto result = sim::utils::strings::fmt("Hello, world!™");
    EXPECT_EQ(result, "Hello, world!™");
}

TEST(test_fmt, with_args)
{
    const auto result = sim::utils::strings::fmt("Hello, {}!", "World");
    EXPECT_EQ(result, "Hello, World!");
}

TEST(test_fmt, with_args_wide)
{
    const auto result = sim::utils::strings::fmt(L"Hello, {}!", L"World");
    EXPECT_EQ(result, L"Hello, World!");
}

}
