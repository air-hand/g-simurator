cd $PSScriptRoot

Copy-Item $Env:VCToolsInstallDir/modules/std.ixx -Destination .\std.ixx -Force
Copy-Item $Env:VCToolsInstallDir/modules/std.compat.ixx -Destination .\std.compat.ixx -Force

$source_files = Get-ChildItem .\* -Include @("*.ixx") | Sort-Object | % {
    return ("`"{0}`"" -F $_.Name)
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
