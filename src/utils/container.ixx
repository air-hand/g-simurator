module;

#include <cassert>

#include <boost/circular_buffer.hpp>

#include "macro.hpp"
#include "logger.hpp"

export module utils:container;

import std;
import :logger;

namespace sim::utils::container
{

export template <typename T>
class BoundedBuffer final
{
public:
    explicit BoundedBuffer(std::size_t capacity) noexcept : container_(capacity), mutex_()
    {
        assert(capacity > 0);
    }
    ~BoundedBuffer() = default;

    DELETE_COPY_AND_ASSIGN(BoundedBuffer);

    void push(const T& value)
    {
        DEBUG_LOG_SPAN(_);
        auto lock = std::unique_lock<std::mutex>(mutex_);
        // TODO: ここで待つのもできるが、popも相応にできないといけないので一方を待つだけでもいいんじゃないだろうか
        //condition_.wait(lock, [this] { return !container_.full(); });
        container_.push_back(value);
        lock.unlock();
        is_not_empty_.notify_one();
    }

    auto pop()
    {
        DEBUG_LOG_SPAN(_);
        auto lock = std::unique_lock<std::mutex>(mutex_);
        is_not_empty_.wait(lock, [this] { return !container_.empty(); });
        auto result = container_.front();
        container_.pop_front();
        return result;
    }

    auto size() noexcept
    {
        DEBUG_LOG_SPAN(_);
        auto lock = std::unique_lock<std::mutex>(mutex_);
        return container_.size();
    }

private:
    boost::circular_buffer<T> container_;
    std::mutex mutex_;
    std::condition_variable is_not_empty_;
};

}
