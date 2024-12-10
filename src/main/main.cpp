import std.compat;
import sim;

uint32_t main(uint32_t argc, char** argv)
{
    sim::MainProc main_proc(argc, argv);
    return main_proc.Run();
}
