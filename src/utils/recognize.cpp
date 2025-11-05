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
        freetype->loadFontData("C:/Windows/Fonts/meiryo.ttc", 0);
    }
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
    for (const auto& r : results_)
    {
        DEBUG_LOG_ARGS("Text: {}, Confidence: {}", r.text, r.confidence);
        out += " " + r.text;
    }
    return out;
}

cv::Mat RecognizeResults::DrawRects() const
{
    cv::Mat out;
    input_.copyTo(out);

    const auto font = LoadFontData();

    for (const auto& r : results_)
    {
        cv::rectangle(out, r.rect, cv::Scalar(0, 0, 255), 2);

        const auto text_with_confidence = std::format("{}:[{:.2f}]", r.text, r.confidence);
        font->putText(
            out,
            text_with_confidence,
            cv::Point(r.rect.x, r.rect.y - 10),
            10,
            cv::Scalar(255, 0, 0),
            1,
            cv::LINE_AA,
            //true
            false
        );
    }
    return out;
}

namespace
{
class TessBaseAPIWrapper final
{
private:
    tesseract::TessBaseAPI& tess_;
public:
    TessBaseAPIWrapper(tesseract::TessBaseAPI& tess) : tess_(tess)
    {
    }
    ~TessBaseAPIWrapper()
    {
        tess_.Clear();
    }

    DELETE_COPY_AND_ASSIGN(TessBaseAPIWrapper);

    auto& Get() noexcept
    {
        return tess_;
    }
};

}

class RecognizeText::Impl
{
public:
    Impl() : tess_()
    {
    }
    ~Impl()
    {
        DEBUG_LOG_SPAN(_);
        tess_.End();
        tesseract::TessBaseAPI::ClearPersistentCache();
    }

    bool Init()
    {
        // returns 0:success
        const auto result = tess_.Init(nullptr, "jpn");
        if (result != 0) {
            logging::log("Failed to init tesseract.");
            DEBUG_ASSERT(false);
            return false;
        }
        return true;
    }

    DELETE_COPY_AND_ASSIGN(Impl);

    std::vector<RecognizeResults::Result> RecognizeImage(const cv::Mat& image, float border)
    {
        TessBaseAPIWrapper tess(tess_);
        tess.Get().SetImage(image.data, image.cols, image.rows, 1, image.step);
        {
            DEBUG_ASSERT(tess.Get().Recognize(nullptr) == 0);
        }

        std::vector<RecognizeResults::Result> results;
        std::unique_ptr<tesseract::ResultIterator> it(tess.Get().GetIterator());
        const auto level = tesseract::RIL_WORD; // Recognize level word
        if (it == nullptr)
        {
            return results;
        }
        do
        {
            decltype(results)::value_type result;

            result.confidence = it->Confidence(level);
            if (result.confidence < border)
            {
//                continue;
            }

            std::unique_ptr<const char[]> output(it->GetUTF8Text(level));
            if (output == nullptr)
            {
                continue;
            }
            result.text = output.get();

            int x1, y1, x2, y2;
            it->BoundingBox(level, &x1, &y1, &x2, &y2);
            result.rect = cv::Rect(x1, y1, x2 - x1, y2 - y1);

            results.emplace_back(result);
        } while (it->Next(level));
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

bool RecognizeText::Init()
{
    return impl_->Init();
}

void RecognizeText::Finalize()
{
    impl_.reset();
}

RecognizeResults RecognizeText::RecognizeImage(const cv::Mat& image, float border)
{
    DEBUG_LOG_SPAN(_);
    const auto results = impl_->RecognizeImage(image, border);
    return RecognizeResults(image, results);
}

}
