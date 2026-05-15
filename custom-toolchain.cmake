if(DEFINED _VCPKG_ROOT_DIR)
  message(STATUS "_VCPKG_ROOT_DIR ${_VCPKG_ROOT_DIR}")
endif()
if(DEFINED ENV{VCPKG_ROOT})
  message(STATUS "ENV{VCPKG_ROOT} $ENV{VCPKG_ROOT}")
endif()

include("$ENV{VCPKG_ROOT}/scripts/toolchains/windows.cmake")

set(CMAKE_CUDA_ARCHITECTURES "86" CACHE STRING "")
