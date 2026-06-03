#include <gtest/gtest.h>

import std;
import utils;

namespace
{

namespace utils = sim::utils;
namespace training = utils::game::training;

TEST(test_action_span_detector, found_some_content)
{
    auto detector_stub = [](const utils::CapturedImage& img) -> bool {
        return img.Path().string().contains("bound");
    };

    auto span_detector = training::ActionSpanDetector(detector_stub);
    EXPECT_FALSE(span_detector.push(utils::CapturedImage("path/to/file1")));
    EXPECT_FALSE(span_detector.push(utils::CapturedImage("path/to/file2")));
    EXPECT_FALSE(span_detector.push(utils::CapturedImage("path/to/file3/this_is_first_bound")));
    EXPECT_FALSE(span_detector.push(utils::CapturedImage("path/to/file4/this_is_first_content")));
    EXPECT_FALSE(span_detector.push(utils::CapturedImage("path/to/file5/this_is_second_content")));

    const auto result = span_detector.push(utils::CapturedImage("path/to/file6/this_is_second_bound"));
    ASSERT_TRUE(result);
    const auto& span = *result;
    EXPECT_EQ(span.size(), 2);
    EXPECT_EQ(span[0].Path(), "path/to/file4/this_is_first_content");
    EXPECT_EQ(span[1].Path(), "path/to/file5/this_is_second_content");
}

}
