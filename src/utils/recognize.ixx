module;

#include <opencv2/opencv.hpp>

#include "macro.hpp"

export module utils:recognize;

import std;

namespace sim::utils::recognize
{

export class RecognizeResults final
{
public:
    struct Result
    {
        std::string text;
        cv::Rect rect;
        float confidence;
    };

    RecognizeResults(const cv::Mat& input, const std::vector<Result>& results);
    ~RecognizeResults() = default;

    DELETE_COPY_AND_ASSIGN(RecognizeResults);

    std::string ToString() const;
    cv::Mat DrawRects() const;
private:
    const cv::Mat& input_;
    std::vector<Result> results_;
};

// image.ixx に統合してもよさそう
export class RecognizeText final
{
public:
    ~RecognizeText();

    DELETE_COPY_AND_ASSIGN(RecognizeText);

    static RecognizeText& Get();

    bool Init();
    void Finalize();

    RecognizeResults RecognizeImage(const cv::Mat& image, float border = -1.0f);
private:
    RecognizeText();
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
