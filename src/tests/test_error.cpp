#include <gtest/gtest.h>
#include <gmock/gmock.h>

import std;
import utils;

namespace
{
TEST(test_error, occur_error)
{
    const auto unexpected = std::unexpected(sim::utils::Error::create("some error"));
    EXPECT_THAT(unexpected.error().str(), testing::HasSubstr("some error"));
}

TEST(test_error, has_child_error)
{
    auto child = sim::utils::Error::create("this is child");
    const auto parent = sim::utils::Error::create("parent", std::move(child));
    EXPECT_THAT(parent.str(), testing::HasSubstr("this is child"));
}

TEST(test_error, use_result)
{
    auto ok_or_ng = [](bool is_error) -> sim::utils::Result<int> {
        if (is_error) {
            return std::unexpected(sim::utils::Error::create("error"));
        }
        return 100;
    };

    {
        const auto ng = ok_or_ng(true);
        ASSERT_FALSE(ng);
        EXPECT_THAT(ng.error().str(), testing::HasSubstr("error"));
    }
    {
        const auto ok = ok_or_ng(false);
        ASSERT_TRUE(ok);
        EXPECT_EQ(*ok, 100);
    }
}
}
