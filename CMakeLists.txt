cmake_minimum_required(VERSION 3.15)
project(VZCodeVisualization VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_WASM "Build for WebAssembly" ON)
option(BUILD_TESTS "Build tests" OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party
)

# Source files
set(SOURCES
    src/core/code_analyzer.cpp
    src/core/gemini_client.cpp
    src/parser/mermaid_parser.cpp
    src/parser/vizzu_converter.cpp
    src/vizzu/animation_wrapper.cpp
    src/vizzu/vizzu_integration.cpp
    src/utils/json_utils.cpp
    src/utils/http_client.cpp
)

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

# Install targets
install(TARGETS vzcode
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
