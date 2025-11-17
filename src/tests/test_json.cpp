#include <gtest/gtest.h>

import std;
import utils;

namespace
{

TEST(test_json, remove_comments)
{
    const auto orig = std::string(
        R"(
        // comment
        {
            /* aiueo */
            "foo": "bar" // inline comment
        }
        )"
    );
    const auto removed = sim::utils::json::remove_json_comments(orig);
    EXPECT_FALSE(removed.contains("aiueo"));
    EXPECT_FALSE(removed.contains("// inline comment"));
}

}
