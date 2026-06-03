#pragma once

// This macro header is intended for module implementation units that already
// import std and sim::utils::logger through their module interface.

#ifdef DEBUG

#define DEBUG_LOG(fmt, ...) \
    do \
    { \
        sim::utils::logging::log( \
            "DEBUG {}({}): " fmt, __FILE__, __LINE__ __VA_OPT__(, __VA_ARGS__) \
        ); \
    } while (false)

#define DEBUG_LOG_SPAN(label) const sim::utils::logging::LogSpan label##_span(std::format("DEBUG {}({}): {} [{}]", __FILE__, __LINE__, __FUNCSIG__, #label))

#else

#define DEBUG_LOG(_, ...) do {} while (false)
#define DEBUG_LOG_SPAN(_) do {} while (false)

#endif
