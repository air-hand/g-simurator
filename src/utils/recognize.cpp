module;

#include <tesseract/baseapi.h>
#include <opencv2/opencv.hpp>

module utils;

namespace
{

std::string foo(const cv::Mat& image)
{
    tesseract::TessBaseAPI tess;
    tess.SetImage(image.data, image.cols, image.rows, 1, image.step);
    const char* output = tess.GetUTF8Text();
    std::string out(output);
    delete[] output;
    return out;
}

}

namespace sim::utils::recognize
{
RecognizeText::RecognizeText()
{
    cv::Mat image;
    foo(image);
}
}
