#include <gtest/gtest.h>

import std;
import utils;

namespace
{

TEST(test_unicode, utf8_to_utf16)
{
    const auto utf8 = std::string("Hello, world!™");
    const auto utf16 = sim::utils::unicode::to_utf16(utf8);
    EXPECT_EQ(utf16, L"Hello, world!™");
}

TEST(test_unicode, utf8_to_utf16_empty)
{
    const auto utf8 = std::string();
    const auto utf16 = sim::utils::unicode::to_utf16(utf8);
    EXPECT_EQ(utf16, L"");
}

TEST(test_unicode, utf16_to_utf8)
{
    const auto utf16 = std::wstring(L"Hello, world!™");
    const auto utf8 = sim::utils::unicode::to_utf8(utf16);
    EXPECT_EQ(utf8, "Hello, world!™");
}

TEST(test_unicode, utf16_to_utf8_empty)
{
    const auto utf16 = std::wstring();
    const auto utf8 = sim::utils::unicode::to_utf8(utf16);
    EXPECT_EQ(utf8, "");
}

}
