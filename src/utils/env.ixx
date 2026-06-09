module;

export module utils:env;

import std;

namespace sim::utils
{
export std::optional<std::string> get_env(const char* name);
}
