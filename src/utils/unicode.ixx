export module utils:unicode;

import std;
import :repro; // 14.52.36328だと、stop_tokenの特殊化でC1116が発生する
// 14.52.36405では問題なし

namespace sim::utils::unicode
{
export std::wstring to_utf16(const std::string& utf8);
export std::string to_utf8(const std::wstring& utf16);
}
