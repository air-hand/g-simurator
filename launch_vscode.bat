@echo off
cd /d "%~dp0"
pwsh -Command ". .\tools\envs.ps1; code ."
