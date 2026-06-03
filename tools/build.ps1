Param(
    [string]$build_type="Debug",
    [int]$clean=0
)

$ErrorActionPreference = "Stop"

$clean = $clean -eq 1

. $PSScriptRoot\envs.ps1

cd $PSScriptRoot\..

$Env:CMAKE_BUILD_PARALLEL_LEVEL = ([int]$Env:NUMBER_OF_PROCESSORS * 2)
$CMAKE_BUILD_TYPE = $build_type
$BUILD_DIR = ".\build\${CMAKE_BUILD_TYPE}"

if ($clean -and (Test-Path $BUILD_DIR -PathType Container)) {
    Remove-Item -Recurse -Force $BUILD_DIR
}

$CMAKE_PRESET_COMMAND = @(
    "cmake"
    , "--preset=vcpkg"
    , "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    , "-DVCPKG_INSTALLED_DIR=${PSScriptRoot}/../vcpkg_installed"
    , "-B ${BUILD_DIR}"
)
if ($clean) {
    $CMAKE_PRESET_COMMAND += "--fresh"
}
Invoke-Expression -Command ($CMAKE_PRESET_COMMAND -join " ")
$CMAKE_BUILD_COMMAND = @(
    "cmake"
    , "--build"
    , "${BUILD_DIR}"
)
if ($clean) {
    $CMAKE_BUILD_COMMAND += "--clean-first"
}
# continue ninja build if some failures
$CMAKE_BUILD_COMMAND += "-- -k 0"
Invoke-Expression -Command ($CMAKE_BUILD_COMMAND -join " ")
if ($? -ne $true) {
    throw "Build failed."
}
# https://clangd.llvm.org/installation#compile_commandsjson
Copy-Item $BUILD_DIR/compile_commands.json .\build
