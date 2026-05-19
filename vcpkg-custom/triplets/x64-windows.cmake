include("${VCPKG_ROOT_DIR}/triplets/x64-windows.cmake")

# /permissive for protobuf.
set(VCPKG_CXX_FLAGS "/Zc:__cplusplus /permissive /utf-8" CACHE STRING "")
#set(VCPKG_CXX_FLAGS_DEBUG "${VCPKG_CXX_FLAGS_DEBUG} /fsanitize=address" CACHE STRING "")
#set(VCPKG_LINKER_FLAGS_DEBUG "${VCPKG_LINKER_FLAGS_DEBUG} /INCREMENTAL:NO" CACHE STRING "")
set(VCPKG_C_FLAGS "/utf-8" CACHE STRING "")
#set(VCPKG_C_FLAGS_DEBUG "${VCPKG_C_FLAGS_DEBUG} /fsanitize=address" CACHE STRING "")

set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../toolchains/custom-toolchain.cmake")
# need to with VCPKG_CHAINLOAD_TOOLCHAIN_FILE on Windows
set(VCPKG_LOAD_VCVARS_ENV ON)
# need to be ENV{VCPKG_ROOT} used by custom toolchain on Windows
set(VCPKG_ENV_PASSTHROUGH_UNTRACKED "VCPKG_ROOT")
