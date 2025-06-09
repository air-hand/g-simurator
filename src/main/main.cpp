#include "debug/memory.hpp"
#include "debug/exception.hpp"

import sim;
import utils;

namespace 
{
std::unique_ptr<sim::MainProc> main_proc = nullptr;

BOOL WINAPI ConsoleHandler(DWORD dwCtrlType) noexcept
{
    switch(dwCtrlType)
    {
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            if (main_proc != nullptr)
            {
                main_proc->Cancel();
            }
            return TRUE;
        default:
            return FALSE;
    }
}
}

int main(int argc, char** argv)
{
    DEBUG_SET_EXCEPTION_HANDLER();

    sim::utils::logging::init();

    DEBUG_MEMORY_LEAK_TRACKING(_);

    ::SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    main_proc = std::make_unique<sim::MainProc>(argc, argv);
    const auto exit_code = main_proc->Run();

    ::SetConsoleCtrlHandler(ConsoleHandler, FALSE);
    main_proc.reset();

    return exit_code;
}
