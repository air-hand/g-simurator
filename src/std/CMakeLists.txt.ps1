$ErrorActionPreference = "Stop"

cd $PSScriptRoot

Copy-Item $Env:VCToolsInstallDir/modules/std.ixx -Destination .\std.ixx -Force
Copy-Item $Env:VCToolsInstallDir/modules/std.compat.ixx -Destination .\std.compat.ixx -Force

Function SourceFiles([string]$extension) {
    return (Get-ChildItem .\* -Include @("*.${extension}") -Recurse | Sort-Object | % {
        return ((Resolve-Path $_ -Relative) -replace '\\', '/')
    } | Join-String -Separator "`r`n")
}

$content = (@'
cmake_policy(SET CMP0076 NEW)
target_sources(${{PROJECT_NAME}}-std
    PUBLIC
    FILE_SET modules TYPE CXX_MODULES FILES
{0}
)
target_sources(${{PROJECT_NAME}}-std
    PRIVATE
{1}
)
'@ -F (SourceFiles "ixx"), (SourceFiles "cpp"))

Set-Content -Path CMakeLists.txt -Value $content
