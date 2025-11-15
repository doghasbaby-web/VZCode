cmake_minimum_required(VERSION 3.15)
project(VZCodeVisualization VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_WASM "Build for WebAssembly" ON)
option(BUILD_TESTS "Build tests" ON)
option(ENABLE_VIZZU_KERNEL "Enable Vizzu C++ kernel integration" ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party
)

# Vizzu library integration
if(ENABLE_VIZZU_KERNEL)
    message(STATUS "Vizzu C++ kernel integration enabled")
    add_definitions(-DVIZZU_KERNEL_AVAILABLE)

    # Include Vizzu library headers
    include_directories(
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src/apps/weblib
    )

    # Collect Vizzu library source files
    file(GLOB_RECURSE VIZZU_SOURCES
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src/base/*.cpp
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src/chart/*.cpp
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src/dataframe/*.cpp
    )

    # Add Vizzu weblib sources
    list(APPEND VIZZU_SOURCES
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src/apps/weblib/interface.cpp
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src/apps/weblib/cinterface.cpp
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src/apps/weblib/jscriptcanvas.cpp
    )

    list(LENGTH VIZZU_SOURCES VIZZU_SOURCE_COUNT)
    message(STATUS "Found ${VIZZU_SOURCE_COUNT} Vizzu source files")
endif()

# Source files
set(SOURCES
    src/core/code_analyzer.cpp
    src/core/gemini_client.cpp
    src/parser/mermaid_parser.cpp
    src/parser/vizzu_converter.cpp
    src/vizzu/animation_wrapper.cpp
    src/vizzu/vizzu_integration.cpp
    src/vizzu/vizzu_kernel_wrapper.cpp
    src/utils/json_utils.cpp
    src/utils/http_client.cpp
)

# Add Vizzu sources if kernel is enabled
if(ENABLE_VIZZU_KERNEL)
    list(APPEND SOURCES ${VIZZU_SOURCES})
endif()

# WASM-specific settings
if(BUILD_WASM)
    message(STATUS "Building for WebAssembly")

    set(CMAKE_EXECUTABLE_SUFFIX ".js")

    # Emscripten flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ALLOW_MEMORY_GROWTH=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s EXPORTED_RUNTIME_METHODS='[\"ccall\",\"cwrap\"]'")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s EXPORTED_FUNCTIONS='[\"_malloc\",\"_free\"]'")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s FETCH=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ASYNCIFY=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s MODULARIZE=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s EXPORT_NAME='VZCodeModule'")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ENVIRONMENT='web,worker'")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind")

    # Optimization
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    endif()

    add_executable(vzcode ${SOURCES} src/bindings/wasm_bindings.cpp)

    # Copy output to web directory
    add_custom_command(TARGET vzcode POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vzcode.js
        ${CMAKE_SOURCE_DIR}/web/vzcode.js
    )
    add_custom_command(TARGET vzcode POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vzcode.wasm
        ${CMAKE_SOURCE_DIR}/web/vzcode.wasm
    )
else()
    # Native build
    add_library(vzcode_lib STATIC ${SOURCES})
    add_executable(vzcode_demo src/main.cpp)
    target_link_libraries(vzcode_demo vzcode_lib)
endif()

# Tests
if(BUILD_TESTS AND NOT BUILD_WASM)
    enable_testing()

    # Download Google Test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Test executables
    add_executable(test_json_utils tests/test_json_utils.cpp)
    target_link_libraries(test_json_utils vzcode_lib gtest gtest_main)
    add_test(NAME JsonUtilsTest COMMAND test_json_utils)

    add_executable(test_mermaid_parser tests/test_mermaid_parser.cpp)
    target_link_libraries(test_mermaid_parser vzcode_lib gtest gtest_main)
    add_test(NAME MermaidParserTest COMMAND test_mermaid_parser)

    add_executable(test_vizzu_converter tests/test_vizzu_converter.cpp)
    target_link_libraries(test_vizzu_converter vzcode_lib gtest gtest_main)
    add_test(NAME VizzuConverterTest COMMAND test_vizzu_converter)

    add_executable(test_code_analyzer tests/test_code_analyzer.cpp)
    target_link_libraries(test_code_analyzer vzcode_lib gtest gtest_main)
    add_test(NAME CodeAnalyzerTest COMMAND test_code_analyzer)

    add_executable(test_integration tests/test_integration.cpp)
    target_link_libraries(test_integration vzcode_lib gtest gtest_main)
    add_test(NAME IntegrationTest COMMAND test_integration)

    message(STATUS "Tests enabled - use 'make test' or 'ctest' to run")
endif()

# Install targets
if(BUILD_WASM)
    install(TARGETS vzcode
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
else()
    install(TARGETS vzcode_lib vzcode_demo
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()
