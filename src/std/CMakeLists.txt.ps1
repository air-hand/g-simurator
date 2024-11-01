$ErrorActionPreference = "Stop"

cd $PSScriptRoot

Copy-Item $Env:VCToolsInstallDir/modules/std.ixx -Destination .\std.ixx -Force
Copy-Item $Env:VCToolsInstallDir/modules/std.compat.ixx -Destination .\std.compat.ixx -Force

#Invoke-Expression -Command (
#    @(
#        "cppwinrt.exe", "-output", ".\", "-verbose"
#        , "-input", "${Env:SystemRoot}\System32\WinMetadata\Windows.Foundation.winmd"
#    ) -join " "
#)
#Add-Content -Path .\winrt\winrt.ixx -Value @(
#    "// Added by ${PSScriptName}"
#    , '#include "../winrt.hpp"'
#)

$source_files = Get-ChildItem .\* -Include @("*.ixx") -Recurse | Sort-Object | % {
    return ((Resolve-Path $_ -Relative) -replace '\\', '/')
} | Join-String -Separator "`r`n"

$content = (@'
cmake_policy(SET CMP0076 NEW)
target_sources(${{PROJECT_NAME}}-std
    PUBLIC
    FILE_SET modules TYPE CXX_MODULES FILES
{0}
)
'@ -F $source_files)

Set-Content -Path CMakeLists.txt -Value $content
