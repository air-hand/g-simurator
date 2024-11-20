module;

#include <opencv2/opencv.hpp>

#include "macro.hpp"

export module utils:recognize;

import std;

namespace sim::utils::recognize
{

export class RecognizeText
{
public:
    ~RecognizeText();

    DELETE_COPY_AND_ASSIGN(RecognizeText);

//#if 0
    static RecognizeText& Get();

//    void Init();
//    void Finalize();

    std::string ImageToText(const cv::Mat& image);
    std::string ImageToText(const std::filesystem::path& path);
//#endif

private:
    RecognizeText();
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
