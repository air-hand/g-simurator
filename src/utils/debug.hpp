#pragma once

#ifdef DEBUG

#include <opencv2/opencv.hpp>

namespace sim::utils::debug
{
LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* e) noexcept;
void AssertHandler(const char* expr, const char* file, const int line);
int OpenCVErrorHandler(int status, const char* func_name, const char* err_msg, const char* file_name, int line, void* userdata);
}

#define DEBUG_SET_EXCEPTION_FILTER() \
    do \
    { \
        SetUnhandledExceptionFilter(sim::utils::debug::UnhandledExceptionFilter); \
        cv::redirectError(sim::utils::debug::OpenCVErrorHandler); \
    } while (false)

#define DEBUG_ASSERT(expr) \
    do \
    { \
        if (!(expr)) \
        { \
            sim::utils::debug::AssertHandler(#expr, __FILE__, __LINE__); \
        } \
    } while (false)

#else

#define DEBUG_SET_EXCEPTION_FILTER() do {} while (false)

#define DEBUG_ASSERT(expr) do {} while (false)

#endif
