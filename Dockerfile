FROM mcr.microsoft.com/windows/servercore:ltsc2025

SHELL ["pwsh", "-NoProfile", "-Command"]

WORKDIR C:/work

COPY . .

RUN ./tools/setup_devenv.ps1
