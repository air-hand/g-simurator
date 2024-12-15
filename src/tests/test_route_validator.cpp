#include <gtest/gtest.h>

#include "proto/route.hpp"

import utils;

namespace
{

TEST(test_route_validator, validate)
{
    sim::route::RouteList list;
    list.set_name("test");
    list.set_window_name("window");

    auto* route = list.add_routes();
    route->set_expected("expected");
    auto* roi = route->mutable_roi();
    roi->set_top_percent(0);
    roi->set_left_percent(0);
    roi->set_width_percent(100);
    roi->set_height_percent(100);

    EXPECT_EQ(1, list.routes_size());

    EXPECT_FALSE(sim::utils::RouteValidator(list)());

    std::ranges::for_each(std::string("keys"), [&route](const char k) {
        route->add_keys(std::string(1, k));
    });
    EXPECT_EQ(4, route->keys_size());
    EXPECT_TRUE(sim::utils::RouteValidator(list)());
}

}
