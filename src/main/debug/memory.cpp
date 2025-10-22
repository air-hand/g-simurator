#include "memory.hpp"

#define MI_MALLOC_OVERRIDE 0

#if MI_MALLOC_OVERRIDE
#include <mimalloc.h>
//#include <mimalloc-new-delete.h>

#define SIM_MALLOC(size) ::mi_malloc(size)
#define SIM_FREE(ptr) ::mi_free(ptr)
#define SIM_ALIGNED_ALLOC(size, align) ::mi_malloc_aligned(size, static_cast<std::size_t>(align))
#define SIM_ALIGNED_FREE(ptr, align) ::mi_free_aligned(ptr, static_cast<std::size_t>(align))

#else


#define SIM_MALLOC(size) std::malloc(size)
#define SIM_FREE(ptr) std::free(ptr)
#define SIM_ALIGNED_ALLOC(size, align) ::_aligned_malloc(size, static_cast<std::size_t>(align))
#define SIM_ALIGNED_FREE(ptr, align) ::_aligned_free(ptr)

#endif

#ifdef DEBUG

#include <boost/stacktrace.hpp>
#include "utils/logger.hpp"
#include "utils/macro.hpp"

import std.compat;
import utils;

#if OVERRIDE_NEW_DELETE

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

    class FlagGuard final
    {
    private:
        std::atomic<bool>& flag_;
    public:
        explicit FlagGuard(std::atomic<bool>& flag) noexcept : flag_(flag) 
        {
            flag_.store(false, std::memory_order_relaxed);
        }
        ~FlagGuard() noexcept
        {
            flag_.store(true, std::memory_order_relaxed);
        }
        DELETE_COPY_AND_ASSIGN(FlagGuard);
    };

    static void Initialize() noexcept
    {
        if (!tracking_)
        {
            tracking_ = true;
            DEBUG_LOG("Memory tracker tracking");
        }
    }

    static void Alloc(void* ptr, std::size_t size);
    static void Free(void* ptr);
    static void ReportLeaks() noexcept;
private:
    static inline std::atomic<bool> tracking_ = false;
    static inline std::unordered_map<void*, AllocationInfo> allocations_;
    static inline std::mutex mutex_;
};

void MemoryTracker::Alloc(void* ptr, std::size_t size)
{
    if (!tracking_)
    {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    FlagGuard _(tracking_);
    allocations_.emplace(ptr, AllocationInfo({size, boost::stacktrace::stacktrace()}));
}

void MemoryTracker::Free(void* ptr)
{
    if (!tracking_)
    {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    FlagGuard _(tracking_);
    allocations_.erase(ptr);
}

void MemoryTracker::ReportLeaks() noexcept
{
    if (!tracking_)
    {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    // Disable track memory allocations below.
    FlagGuard _(tracking_);
    if (allocations_.empty())
    {
        DEBUG_LOG("No memory leaks");
        return;
    }
    DEBUG_LOG("Memory leaks detected:");
    for (const auto& [ptr, info] : allocations_)
    {
        DEBUG_LOG_ARGS("Memory leak: {} addr {} bytes", ptr, info.size);
        DEBUG_LOG_ARGS("Stack trace:\n{}", boost::stacktrace::to_string(info.trace));
    }
}

}

namespace sim::debug
{
MemoryTrackerSpan::MemoryTrackerSpan()
{
    MemoryTracker::Initialize();
}

MemoryTrackerSpan::~MemoryTrackerSpan()
{
    MemoryTracker::ReportLeaks();
}
}

#endif // OVERRIDE_NEW_DELETE

#endif // DEBUG

#ifdef DEBUG
#define DEBUG_MEMORY_TRACKER_ALLOC(ptr, size) MemoryTracker::Alloc(ptr, size)
#define DEBUG_MEMORY_TRACKER_FREE(ptr) MemoryTracker::Free(ptr)
#else
#define DEBUG_MEMORY_TRACKER_ALLOC(ptr, size) do {} while (0)
#define DEBUG_MEMORY_TRACKER_FREE(ptr) do {} while (0)
#endif


#if OVERRIDE_NEW_DELETE
#pragma warning(push)
#pragma warning(disable: 28251) // Inconsistent annotation for 'new': this instance has no annotations.
void* operator new(std::size_t size)
{
    void* ptr = SIM_MALLOC(size);
    if (ptr == nullptr)
    {
        throw std::bad_alloc();
    }
    DEBUG_MEMORY_TRACKER_ALLOC(ptr, size);
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    DEBUG_MEMORY_TRACKER_FREE(ptr);
    SIM_FREE(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept
{
    DEBUG_MEMORY_TRACKER_FREE(ptr);
    SIM_FREE(ptr);
}

void* operator new[](std::size_t size)
{
    void* ptr = SIM_MALLOC(size);
    if (ptr == nullptr)
    {
        throw std::bad_alloc();
    }
    DEBUG_MEMORY_TRACKER_ALLOC(ptr, size);
    return ptr;
}

void operator delete[](void* ptr) noexcept
{
    DEBUG_MEMORY_TRACKER_FREE(ptr);
    SIM_FREE(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept
{
    DEBUG_MEMORY_TRACKER_FREE(ptr);
    SIM_FREE(ptr);
}

void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
    void* ptr = SIM_MALLOC(size);
    if (ptr != nullptr)
    {
        DEBUG_MEMORY_TRACKER_ALLOC(ptr, size);
    }
    return ptr;
}

void* operator new(std::size_t size, const std::align_val_t align)
{
    void* ptr = SIM_ALIGNED_ALLOC(size, align);
    if (ptr == nullptr)
    {
        throw std::bad_alloc();
    }
    DEBUG_MEMORY_TRACKER_ALLOC(ptr, size);
    return ptr;
}

void operator delete(void* ptr, [[maybe_unused]] const std::align_val_t align) noexcept
{
    DEBUG_MEMORY_TRACKER_FREE(ptr);
    SIM_ALIGNED_FREE(ptr, align);
}

#pragma warning(pop)
#endif
