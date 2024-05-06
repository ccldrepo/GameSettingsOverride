cmake_minimum_required(VERSION 3.28)

project(
    ccld_GameSettingsOverride
    VERSION 1.5.0
    DESCRIPTION "SKSE plugin that allows users to modify game settings via configuration files."
    LANGUAGES CXX
)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION "$<$<CONFIG:Release>:ON>")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
    @ONLY
)

# --- Declare Sources ----------------------------------------------------------

include(cmake/headerlist.cmake)
include(cmake/sourcelist.cmake)

source_group(
    TREE
        ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        ${headers}
        ${sources}
)

# --- Declare Dependencies -----------------------------------------------------

if(NOT CMAKE_PREFIX_PATH AND DEFINED ENV{DEV_INSTALL_PATH})
    cmake_path(SET CMAKE_PREFIX_PATH "$ENV{DEV_INSTALL_PATH}/${CMAKE_BUILD_TYPE}")
endif()

find_package(CommonLibSSE REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(tomlplusplus REQUIRED)

# --- Declare Targets ----------------------------------------------------------

add_commonlibsse_plugin(
    "${PROJECT_NAME}"
    SOURCES
        ${headers}
        ${sources}
        "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
)

target_compile_definitions(
    "${PROJECT_NAME}"
    PRIVATE
        _UNICODE
        NOMINMAX
)

target_compile_features(
    "${PROJECT_NAME}"
    PRIVATE
        cxx_std_23
)

if(MSVC)
    target_compile_options(
        "${PROJECT_NAME}"
        PRIVATE
            /permissive-
            /sdl
            /utf-8
            /W4
            /Zc:preprocessor
    )
endif()

target_include_directories(
    "${PROJECT_NAME}"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    "${PROJECT_NAME}"
    PRIVATE
        fmt::fmt
        spdlog::spdlog
        tomlplusplus::tomlplusplus
)

target_precompile_headers(
    "${PROJECT_NAME}"
    PRIVATE
        src/PCH.h
)
