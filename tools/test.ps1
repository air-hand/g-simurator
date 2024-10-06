cd $PSScriptRoot
cd ..

# https://discourse.cmake.org/t/ctest-scripting-cmake-presets/9610/2
ctest --build .\build --preset=test -C Debug
