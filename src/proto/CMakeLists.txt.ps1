cd $PSScriptRoot

Function SourceFiles([string]$extension) {
    return (Get-ChildItem .\* -Include @("*.${extension}") -Recurse | Sort-Object | % {
        return ((Resolve-Path $_ -Relative) -replace '\\', '/')
    } | Join-String -Separator "`r`n")
}

$content = (@'
cmake_policy(SET CMP0076 NEW)
target_sources(${{PROJECT_NAME}}-proto
    PUBLIC
    FILE_SET modules TYPE CXX_MODULES FILES
{0}
)
target_sources(${{PROJECT_NAME}}-proto
    PRIVATE
{1}
)
'@ -F (SourceFiles "ixx"), (SourceFiles "cc"))

Set-Content -Path CMakeLists.txt -Value $content
