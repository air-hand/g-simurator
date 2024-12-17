#include "debug.hpp"

#ifdef DEBUG
#include "logger.hpp"

#include <dbghelp.h>

import std.compat;
import utils;

namespace sim::utils::debug
{
void AssertHandler(const char* expr, const char* file, const int line)
{
    constexpr DWORD CUSTOM_ASSERT_EXCEPTION = 0xE0000001;
    DEBUG_LOG_ARGS("Assertion failed: {} at {}:{}\n", expr, file, line);
    // MEMO: Custom Exception code
    RaiseException(CUSTOM_ASSERT_EXCEPTION, 0, 0, nullptr);
}

}

#endif
