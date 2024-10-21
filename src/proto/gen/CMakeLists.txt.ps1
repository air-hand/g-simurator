Function SourceFiles() {
    return (Get-ChildItem .\* -Include "*.cc" | Sort-Object)
}

cd $PSScriptRoot

$source_files = (SourceFiles | % { ("`"{0}`"" -F $_.Name) }) -join "`r`n"

$content = (@'
cmake_policy(SET CMP0076 NEW)
target_sources(${{PROJECT_NAME}}-proto
    PRIVATE
{0}
)
'@ -F $source_files)

Set-Content -Path CMakeLists.txt -Value $content
