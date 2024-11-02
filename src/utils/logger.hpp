#pragma once

#ifdef DEBUG
#define DEBUG_LOG(x) \
    do \
    { \
        sim::utils::logging::log("DEBUG {}({}): {}", __FILE__, __LINE__, x); \
    } while (false)

#define DEBUG_LOG_ARGS(x, ...) \
    do \
    { \
        sim::utils::logging::log( \
            "DEBUG {}({}): {}", __FILE__, __LINE__, sim::utils::strings::fmt(x, __VA_ARGS__) \
        ); \
    } while (false)

#define DEBUG_LOG_SPAN(x) const sim::utils::logging::LogSpan x(sim::utils::strings::fmt("DEBUG {}", __FUNCSIG__))

#else

#define DEBUG_LOG(_) do {} while (false)
#define DEBUG_LOG_ARGS(_, ...) do {} while (false)
#define DEBUG_LOG_SPAN(_) do {} while (false)

#endif
