@echo off

chcp 65001

IF NOT DEFINED VCINSTALLDIR (
    call "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
)

IF NOT DEFINED VCPKG_ROOT (
    set VCPKG_ROOT=%~dp0vendor\vcpkg
)
set PATH=%VCPKG_ROOT%;%PATH%

IF NOT DEFINED VCPKG_TARGET_TRIPLET (
    set VCPKG_TARGET_TRIPLET=x64-windows
)

pwsh %~dp0\
