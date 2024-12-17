#ifndef _MAIN_MEMORY_HPP_
#define _MAIN_MEMORY_HPP_

#include <boost/stacktrace.hpp>

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
            std::cerr << "No memory leaks" << std::endl;
            return;
        }
        std::cerr << "Memory leaks detected:" << std::endl;
        for (const auto& [ptr, info] : allocations_)
        {
            std::cerr << "Memory leak: " << info.size << " bytes" << std::endl;
            std::cerr << info.trace << std::endl;
        }
    }
private:
    static inline std::unordered_map<void*, AllocationInfo> allocations_;
    static inline std::mutex mutex_;
};

// override new and delete operators to track memory allocations (DEBUG build only)
#ifdef DEBUG

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

#endif
