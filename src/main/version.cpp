#define STR1(x) #x
#define STR(x)  STR1(x)

static_assert(__cplusplus == 202004L, "C++23 required (/std:c++latest + /Zc:__cplusplus) Current: " STR(__cplusplus));
static_assert(_MSC_VER == 1942, "MSVC 19.42 required. Current: " STR(_MSC_VER));
static_assert(_WIN32_WINNT == 0x0A00, "Windows 10 or 11 required. Current: " STR(_MSC_VER));
