#include "main_proc.hpp"

int main(int argc, char** argv)
{
    sim::MainProc main_proc(argc, argv);
    return main_proc.Run();
}
