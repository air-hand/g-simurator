module;

#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>
#include <tesseract/baseapi.h>

#include "macro.hpp"
#include "logger.hpp"
#include "debug.hpp"

module utils;

import std;
import :logger;
import :recognize;

namespace
{

cv::Ptr<cv::freetype::FreeType2> freetype = nullptr;

auto LoadFontData()
{
    if (freetype == nullptr)
    {
        freetype = cv::freetype::createFreeType2();
    }
    freetype->loadFontData("C:/Windows/Fonts/meiryo.ttc", 0);
    return freetype;
}

}

namespace sim::utils::recognize
{

RecognizeResults::RecognizeResults(const cv::Mat& input, const std::vector<Result>& results) : input_(input), results_(results)
{
}

std::string RecognizeResults::ToString() const
{
    std::string out;
    std::for_each(results_.begin(), results_.end(), [&out](const auto& r) {
        DEBUG_LOG_ARGS("Text: {}, Confidence: {}", r.text, r.confidence);
        out += " " + r.text;
    });
    return out;
}

cv::Mat RecognizeResults::DrawRects() const
{
    cv::Mat out;
    input_.copyTo(out);

    for (const auto& r : results_)
    {
        cv::rectangle(out, r.rect, cv::Scalar(0, 0, 255), 2);

        const auto font = LoadFontData();

        const auto text_with_confidence = std::format("{}:[{:.2f}]", r.text, r.confidence);
        font->putText(
            out,
            text_with_confidence,
            cv::Point(r.rect.x, r.rect.y - 10),
            10,
            cv::Scalar(0, 255, 0),
            1,
            cv::LINE_AA,
            true
        );
    }
    return out;
}

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

    std::vector<RecognizeResults::Result> RecognizeImage(const cv::Mat& image)
    {
        tess_.Clear();
//        tess_.SetImage(image.data, image.cols, image.rows, image.channels(), image.step);
        tess_.SetImage(image.data, image.cols, image.rows, 1, image.step);

        {
            const auto result = tess_.Recognize(nullptr);
            DEBUG_ASSERT(result == 0);
        }

        std::vector<RecognizeResults::Result> results;
        auto* it = tess_.GetIterator();
        const auto level = tesseract::RIL_WORD; // Recognize level word
        if (it == nullptr)
        {
            return results;
        }
        do
        {
            decltype(results)::value_type result;

            const char* output = it->GetUTF8Text(level);
            result.text = output;
            delete[] output;

            int x1, y1, x2, y2;
            it->BoundingBox(level, &x1, &y1, &x2, &y2);
            result.rect = cv::Rect(x1, y1, x2 - x1, y2 - y1);

            result.confidence = it->Confidence(level);

            results.emplace_back(result);
        } while (it->Next(level));
        // TODO: 解放忘れを防ぎたい...
        delete it;
        return results;
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

RecognizeResults RecognizeText::RecognizeImage(const cv::Mat& image)
{
    DEBUG_LOG_SPAN(_);
    const auto results = impl_->RecognizeImage(image);
    return RecognizeResults(image, results);
}

}
