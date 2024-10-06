cd $PSScriptRoot

$source_files = (Get-ChildItem .\* -Include "*.cpp" | Sort-Object | % {
    return ("`"{0}`"" -F $_.Name)
} | Join-String -Separator "`r`n")

$content = (@'
cmake_policy(SET CMP0076 NEW)
target_sources(${{PROJECT_NAME}}
    PRIVATE
{0}
)
'@ -F $source_files)

Set-Content -Path CMakeLists.txt -Value $content
