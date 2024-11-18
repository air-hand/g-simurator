module;

#include "logger.hpp"

module utils;

import :logger;

namespace sim::utils::time
{

void sleep(uint32_t milliseconds)
{
    DEBUG_LOG_SPAN(_);
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

}
