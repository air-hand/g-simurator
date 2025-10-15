Param(
    [string]$build_type="Debug",
    [int]$clean=0
)

$ErrorActionPreference = "Stop"

$clean = $clean -eq 1

. $PSScriptRoot\envs.ps1

cd $PSScriptRoot\..

Remove-Item ./src/**/proto/gen/*.pb.*
Get-ChildItem ./src -Include "*.proto" -Recurse | % { Split-Path $_ -Parent } | Sort-Object -Unique | % {
    (Resolve-Path $_ -Relative) -replace '\\', '/'
} | % {
    Write-Host "Running protoc $_"
    if (-not(Test-Path "./${_}/gen")) {
        mkdir "./${_}/gen"
    }
    protoc -I"./${_}" --cpp_out="./${_}/gen" "./${_}/*.proto"
}

Get-ChildItem ./ -Exclude @(".git", "build", "tools", "vcpkg_installed") | ForEach-Object -Parallel {
    Get-ChildItem $_.FullName -Include "CMakeLists.txt" -Recurse | % {
        Write-Host "Remove ${_}"
        Remove-Item $_
    }
}
Get-ChildItem ./ -Exclude @(".git", "build", "tools", "vcpkg_installed") | ForEach-Object -Parallel {
    Get-ChildItem $_.FullName -Include "CMake*.ps1" -Recurse | Sort-Object | % {
        Write-Host "Running ${_}"
        pwsh $_
    }
}

$Env:CMAKE_BUILD_PARALLEL_LEVEL = ([int]$Env:NUMBER_OF_PROCESSORS * 2)
$CMAKE_BUILD_TYPE = $build_type

if ($clean) {
    Remove-Item -Recurse -Force ./build
}

$CMAKE_PRESET_COMMAND = @(
    "cmake"
    , "--preset=vcpkg"
    , "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    , "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
    , "-B .\build"
)
if ($clean) {
    $CMAKE_PRESET_COMMAND += "--fresh"
}
Invoke-Expression -Command ($CMAKE_PRESET_COMMAND -join " ")
#Invoke-Expression -Command (@(
#    "cmake"
#    , "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
#    , "-B .\build"
#    ) -join " ")
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
