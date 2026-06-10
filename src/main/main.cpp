#include "debug/memory.hpp"
#include "debug/exception.hpp"

import std;
import sim;
import utils;

int main(int argc, char** argv)
{
    DEBUG_SET_EXCEPTION_HANDLER();

    sim::utils::logging::init();

    DEBUG_MEMORY_LEAK_TRACKING(_);

    auto console_stop_handler = sim::utils::ConsoleStopHandler {};
    auto main_proc = sim::MainProc(argc, argv, console_stop_handler.get_token());
    const auto exit_code = main_proc.Run();

    return exit_code;
}
