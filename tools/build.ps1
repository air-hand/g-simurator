$ErrorActionPreference = "Stop"

cd $PSScriptRoot
cd ..

if (-not(Test-Path "${Env:VCPKG_ROOT}\.git")) {
    git clone https://github.com/microsoft/vcpkg.git $Env:VCPKG_ROOT
    cd $Env:VCPKG_ROOT
    .\bootstrap-vcpkg.bat
    cd -
}

Get-ChildItem ./ -Include "CMake*.ps1" -Exclude @("./build", "./tools") -Recurse | Sort-Object | % {
    Write-Host "Running $_"
    pwsh $_
}

cmake --preset=vcpkg -DCMAKE_BUILD_TYPE=Debug
cmake --build .\build

if ($? -ne $true) {
    throw "Build failed."
}
# https://discourse.cmake.org/t/ctest-scripting-cmake-presets/9610/2
#ctest --build .\build --preset=test -C Debug
