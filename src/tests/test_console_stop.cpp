#include <gtest/gtest.h>

import std;
import utils;

namespace
{

TEST(test_console_stop, request_stop_updates_token)
{
    auto handler = sim::utils::ConsoleStopHandler {};
    const auto token = handler.get_token();

    EXPECT_FALSE(token.stop_requested());
    EXPECT_TRUE(handler.request_stop());
    EXPECT_TRUE(token.stop_requested());
    EXPECT_TRUE(handler.stop_requested());
}

TEST(test_console_stop, second_handler_does_not_share_stop_state)
{
    auto handler = sim::utils::ConsoleStopHandler {};
    auto second_handler = sim::utils::ConsoleStopHandler {};
    const auto token = second_handler.get_token();

    EXPECT_TRUE(handler.request_stop());
    EXPECT_TRUE(handler.stop_requested());
    EXPECT_FALSE(token.stop_requested());
}

}
