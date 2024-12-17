#include "debug/exception.hpp"
#include "debug/memory.hpp"

import sim;

int main(int argc, char** argv)
{
    DEBUG_REPORT_MEMORY_LEAKS();
    DEBUG_SET_EXCEPTION_HANDLER();

    sim::MainProc main_proc(argc, argv);
    return main_proc.Run();
}
