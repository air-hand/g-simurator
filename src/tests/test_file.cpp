#include <stdexcept>
#include <gtest/gtest.h>
#include <sstream>

#include "../utils/file.hpp"

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

TEST(test_file, read_all_unique_ptr)
{
    std::unique_ptr<std::istringstream> ss(new std::istringstream("Hello, world!™"));
    const auto content = sim::utils::read_all(std::move(ss));
    EXPECT_EQ(content, "Hello, world!™");
}

}
