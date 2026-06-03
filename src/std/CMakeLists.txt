cmake_policy(SET CMP0076 NEW)

set(EXTERNAL_MODULES_SOURCES
    "$ENV{VCToolsInstallDir}/modules/std.ixx"
    "$ENV{VCToolsInstallDir}/modules/std.compat.ixx"
)

set(MODULE_SOURCES)
foreach(module IN LISTS EXTERNAL_MODULES_SOURCES)
    if(NOT EXISTS "${module}")
        message(FATAL_ERROR "External module not found: ${module}")
    endif()
    get_filename_component(module_file "${module}" NAME)
    set(OUTPUT_TO "${CMAKE_CURRENT_SOURCE_DIR}/${module_file}")
    add_custom_command(
        OUTPUT "${OUTPUT_TO}"
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different "${module}" "${OUTPUT_TO}"
        DEPENDS
            "${module}"
        COMMENT "Copying ${module_file}"
        VERBATIM
    )
    list(APPEND MODULE_SOURCES "${OUTPUT_TO}")
endforeach()

target_sources(${PROJECT_NAME}-std
    PUBLIC
    FILE_SET modules TYPE CXX_MODULES FILES
        ${MODULE_SOURCES}
)

# https://discourse.cmake.org/t/bug-with-generated-file-and-target-sources-private/5262/11
# add_custom_command needs target defined on same scope, so add_dependencies explicitly
add_custom_target(${PROJECT_NAME}-std-copy
    DEPENDS ${MODULE_SOURCES}
)
add_dependencies(${PROJECT_NAME}-std ${PROJECT_NAME}-std-copy)
