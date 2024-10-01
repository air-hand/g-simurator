rem @echo off

rem pwsh -NoProfile -Command "Start-Process pwsh.exe -File .\setup_vs.ps1 -Verb RunAs" -WorkingDirectory "%~dp0"
rem pwsh -NoProfile -Command "Start-Process pwsh.exe -File .\setup_vs.ps1 -Verb RunAs" -WorkingDirectory "%~dp0"
rem pwsh -Command "Start-Process pwsh.exe -ArgumentList ""-File .\setup_vs.ps1 -Verb RunAs""" -WorkingDirectory "%~dp0"
pwsh -Command "Start-Process pwsh.exe -ArgumentList ""-File %~dp0\setup_vs.ps1"" -Verb RunAs"

