module;

export module utils:algorithm;

import std;

namespace sim::utils::algorithm
{

// Avoids UB and dangling reference caused by std::clamp
export template <typename T>
constexpr T safe_clamp(const T& value, const T& low, const T& high)
{
    if (low > high) {
        return safe_clamp(value, high, low);
    }

    if (value < low) {
        return low;
    } else if (value > high) {
        return high;
    } else {
        return value;
    }
}

}

