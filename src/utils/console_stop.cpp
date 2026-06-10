module;

#include "std/windows.hpp"
#include "logger.hpp"
#include "macro.hpp"

module utils;

import std.compat;
import :console_stop;
import :logger;

namespace
{

std::mutex g_handler_mutex;
std::stop_source* g_stop_source = nullptr;

BOOL WINAPI ConsoleHandler(DWORD dwCtrlType) noexcept
{
    switch(dwCtrlType)
    {
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
        {
            const std::lock_guard lock(g_handler_mutex);
            if (g_stop_source != nullptr)
            {
                g_stop_source->request_stop();
            }
            return TRUE;
        }
        default:
            return FALSE;
    }
}

}

namespace sim::utils
{

class ConsoleStopHandler::Impl final
{
public:
    Impl()
    {
        const std::lock_guard lock(g_handler_mutex);
        if (g_stop_source != nullptr)
        {
            sim::utils::logging::log("Console stop handler is already registered.");
            return;
        }

        g_stop_source = &stop_source_;
        registered_ = ::SetConsoleCtrlHandler(ConsoleHandler, TRUE) != FALSE;
        if (!registered_)
        {
            sim::utils::logging::log("Failed to register console stop handler. error={}", ::GetLastError());
            g_stop_source = nullptr;
        }
    }

    ~Impl()
    {
        if (registered_)
        {
            const std::lock_guard lock(g_handler_mutex);
            ::SetConsoleCtrlHandler(ConsoleHandler, FALSE);
            if (g_stop_source == &stop_source_)
            {
                g_stop_source = nullptr;
            }
        }
    }

    DELETE_COPY_AND_ASSIGN(Impl);
    DELETE_MOVE_AND_ASSIGN(Impl);

    std::stop_token get_token() const noexcept
    {
        return stop_source_.get_token();
    }

    bool stop_requested() const noexcept
    {
        return stop_source_.stop_requested();
    }

    bool request_stop() noexcept
    {
        return stop_source_.request_stop();
    }

private:
    std::stop_source stop_source_;
    bool registered_ = false;
};

ConsoleStopHandler::ConsoleStopHandler()
    : impl_(std::make_unique<Impl>())
{
}

ConsoleStopHandler::~ConsoleStopHandler() = default;

std::stop_token ConsoleStopHandler::get_token() const noexcept
{
    return impl_->get_token();
}

bool ConsoleStopHandler::stop_requested() const noexcept
{
    return impl_->stop_requested();
}

bool ConsoleStopHandler::request_stop() noexcept
{
    return impl_->request_stop();
}

}
