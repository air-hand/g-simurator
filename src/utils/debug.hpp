#pragma once

#ifdef DEBUG

//#include <opencv2/opencv.hpp>

namespace sim::utils::debug
{
void AssertHandler(const char* expr, const char* file, const int line);
}

#define DEBUG_ASSERT(expr) \
    do \
    { \
        if (!(expr)) \
        { \
            sim::utils::debug::AssertHandler(#expr, __FILE__, __LINE__); \
        } \
    } while (false)

#else

#define DEBUG_ASSERT(expr) do {} while (false)

#endif
