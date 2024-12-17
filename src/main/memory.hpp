#pragma once

// override new and delete operators to track memory allocations (DEBUG build only)
#ifdef DEBUG

#include <boost/stacktrace.hpp>
#include "utils/logger.hpp"

import std;

class MemoryTracker final
{
public:
    struct AllocationInfo
    {
        std::size_t size;
        boost::stacktrace::stacktrace trace;
    };

    static void Alloc(void* ptr, std::size_t size)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_[ptr] = {size, boost::stacktrace::stacktrace()};
    }
    static void Free(void* ptr)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.erase(ptr);
    }
    static void ReportLeaks()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (allocations_.empty())
        {
            DEBUG_LOG("No memory leaks");
            return;
        }
        DEBUG_LOG("Memory leaks detected:");
        for (const auto& [ptr, info] : allocations_)
        {
            DEBUG_LOG_ARGS("Memory leak: {} bytes", info.size);
            DEBUG_LOG_ARGS("Stack trace:\n{}", boost::stacktrace::to_string(info.trace));
        }
    }
private:
    static inline std::unordered_map<void*, AllocationInfo> allocations_;
    static inline std::mutex mutex_;
};


void* operator new(std::size_t size)
{
    void* ptr = std::malloc(size);
    if (ptr == nullptr)
    {
        throw std::bad_alloc();
    }
    MemoryTracker::Alloc(ptr, size);
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    MemoryTracker::Free(ptr);
    std::free(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept
{
    MemoryTracker::Free(ptr);
    std::free(ptr);
}

void* operator new[](std::size_t size)
{
    void* ptr = std::malloc(size);
    if (ptr == nullptr)
    {
        throw std::bad_alloc();
    }
    MemoryTracker::Alloc(ptr, size);
    return ptr;
}

void operator delete[](void* ptr) noexcept
{
    MemoryTracker::Free(ptr);
    std::free(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept
{
    MemoryTracker::Free(ptr);
    std::free(ptr);
}

#define DEBUG_REPORT_MEMORY_LEAKS() do { \
    std::atexit(MemoryTracker::ReportLeaks); \
    } while (0)

#else

#define DEBUG_REPORT_MEMORY_LEAKS() do {} while (0)

#endif // DEBUG
