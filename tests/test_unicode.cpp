#if false
#include <stdexcept>
#include <gtest/gtest.h>

// FIXME: split src/utils as library first.
#include "../src/utils/unicode.hpp"

namespace
{

TEST(test_unicode, utf8_to_utf16)
{
    const std::string utf8 = "Hello, world!™";
    const std::wstring utf16 = sim::utils::unicode::to_utf16(utf8);
    EXPECT_EQ(utf16, L"Hello, world!™");
}

}
#endif
