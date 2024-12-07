﻿cd $PSScriptRoot

Function SourceFiles([string]$extension) {
    return (Get-ChildItem .\* -Include @("*.${extension}") -Recurse | Sort-Object | % {
        return ((Resolve-Path $_ -Relative) -replace '\\', '/')
    } | Join-String -Separator "`r`n")
}

$content = (@'
target_sources(${{PROJECT_NAME}}
    PRIVATE
{0}
)
'@ -F (SourceFiles "cpp")
)

Set-Content -Path CMakeLists.txt -Value $content
