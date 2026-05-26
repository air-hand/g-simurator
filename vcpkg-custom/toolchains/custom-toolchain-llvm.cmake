include("${CMAKE_CURRENT_LIST_DIR}/../utils.cmake")

set(CMAKE_CXX_STANDARD 23 CACHE STRING "")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "")

set(CMAKE_CUDA_STANDARD 20 CACHE STRING "")
# forward to all ports by vcpkg
# https://developer.nvidia.com/cuda-gpus
# GeForce RTX 30 Series (Ampere): 8.6
# GeForce RTX 40 Series (Ada Lovelace): 8.9
set(CMAKE_CUDA_ARCHITECTURES "86" CACHE STRING "")
#set(CMAKE_CUDA_ARCHITECTURES "86;89" CACHE STRING "")


set(CMAKE_C_COMPILER "$ENV{llvmX64}/clang-cl.exe" CACHE FILEPATH "")
#set(CMAKE_CXX_COMPILER "$ENV{llvmX64}/clang-cl.exe" CACHE FILEPATH "")
set(CMAKE_CXX_COMPILER "$ENV{llvmX64}/clang-cl.exe" CACHE FILEPATH "")
set(CMAKE_LINKER "$ENV{llvmX64}/lld-link.exe" CACHE FILEPATH "")
set(CMAKE_RC_COMPILER "$ENV{llvmX64}/llvm-rc.exe" CACHE FILEPATH "")
set(CMAKE_CUDA_COMPILER "$ENV{CUDA_PATH}/bin/nvcc.exe")
#set(CMAKE_CUDA_HOST_COMPILER "$ENV{llvmX64}/clang-cl.exe" CACHE FILEPATH "")
set(CMAKE_CUDA_HOST_COMPILER "$ENV{VCToolsInstallDir}/bin/Hostx64/x64/cl.exe" CACHE FILEPATH "")

# need VCPKG_KEEP_ENV_VARS and VCPKG_ENV_PASSTHROUGH_UNTRACKED to use ENV{VCPKG_ROOT} on Windows
include("$ENV{VCPKG_ROOT}/scripts/toolchains/windows.cmake")

# issue with zlib /c65001
replace_var(CMAKE_RC_FLAGS "/c65001" "-c65001")
# maybe error with cuda + clang-cl
#string(APPEND CMAKE_CUDA_FLAGS " --keep --use-local-env --allow-unsupported-compiler -ccbin "cl.exe")
#string(APPEND CMAKE_CUDA_FLAGS_INIT " --allow-unsupported-compiler")
