Param(
    [string]$build_type="Debug"
)

$ErrorActionPreference = 'Stop'

. $PSScriptRoot\envs.ps1

cd $PSScriptRoot\..

$BUILD_DIR = ".\build\${build_type}"
& "${BUILD_DIR}\simurator-test.exe"

## https://discourse.cmake.org/t/ctest-scripting-cmake-presets/9610/2
#$CMAKE_TEST_COMMAND = @(
#    "ctest"
#    , "--build"
#    , ".\build"
#    , "--preset=test"
#    , "-C"
#    , "Debug"
#)
#Invoke-Expression -Command ($CMAKE_TEST_COMMAND -join " ")
