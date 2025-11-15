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
import :file;

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
        if (r.is_line_start && !out.empty())
        {
            out += "\n";
        }
        out += r.text + " ";
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
            15,
            cv::Scalar(255, 0, 0),
            1,
            cv::LINE_AA,
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
        DEBUG_LOG_SPAN(_);
        // returns 0:success
//        const auto result = tess_.Init(nullptr, "jpn", tesseract::OEM_LSTM_ONLY);
        const auto result = tess_.Init(nullptr, "jpn");
        if (result != 0) {
            logging::log("Failed to init tesseract.");
            DEBUG_ASSERT(false);
            return false;
        }

        // ========================================
        // 高速化のための設定
        // ========================================
#if 0
        // OEM: LSTM only (デフォルトは3=両方、1=LSTM onlyで高速化)
        tess_.SetVariable("tessedit_ocr_engine_mode", "1");

        // PSM: スパースな日本語テキストを想定（ゲームUI向け）
        tess_.SetPageSegMode(tesseract::PSM_SPARSE_TEXT);

        // 並列化を有効化（マルチスレッド処理）
        tess_.SetVariable("tessedit_parallelize", "1");

        // 不要な辞書の読み込みを無効化（日本語では英語の辞書は不要）
        tess_.SetVariable("load_system_dawg", "0");
        tess_.SetVariable("load_freq_dawg", "0");
        tess_.SetVariable("load_punc_dawg", "0");
        tess_.SetVariable("load_number_dawg", "0");
        tess_.SetVariable("load_bigram_dawg", "0");

        // 適応学習を無効化（高速化）
        tess_.SetVariable("classify_enable_learning", "0");
        tess_.SetVariable("classify_enable_adaptive_matcher", "0");

        // ビーム探索の制限（速度優先）
        tess_.SetVariable("lstm_use_matrix", "1"); // 1のままでOK
        tess_.SetVariable("lstm_choice_iterations", "1"); // デフォルトは5、1に削減

        // デバッグ出力を完全無効化
        tess_.SetVariable("debug_file", "/dev/null");
        tess_.SetVariable("classify_debug_level", "0");
        tess_.SetVariable("matcher_debug_level", "0");
#endif

#ifdef DEBUG
        {
            const auto fp = sim::utils::open_file("tess_params.txt", "wb");
            if (fp) {
                tess_.PrintVariables(fp.get());
            }
        }
#endif
        return true;
    }

    DELETE_COPY_AND_ASSIGN(Impl);

    std::vector<RecognizeResults::Result> RecognizeImage(const cv::Mat& image, float border)
    {
        TessBaseAPIWrapper tess(tess_);
        {
            DEBUG_LOG_SPAN(set_image);
            tess.Get().SetImage(image.data, image.cols, image.rows, image.elemSize(), image.step);
        }
        {
            DEBUG_LOG_SPAN(recognize);
            if (tess.Get().Recognize(nullptr) != 0)
            {
                DEBUG_ASSERT(false);
                return {};
            }
        }

        std::vector<RecognizeResults::Result> results;
        std::unique_ptr<tesseract::ResultIterator> it(tess.Get().GetIterator());
        const auto level = tesseract::RIL_WORD;
        if (it == nullptr)
        {
            return results;
        }
        {
            DEBUG_LOG_SPAN(get_texts);
            do
            {
                decltype(results)::value_type result;

                result.confidence = it->Confidence(level);
                if (result.confidence < border)
                {
                    continue;
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

                result.is_line_start = it->IsAtBeginningOf(tesseract::RIL_TEXTLINE);

                results.emplace_back(result);
            } while (it->Next(level));
        }
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
