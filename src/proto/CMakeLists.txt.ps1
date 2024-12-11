cd $PSScriptRoot

Function SourceProtoFiles() {
    return (Get-ChildItem .\* -Include @("*.proto") -Recurse | Sort-Object | % {
        return ((Resolve-Path $_ -Relative) -replace '\\', '/')
    } | % {
        return "`${CMAKE_CURRENT_SOURCE_DIR}/${_}"
    } | Join-String -Separator "`r`n")
}

$content = (@'
cmake_policy(SET CMP0076 NEW)
set(PROTO_GEN_DIR ${{CMAKE_CURRENT_SOURCE_DIR}}/gen)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    {0}
    PROTOC_OUT_DIR ${{PROTO_GEN_DIR}}
)

target_sources(${{PROJECT_NAME}}-proto
    PRIVATE
    ${{PROTO_SRCS}}
#    ${{PROTO_HDRS}}
)
'@ -F (SourceProtoFiles))

Set-Content -Path CMakeLists.txt -Value $content
