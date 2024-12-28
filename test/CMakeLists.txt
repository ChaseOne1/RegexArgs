find_file(TEST_FRAMEWORK doctest.h HINTS ${CMAKE_CURRENT_SOURCE_DIR} NO_DEFAULT_PATH)
if(NOT TEST_FRAMEWORK)
    message("Cannot find the test framework. Downloading...")
    file(
        DOWNLOAD https://github.com/doctest/doctest/releases/latest/download/doctest.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/doctest.h 
        SHOW_PROGRESS
        STATUS TEST_FRAMEWORK_DOWNLOAD
    )
    list(GET TEST_FRAMEWORK_DOWNLOAD 0 TEST_FRAMEWORK_STATUS)
    if(TEST_FRAME_STATUS)
        message(FATAL_ERROR "Failed to download the test framework.")
    endif()
endif()

aux_source_directory(. TEST_SOURCES)
foreach(TEST_SOURCE ${TEST_SOURCES})
    cmake_path(GET TEST_SOURCE FILENAME TEST_TARGET)
    string(REPLACE ".cc" "" TEST_TARGET ${TEST_TARGET})
    add_executable(${TEST_TARGET} ${TEST_SOURCE})
    target_compile_features(${TEST_TARGET} PRIVATE cxx_std_20)
    target_include_directories(${TEST_TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    if(ARGS_USE_BOOST_REGEX)
        # target_include_directories(${TEST_TARGET} PRIVATE regex/include/)
        target_compile_definitions(${TEST_TARGET} PRIVATE USE_BOOST_REGEX)
    endif()
    set_property(TARGET ${TEST_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
    add_custom_command(TARGET ${TEST_TARGET} POST_BUILD COMMAND ./${TEST_TARGET} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
endforeach()
