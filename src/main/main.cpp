#include "utils/debug.hpp"

import std.compat;
import sim;

uint32_t main(uint32_t argc, char** argv)
{
    DEBUG_SET_EXCEPTION_HANDLER();

    sim::MainProc main_proc(argc, argv);
    return main_proc.Run();
}
