#pragma once

#define OVERRIDE_NEW_DELETE 0
// override new and delete operators to track memory allocations (DEBUG build only)


#if OVERRIDE_NEW_DELETE

#ifdef DEBUG
#include <cstdlib>
#include <new>

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
#endif // DEBUG

#else

#define DEBUG_MEMORY_LEAK_TRACKING(x) do {} while (0)

#endif // OVERRIDE_NEW_DELETE


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
