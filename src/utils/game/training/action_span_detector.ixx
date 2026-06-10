module;

#include "utils/logger.hpp"
#include "utils/macro.hpp"

export module utils:game.training.action_span_detector;

import std;

import :capture;
import :logger;

namespace utils = sim::utils;

namespace sim::utils::game::training
{

export template <typename T>
concept SpanBoundaryDetector =
    std::movable<T> &&
    requires(const T& detector, const utils::CapturedImage& img) {
        { detector(img) } -> std::same_as<bool>;
    };

export template <SpanBoundaryDetector Detector>
class ActionSpanDetector final
{
public:
    explicit ActionSpanDetector(Detector detector)
        : detector_(std::move(detector)) {}
    DECLARE_MOVE_CONSTRUCTOR_DEFAULT(ActionSpanDetector);
    DELETE_COPY_AND_ASSIGN(ActionSpanDetector);
    std::optional<std::vector<utils::CapturedImage>> push(utils::CapturedImage&& image)
    {
        DEBUG_LOG_SPAN(_);
        const bool isBoundary = detector_(image);
        switch(state_)
        {
        case State::Waiting:
            if (isBoundary)
            {
                state_ = State::Bound;
            }
            return std::nullopt;
        case State::Bound:
            if (!isBoundary)
            {
                state_ = State::OnSpan;
                buffer_.emplace_back(std::move(image));
            }
            return std::nullopt;
        case State::OnSpan:
            if (isBoundary)
            {
                state_ = State::Waiting;
                return std::exchange(buffer_, {});
            }
            buffer_.emplace_back(std::move(image));
            return std::nullopt;
        default:
            return std::nullopt;
        }
    }
private:
    enum class State {
        Waiting,
        Bound,
        OnSpan,
    };
    State state_ = State::Waiting;
    Detector detector_;
    std::vector<utils::CapturedImage> buffer_;
};

}
