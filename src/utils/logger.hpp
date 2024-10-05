#pragma once

#include <string>

namespace sim
{
namespace utils
{

namespace logging
{

void init();
void log(const char* message);
void log(const std::string& message);

}

}
}
