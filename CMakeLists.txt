cmake_minimum_required(VERSION 3.20)

project(RegexArgs VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)

option(ARGS_BUILD_TESTS "build the unit tests")
option(ARGS_BUILD_SINGLE_HEADER "build the single header file")
option(ARGS_USE_BOOST_REGEX "build with boost's regex library")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_include_directories(${PROJECT_NAME} INTERFACE include)
if(ARGS_USE_BOOST_REGEX)
    target_compile_definitions(${PROJECT_NAME} INTERFACE USE_BOOST_REGEX)
endif()

if(ARGS_BUILD_SINGLE_HEADER)
    find_package(Python3 3.7 REQUIRED)
    find_path(QUOM_DIR quom HINTS ${CMAKE_CURRENT_SOURCE_DIR}/scripts NO_DEFAULT_PATH)
    if(NOT QUOM_DIR)
        message("Downloading the generation tool...")
        #file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts)
        FetchContent_Declare(
            quom
            GIT_REPOSITORY https://github.com/Viatorus/quom.git
            GIT_PROGRESS TRUE
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/scripts/quom
        )
        FetchContent_MakeAvailable(quom)
    endif()
    add_custom_target(
        RegexArgs_single_header
        ALL
        COMMAND Python3::Interpreter -m
            quom
                ${CMAKE_CURRENT_SOURCE_DIR}/include/RegexArgs.hpp
                ${CMAKE_CURRENT_SOURCE_DIR}/include/RegexArgs_gen.hpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/quom/src
        COMMENT "Generating single header file..."
    )
endif()

if(ARGS_BUILD_TESTS)
    add_subdirectory(test)
endif()
