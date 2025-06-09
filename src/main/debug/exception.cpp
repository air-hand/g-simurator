#include "exception.hpp"

#ifdef DEBUG
#include "utils/debug.hpp"
#include "utils/logger.hpp"

#include <boost/stacktrace.hpp>
#include <dbghelp.h>

import std.compat;
import utils;

namespace
{
bool WriteMiniDump(EXCEPTION_POINTERS* e)
{
    const auto now = std::chrono::system_clock::now();
    const auto filename = sim::utils::strings::fmt(L"./tmp/crash_{:%Y%m%d%H%M%S}.dmp", std::chrono::time_point_cast<std::chrono::milliseconds>(now));
    const auto file = CreateFileW(filename.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    MINIDUMP_EXCEPTION_INFORMATION info;
    info.ThreadId = GetCurrentThreadId();
    info.ExceptionPointers = e;
    info.ClientPointers = TRUE;

    const auto result = MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        file,
        MiniDumpWithDataSegs,
        e ? &info : nullptr,
        nullptr,
        nullptr
    );
    CloseHandle(file);
    return result == TRUE;
}

}

namespace sim::debug
{
LONG CALLBACK VectoredExceptionHandler(PEXCEPTION_POINTERS e) noexcept
{
    DEBUG_LOG_SPAN(_);
    DEBUG_LOG_ARGS("Exception code: 0x{:X}", e->ExceptionRecord->ExceptionCode);
    DEBUG_LOG_ARGS("Stack trace:\n{}\n", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
    return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* e) noexcept
{
    DEBUG_LOG_SPAN(_);
    if (!WriteMiniDump(e))
    {
        DEBUG_LOG("Failed to write minidump.");
    }
    DEBUG_LOG_ARGS("Exception code: 0x{:X}", e->ExceptionRecord->ExceptionCode);
    DEBUG_LOG_ARGS("Stack trace:\n{}\n", boost::stacktrace::to_string(boost::stacktrace::stacktrace()));
    return EXCEPTION_EXECUTE_HANDLER;
}

int OpenCVErrorHandler(int status, [[maybe_unused]] const char* func_name, const char* err_msg, const char* file_name, int line, [[maybe_unused]] void* userdata)
{
    DEBUG_LOG_ARGS("OpenCV Error: {} {} at {}:{}\n", status, err_msg, file_name, line);
    DEBUG_ASSERT(false);
    return 0;
}

}

#endif
