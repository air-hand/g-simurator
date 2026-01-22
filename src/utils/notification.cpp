module;

#include "logger.hpp"

module utils;

import :logger;

namespace sim::utils::notification
{
namespace {
void beep_impl(UINT type, const char* name)
{
    DEBUG_LOG_SPAN(_);
    if (!MessageBeep(type))
    {
        DEBUG_LOG_ARGS("MessageBeep({}) failed: {}", name, GetLastError());
    }
}
}

void beep_success() { beep_impl(MB_ICONINFORMATION, "success"); }
void beep_error()   { beep_impl(MB_ICONERROR, "error"); }
void beep_warning() { beep_impl(MB_ICONWARNING, "warning"); }
}
