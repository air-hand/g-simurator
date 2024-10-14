set(VCPKG_TARGET_ARCHITECTURE x86)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

#set(VCPKG_CMAKE_SYSTEM_NAME Windows)

# /permissive for protobuf.
set(VCPKG_CXX_FLAGS "/std:c++20 /Zc:__cplusplus /permissive /utf-8")
#set(VCPKG_CXX_FLAGS_DEBUG "/DDEBUG")
#set(VCPKG_CXX_FLAGS_RELEASE "/DNDEBUG")
#set(VCPKG_C_FLAGS "/std:c17 /Zc:__STDC__ /utf-8")
set(VCPKG_C_FLAGS "/std:c17 /utf-8")
#set(VCPKG_C_FLAGS_DEBUG "/DDEBUG")
#set(VCPKG_C_FLAGS_RELEASE "/DNDEBUG")

