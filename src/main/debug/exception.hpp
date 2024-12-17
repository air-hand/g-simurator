#pragma once

#ifdef DEBUG

#include <opencv2/opencv.hpp>

namespace sim::debug
{
LONG CALLBACK VectoredExceptionHandler(PEXCEPTION_POINTERS e) noexcept;
LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* e) noexcept;
int OpenCVErrorHandler(int status, const char* func_name, const char* err_msg, const char* file_name, int line, void* userdata);
}

#define DEBUG_SET_EXCEPTION_HANDLER() \
    do \
    { \
        AddVectoredExceptionHandler(1, sim::debug::VectoredExceptionHandler); \
        SetUnhandledExceptionFilter(sim::debug::UnhandledExceptionFilter); \
        cv::redirectError(sim::debug::OpenCVErrorHandler); \
    } while (false)

#else

#define DEBUG_SET_EXCEPTION_HANDLER() do {} while (false)

#endif
