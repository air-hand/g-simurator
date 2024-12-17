#include "memory.hpp"
#ifdef DEBUG

#include <boost/stacktrace.hpp>
#include "utils/logger.hpp"

import std.compat;
import utils;

namespace
{
class MemoryTracker final
{
public:
    struct AllocationInfo
    {
        std::size_t size;
        boost::stacktrace::stacktrace trace;
    };

    static void Alloc(void* ptr, std::size_t size);
    static void Free(void* ptr);
    static void ReportLeaks() noexcept;
private:
    static inline std::unordered_map<void*, AllocationInfo> allocations_;
    static inline std::mutex mutex_;
};

void MemoryTracker::Alloc(void* ptr, std::size_t size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    allocations_[ptr] = {size, boost::stacktrace::stacktrace()};
}

void MemoryTracker::Free(void* ptr)
{
    std::lock_guard<std::mutex> lock(mutex_);
    allocations_.erase(ptr);
}

void MemoryTracker::ReportLeaks() noexcept
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

}

void reportMemoryLeaks() noexcept
{
    MemoryTracker::ReportLeaks();
}

#pragma warning(push)
#pragma warning(disable: 28251) // Inconsistent annotation for 'new': this instance has no annotations.
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

void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
    void* ptr = std::malloc(size);
    if (ptr != nullptr)
    {
        MemoryTracker::Alloc(ptr, size);
    }
    return ptr;
}

void* operator new(std::size_t size, const std::align_val_t align)
{
//    void* ptr = std::aligned_alloc(static_cast<std::size_t>(align), size);
    void* ptr = _aligned_malloc(size, static_cast<std::size_t>(align));
    if (ptr == nullptr)
    {
        throw std::bad_alloc();
    }
    MemoryTracker::Alloc(ptr, size);
    return ptr;
}

void operator delete(void* ptr, const std::align_val_t) noexcept
{
    MemoryTracker::Free(ptr);
//    std::free(ptr);
    _aligned_free(ptr);
}
#pragma warning(pop)

#endif // DEBUG
