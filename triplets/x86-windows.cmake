set(VCPKG_TARGET_ARCHITECTURE x86)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

# /permissive for protobuf.
set(VCPKG_CXX_FLAGS "/Zc:__cplusplus /permissive /utf-8")
set(VCPKG_C_FLAGS "/utf-8")
