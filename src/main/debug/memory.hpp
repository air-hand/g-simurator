#pragma once

// override new and delete operators to track memory allocations (DEBUG build only)
#ifdef DEBUG

#include <cstdlib>
#include <new>

void reportMemoryLeaks() noexcept;

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

#define DEBUG_REPORT_MEMORY_LEAKS() do { \
    std::atexit(reportMemoryLeaks); \
    } while (0)

#else

#define DEBUG_REPORT_MEMORY_LEAKS() do {} while (0)

#endif // DEBUG
