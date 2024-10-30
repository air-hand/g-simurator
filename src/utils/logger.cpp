module;

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>

module utils;

namespace
{
plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
}

namespace sim::utils::logging
{

void init()
{
    plog::init(plog::debug, &consoleAppender);
}

void log(const std::string& message)
{
    PLOG_DEBUG << message;
}

void log(const std::wstring& message)
{
    PLOG_DEBUG << message;
}

}
