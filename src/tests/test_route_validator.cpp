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
    list.add_routes()->set_expected("expected").mutable_roi()->set_top_percent(0).set_left_percent(0).set_width_percent(100).set_height_percent(100);

    const auto validator = sim::utils::RouteValidator(list);
    EXPECT_TRUE(validator.Validate());
}

}
