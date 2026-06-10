module;

#include <opencv2/opencv.hpp>

#include "utils/logger.hpp"
#include "utils/macro.hpp"

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
        if (entry.path().filename() != fs::path("capture_target.png"))
        {
            continue;
        }
        co_yield utils::CapturedImage(entry.path());
    }
}

}

namespace sim::check
{

namespace
{
// adjust if recognize too long
constexpr std::size_t MaxOcrImagesPerSpan = 10000;
constexpr std::size_t MaxOcrSpans = 100;

template<typename T> concept has_finalize = requires(T& val)
{
    { val.Finalize() };
};
template<has_finalize T>
class Finalizer final
{
public:
    explicit Finalizer(T& val) : val_(val) {};
    ~Finalizer() {
        val_.Finalize();
    }
    DELETE_COPY_AND_ASSIGN(Finalizer);
private:
    T& val_;
};
}

int RunTrainingSpanCheck()
{
    utils::ConsoleStopHandler stop{};
    auto stop_token = stop.get_token();
    auto& recognizer = utils::recognize::RecognizeText::Get();
    if (!recognizer.Init())
    {
        utils::logging::log("error: failed to init");
        return 1;
    }
    [[maybe_unused]] const Finalizer finalizer{recognizer};
    auto span_detector = training::ActionSpanDetector(
        training::BlackoutSpanBoundaryDetector(
            training::BlackoutDetectorConfig{.roi = cv::Rect(300, 200, 300, 200)}
        )
    );
    const auto path = fs::path("./tmp-test");
    std::size_t ocr_spans = 0;
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
        if (ocr_spans >= MaxOcrSpans)
        {
            utils::logging::log("skip span: max ocr spans reached");
            continue;
        }
        ++ocr_spans;
        const auto span_size = span->size();
        const auto step = std::max<std::size_t>(span_size / MaxOcrImagesPerSpan, 1);
        utils::logging::log("span start: size={}, ocr_step={}", span_size, step);
        for(std::size_t i = 0; i < span_size; i += step)
        {
            if (stop_token.stop_requested())
            {
                utils::logging::log("Stopped");
                return 2;
            }
            const auto& image = (*span)[i];
            const auto image_path = image.Path();
            utils::logging::log("path: {}, index={}", fs::absolute(image_path).string(), i);
            const auto mat = utils::image::crop(image.Read(), utils::image::RelativeRect{
                .x = 0.05,
                .y = 0.5,
                .width = 0.2,
                .height = 0.4
            }); // 技データ表示UI
            const auto start = std::chrono::steady_clock::now();
            const auto results = recognizer.RecognizeImage(mat, 70.0f);
            const auto end = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            utils::logging::log("Recognized: elapsed={}ms [{}]", elapsed.count(), results.ToString());
            const auto output_dir = image_path.parent_path();
            sim::utils::image::saveImage(results.DrawRects(), (output_dir / "recognized.png").string());
        }
        utils::logging::log("span end");
    }
    return 0;
}

}
