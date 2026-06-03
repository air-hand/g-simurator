export module utils:strings;

import std;

namespace sim::utils::strings
{

export std::string remove_all_whitespaces(const std::string in)
{
    auto view = in | std::views::filter([](char c) {
        const unsigned char uc = static_cast<unsigned char>(c);
        // skip if ASCII space
        if (uc < 0x80 && (uc == ' ' || uc == '\t')) {
            return false;
        }
        return true;
    });
    return std::string(view.begin(), view.end());
}


}
