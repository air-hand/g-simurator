module;

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/TxtFormatter.h>

module utils;

using ch = std::chrono;

namespace
{
plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
plog::RollingFileAppender<plog::TxtFormatter> fileAppender("./logs/app.log", 1024 * 1024, 3);
}

namespace sim::utils::logging
{

void init()
{
    plog::init(plog::debug, &consoleAppender).addAppender(&fileAppender);
}

void log(const std::string& message)
{
    PLOG_DEBUG << message;
}

void log(const std::wstring& message)
{
    PLOG_DEBUG << message;
}

LogSpan::LogSpan(const std::string& message) noexcept : message_(message), start_(ch::steady_clock::now())
{
    PLOG_DEBUG << message_ << " [BEGIN]";
}

LogSpan::~LogSpan()
{
    const auto end = ch::steady_clock::now();
    const auto elapsed_ms = ch::duration_cast<ch::milliseconds>(end - start_);
    PLOG_DEBUG << message_ << " [END] " << elapsed_ms.cpumt() << "(ms)";
}

}
