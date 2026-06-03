#include <gtest/gtest.h>

import std;
import utils;

namespace
{

namespace unicode = sim::utils::unicode;

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
