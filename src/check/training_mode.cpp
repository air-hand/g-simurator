module;

#include <opencv2/opencv.hpp>

#include "utils/logger.hpp"

module check;

import std;
import utils;
import :training_mode;

namespace fs = std::filesystem;

namespace utils = sim::utils;
namespace training = utils::game::training;
namespace
{

using Error = sim::utils::Error;

using CapturedImageResult = sim::utils::Result<sim::utils::CapturedImage>;

std::generator<CapturedImageResult> ReadImagesFromDir(fs::path dir)
{
    DEBUG_LOG_SPAN(_);
    std::error_code ec;

    fs::recursive_directory_iterator itr{dir, ec};
    if (ec)
    {
        co_yield std::unexpected(Error::create("init directory iterator failed", ec));
        co_return;
    }

    for (const decltype(itr) end{}; itr != end; itr.increment(ec))
    {
        if (ec)
        {
            co_yield std::unexpected(Error::create("iterate directory failed", ec));
            co_return;
        }
        const auto& entry = *itr;
        if (!entry.is_regular_file())
        {
            continue;
        }
        co_yield utils::CapturedImage(entry.path());
    }
}

}

namespace sim::check
{

int RunTrainingSpanCheck()
{
    auto span_detector = training::ActionSpanDetector(
        training::BlackoutSpanBoundaryDetector(
            training::BlackoutDetectorConfig{.roi = cv::Rect(300, 200, 300, 200)}
        )
    );
    const auto path = fs::path("./tmp-test");
    for(auto&& result : ReadImagesFromDir(path))
    {
        if (!result)
        {
            const auto& error = result.error();
            utils::logging::log("error: {}", error.message());
            continue;
        }
        const auto span = span_detector.push(std::move(*result));
        if (!span)
        {
            continue;
        }
        utils::logging::log("span start");
        for(const auto& image : *span)
        {
            utils::logging::log("path: {}", image.Path().string());
        }
        utils::logging::log("span end");
    }
    return 0;
}

}
