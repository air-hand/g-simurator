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
class RingBuffer final
{
private:
    boost::circular_buffer<T> container_;
    mutable std::mutex mutex_; // lock in const methods
    std::condition_variable is_not_empty_;
public:
    explicit RingBuffer(std::size_t capacity) noexcept : container_(capacity), mutex_()
    {
        assert(capacity > 0);
    }
    ~RingBuffer() = default;

    DELETE_COPY_AND_ASSIGN(RingBuffer);

    bool push(T&& value, std::function<bool(const decltype(container_)&)> condition)
    {
        DEBUG_LOG_SPAN(_);
        {
            auto lock = std::unique_lock<std::mutex>(mutex_);
            if (!condition(container_))
            {
                return false;
            }
            container_.push_back(std::forward<T>(value));
        }
        is_not_empty_.notify_one();
        return true;
    }

    void push(T&& value)
    {
        DEBUG_LOG_SPAN(_);
        push(std::forward<T>(value), []([[maybe_unused]] const auto&) { return true; });
    }

    [[nodiscard]] T pop()
    {
        DEBUG_LOG_SPAN(_);
        auto lock = std::unique_lock<std::mutex>(mutex_);
        is_not_empty_.wait(lock, [this] { return !container_.empty(); });
        auto result = std::move(container_.front());
        container_.pop_front();
        return result;
    }

    [[nodiscard]] std::optional<T> try_pop(std::chrono::milliseconds timeout)
    {
        DEBUG_LOG_SPAN(_);
        auto lock = std::unique_lock<std::mutex>(mutex_);
        if (!is_not_empty_.wait_for(lock, timeout, [this] { return !container_.empty(); }))
        {
            return std::nullopt;
        }
        auto result = std::move(container_.front());
        container_.pop_front();
        return result;
    }

    void clear()
    {
        DEBUG_LOG_SPAN(_);
        auto lock = std::unique_lock<std::mutex>(mutex_);
        container_.clear();
    }

    auto size() const noexcept
    {
        DEBUG_LOG_SPAN(_);
        auto lock = std::unique_lock<std::mutex>(mutex_);
        return container_.size();
    }
};

}
