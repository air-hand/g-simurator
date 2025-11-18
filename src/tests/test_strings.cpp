#include <gtest/gtest.h>

import std;
import utils;

namespace
{

namespace unicode = sim::utils::unicode;

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

TEST(test_remove_all_whitespaces, single_line)
{
    const auto result = sim::utils::strings::remove_all_whitespaces(unicode::to_utf8(L" aiu え お"));
    EXPECT_EQ(result, unicode::to_utf8(L"aiuえお"));
}

TEST(test_remove_all_whitespaces, multi_line)
{
    const auto result = sim::utils::strings::remove_all_whitespaces(unicode::to_utf8(LR"(
a i
u え 
 お
)"));
    EXPECT_EQ(result, unicode::to_utf8(LR"(
ai
uえ
お
)"));
}

}
