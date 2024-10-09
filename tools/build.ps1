Param(
    [switch]$clean
)

$ErrorActionPreference = "Stop"

cd $PSScriptRoot
cd ..

Remove-Item ${PWD}/src/proto/gen/*.pb.*
vcpkg env --tools "protoc -I${PWD}/src/proto --cpp_out=${PWD}/src/proto/gen ${PWD}/src/proto/*.proto"

Get-ChildItem ./ -Include "CMake*.ps1" -Exclude @("./build", "./tools") -Recurse | Sort-Object | % {
    Write-Host "Running $_"
    pwsh $_
}

$CMAKE_PRESET_COMMAND = @(
    "cmake"
    , "--preset=vcpkg"
    , "-DCMAKE_BUILD_TYPE=Debug"
)
if ($clean) {
    $CMAKE_PRESET_COMMAND += "--fresh"
}
Invoke-Expression -Command ($CMAKE_PRESET_COMMAND -join " ")
$CMAKE_BUILD_COMMAND = @(
    "cmake"
    , "--build"
    , ".\build"
)
if ($clean) {
    $CMAKE_BUILD_COMMAND += "--clean-first"
}
Invoke-Expression -Command ($CMAKE_BUILD_COMMAND -join " ")

if ($? -ne $true) {
    throw "Build failed."
}
# https://discourse.cmake.org/t/ctest-scripting-cmake-presets/9610/2
#ctest --build .\build --preset=test -C Debug
