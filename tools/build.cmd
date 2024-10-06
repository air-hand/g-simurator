@echo off

chcp 65001

call "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"

pwsh %~dp0\build.ps1

