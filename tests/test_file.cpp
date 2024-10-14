#include <stdexcept>
#include <gtest/gtest.h>
#include <sstream>

// FIXME: split src/utils as library first.
#include "../src/utils/file.hpp"

namespace
{

TEST(test_file, read_all_string)
{
    std::basic_istringstream<char> ss("Hello, world!™");
    const auto content = sim::utils::read_all(&ss);
    EXPECT_EQ(content, "Hello, world!™");
}

TEST(test_file, read_all_u8string)
{
    std::basic_istringstream<char8_t> ss(u8"Hello, world!™");
    const auto content = sim::utils::read_all(&ss);
    EXPECT_EQ(content, u8"Hello, world!™");
}

}
