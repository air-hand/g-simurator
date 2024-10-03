@echo off

rem pwsh -Command "Start-Process pwsh.exe -ArgumentList ""-File %~dp0\setup_vs.ps1"" -Verb RunAs"
pwsh %~dp0\setup_vs.ps1
