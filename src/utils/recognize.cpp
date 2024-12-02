module;

#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>

#include "macro.hpp"
#include "logger.hpp"

module utils;

import std;
import :logger;
import :recognize;

namespace sim::utils::recognize
{

class RecognizeText::Impl
{
public:
    Impl() : tess_()
    {
        tess_.Init(nullptr, "jpn");
    }
    ~Impl()
    {
        tess_.End();
    }

    DELETE_COPY_AND_ASSIGN(Impl);

    std::string ImageToText(const cv::Mat& image)
    {
        tess_.Clear();
        tess_.SetImage(image.data, image.cols, image.rows, 1, image.step);
        const char* output = tess_.GetUTF8Text();
        std::string out(output);
        delete[] output;
        DEBUG_LOG_ARGS("Recognized: [{}]", out);
        return out;
    }
private:
    tesseract::TessBaseAPI tess_;
};

RecognizeText::RecognizeText() : impl_(std::make_unique<Impl>())
{
}

RecognizeText::~RecognizeText() = default;

RecognizeText& RecognizeText::Get()
{
    static RecognizeText inst;
    return inst;
}

std::string RecognizeText::ImageToText(const cv::Mat& image)
{
    return impl_->ImageToText(image);
}

std::string RecognizeText::ImageToText(const std::filesystem::path& path)
{
    DEBUG_LOG_SPAN(_);
    const auto image = cv::imread(path.string());
    cv::Mat optimized = image(cv::Rect(0, 0, 400, 200));
//    cv::Mat optimized = image(cv::Rect(0, 0, 2560, 1440));
    cv::cvtColor(optimized, optimized, cv::COLOR_BGR2GRAY);
    cv::threshold(optimized, optimized, 128, 255, cv::THRESH_BINARY);
    const auto output = path.parent_path() / (path.stem().string() + "_optimized" + path.extension().string());
    DEBUG_LOG_ARGS("Output to {}", output.string());
//    cv::resize(image, optimized, cv::Size(), 0.5, 0.5); // resize 50%
    cv::imwrite(output.string(), optimized);
    return ImageToText(optimized);
}

}
