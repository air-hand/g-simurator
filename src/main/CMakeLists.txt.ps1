cd $PSScriptRoot

Function SourceFiles([string]$extension) {
    return (Get-ChildItem .\* -Include @("*.${extension}") -Recurse | Sort-Object | % {
#        return ("`"{0}`"" -F $_.Name)
#        return ("`"{0}`"" -F (Resolve-Path $_ -Relative))
        return ((Resolve-Path $_ -Relative) -replace '\\', '/')
    } | Join-String -Separator "`r`n")
}

$content = (@'
cmake_policy(SET CMP0076 NEW)
target_sources(${{PROJECT_NAME}}
    PRIVATE
    FILE_SET modules TYPE CXX_MODULES FILES
{0}
)
target_sources(${{PROJECT_NAME}}
    PRIVATE
{1}
)
'@ -F (SourceFiles "ixx"), (SourceFiles "cpp"))

Set-Content -Path CMakeLists.txt -Value $content
