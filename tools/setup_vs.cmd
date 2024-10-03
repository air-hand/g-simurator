@echo off

pwsh -Command "Start-Process pwsh.exe -ArgumentList ""-File %~dp0\setup_vs.ps1"" -Verb RunAs"
rem pwsh %~dp0\setup_vs.ps1
