#pragma once

#define OVERRIDE_NEW_DELETE 1
// override new and delete operators to track memory allocations (DEBUG build only)

#if OVERRIDE_NEW_DELETE
#include <cstdlib>
#include <new>
#endif

#ifdef DEBUG

#if !OVERRIDE_NEW_DELETE
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
//#define new ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define DEBUG_REPORT_MEMORY_LEAKS(x) \
    do { \
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); \
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE); \
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR); \
        /*_CrtDumpMemoryLeaks()*/; \
    } while (false)
#else
#include "utils/macro.hpp"
namespace sim::debug
{

class MemoryTrackerSpan final
{
public:
    MemoryTrackerSpan();
    ~MemoryTrackerSpan();
    DELETE_COPY_AND_ASSIGN(MemoryTrackerSpan);
};
}

#define DEBUG_MEMORY_LEAK_TRACKING(x) sim::debug::MemoryTrackerSpan x
#endif // OVERRIDE_NEW_DELETE

#else

#define DEBUG_MEMORY_LEAK_TRACKING(x) do {} while (0)

#endif // DEBUG

#if OVERRIDE_NEW_DELETE
#pragma warning(push)
#pragma warning(disable: 28251) // Inconsistent annotation for 'new': this instance has no annotations.
void* operator new(std::size_t size);

void operator delete(void* ptr) noexcept;

void operator delete(void* ptr, std::size_t) noexcept;

void* operator new[](std::size_t size);

void operator delete[](void* ptr) noexcept;

void operator delete[](void* ptr, std::size_t) noexcept;

void* operator new(std::size_t size, const std::nothrow_t&) noexcept;

void* operator new(std::size_t size, const std::align_val_t align);

void operator delete(void* ptr, const std::align_val_t) noexcept;

#pragma warning(pop)
#endif
