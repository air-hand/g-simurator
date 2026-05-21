FROM mcr.microsoft.com/dotnet/framework/runtime:4.8.1-windowsservercore-ltsc2025

SHELL ["pwsh", "-NoProfile", "-Command"]

WORKDIR D:/work

COPY tools/ .\tools
