# ---------------------------------------------------------------
# embed_resources.cmake
#
# Provides: embed_resources(<target>
#     INPUTS <list of files>
#     [OUTPUT_DIR <dir>]
# )
#
# Example:
#   embed_resources(game_executable
#     INPUTS ${CMAKE_SOURCE_DIR}/assets/logo.png
#            ${CMAKE_SOURCE_DIR}/data/config.json
#            ${CMAKE_SOURCE_DIR}/sounds/hit.wav
#   )
# ---------------------------------------------------------------

function(embed_resources TARGET_NAME)
    set(options)
    set(oneValueArgs OUTPUT_DIR NAMESPACE)
    set(multiValueArgs INPUTS)
    cmake_parse_arguments(EMBED "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT EMBED_INPUTS)
        message(FATAL_ERROR "embed_resources: You must specify at least one file in INPUTS.")
    endif()

    if(NOT EMBED_OUTPUT_DIR)
        set(EMBED_OUTPUT_DIR ${CMAKE_BINARY_DIR}/embedded_data/${TARGET_NAME})
    endif()

    file(MAKE_DIRECTORY ${EMBED_OUTPUT_DIR})

    # Path to the embed tool (should be provided by FetchContent)
    if(NOT TARGET embed)
        message(FATAL_ERROR "embed_resources: The 'embed' tool target was not found.")
    endif()
    set(EMBED_TOOL $<TARGET_FILE:embed>)

    # Compute relative paths for the tool input
    set(EMBED_REL_INPUTS "")
    foreach(_in ${EMBED_INPUTS})
        file(RELATIVE_PATH _rel ${CMAKE_SOURCE_DIR} ${_in})
        list(APPEND EMBED_REL_INPUTS ${_rel})
    endforeach()

    # Command to generate headers
    set(GENERATED_HEADERS
        ${EMBED_OUTPUT_DIR}/resource_holder.hpp
        ${EMBED_OUTPUT_DIR}/resource.hpp
        ${EMBED_OUTPUT_DIR}/span.hpp
    )

    add_custom_command(
        OUTPUT ${GENERATED_HEADERS}
        # COMMAND ${EMBED_TOOL} ${EMBED_INPUTS} -o ${EMBED_OUTPUT_DIR}
        COMMAND ${EMBED_TOOL} ${EMBED_REL_INPUTS} -o ${EMBED_OUTPUT_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        DEPENDS ${EMBED_INPUTS} embed
        COMMENT "Embedding resources for target '${TARGET_NAME}'"
        VERBATIM
    )

    # Logical build target for convenience
    add_custom_target(${TARGET_NAME}_embedded
        DEPENDS ${GENERATED_HEADERS}
    )

    # Ensure target builds after embeds
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}_embedded)

    # Include generated headers
    target_include_directories(${TARGET_NAME} PRIVATE ${EMBED_OUTPUT_DIR})

    # Mark outputs for IDE visibility
    set_source_files_properties(${GENERATED_HEADERS} PROPERTIES GENERATED TRUE)
endfunction()
