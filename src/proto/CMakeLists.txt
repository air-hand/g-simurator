cmake_policy(SET CMP0076 NEW)

file(GLOB_RECURSE MODULE_SOURCES
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    CONFIGURE_DEPENDS
    "*.ixx"
)
list(SORT MODULE_SOURCES)
file(GLOB_RECURSE PROTO_SOURCES
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    CONFIGURE_DEPENDS
    "*.proto"
)
list(SORT PROTO_SOURCES)

target_sources(${PROJECT_NAME}-proto
    PUBLIC
    FILE_SET modules TYPE CXX_MODULES FILES
        ${MODULE_SOURCES}
)
target_sources(${PROJECT_NAME}-proto
    PRIVATE
        ${PROTO_SOURCES}
)

protobuf_generate(
    TARGET ${PROJECT_NAME}-proto
    OUT_VAR PROTO_GENERATED_FILES
    LANGUAGE cpp
    IMPORT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    PROTOC_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gen"
)

# https://discourse.cmake.org/t/bug-with-generated-file-and-target-sources-private/5262/11
# add_custom_command (protobuf_generate) needs target defined on same scope, so add_dependencies explicitly
add_custom_target(${PROJECT_NAME}-proto-generate
    DEPENDS ${PROTO_GENERATED_FILES}
)
add_dependencies(${PROJECT_NAME}-proto ${PROJECT_NAME}-proto-generate)
