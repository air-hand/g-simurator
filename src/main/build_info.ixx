module;

export module sim:build_info;

import std;

namespace sim
{

export std::string_view BuildCommit() noexcept;

}
