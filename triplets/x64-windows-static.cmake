include("${VCPKG_ROOT_DIR}/triplets/x64-windows-static.cmake")

set(VCPKG_CXX_FLAGS "/Zc:__cplusplus /permissive /utf-8" CACHE STRING "")
#set(VCPKG_CXX_FLAGS_DEBUG "${VCPKG_CXX_FLAGS_DEBUG} /fsanitize=address /GS" CACHE STRING "")
set(VCPKG_C_FLAGS "/utf-8" CACHE STRING "")
#set(VCPKG_C_FLAGS_DEBUG "${VCPKG_C_FLAGS_DEBUG} /fsanitize=address /GS" CACHE STRING "")
