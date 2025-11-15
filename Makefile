# Makefile for VZCode Visualization System

# Emscripten compiler
EMCC = emcc
EMXX = em++

# Directories
SRC_DIR = src
BUILD_DIR = build
WEB_DIR = web
DIST_DIR = dist

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra
EMFLAGS = -s WASM=1 \
          -s ALLOW_MEMORY_GROWTH=1 \
          -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
          -s FETCH=1 \
          -s ASYNCIFY=1 \
          -s MODULARIZE=1 \
          -s EXPORT_NAME='VZCodeModule' \
          -s ENVIRONMENT='web,worker' \
          --bind

# Source files
SOURCES = $(SRC_DIR)/core/code_analyzer.cpp \
          $(SRC_DIR)/core/gemini_client.cpp \
          $(SRC_DIR)/parser/mermaid_parser.cpp \
          $(SRC_DIR)/parser/vizzu_converter.cpp \
          $(SRC_DIR)/vizzu/animation_wrapper.cpp \
          $(SRC_DIR)/vizzu/vizzu_integration.cpp \
          $(SRC_DIR)/vizzu/vizzu_kernel_wrapper.cpp \
          $(SRC_DIR)/utils/json_utils.cpp \
          $(SRC_DIR)/utils/http_client.cpp \
          $(SRC_DIR)/bindings/wasm_bindings.cpp

# Include paths
INCLUDES = -I./include -I./src -I./third_party

# Targets
.PHONY: all wasm clean setup help serve

all: setup wasm

setup:
	@mkdir -p $(BUILD_DIR) $(WEB_DIR) $(DIST_DIR)
	@echo "Created build directories"

wasm: setup
	@echo "Building WASM module..."
	$(EMXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) $(EMFLAGS) -O3 -o $(WEB_DIR)/vzcode.js
	@echo "WASM build complete: $(WEB_DIR)/vzcode.js and $(WEB_DIR)/vzcode.wasm"

debug: setup
	@echo "Building WASM module (debug)..."
	$(EMXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) $(EMFLAGS) -g -O0 -o $(WEB_DIR)/vzcode.js
	@echo "Debug WASM build complete"

cmake-build:
	@mkdir -p build
	cd build && emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
	cd build && emmake make
	@echo "CMake build complete"

clean:
	@rm -rf $(BUILD_DIR) $(DIST_DIR)
	@rm -f $(WEB_DIR)/vzcode.js $(WEB_DIR)/vzcode.wasm $(WEB_DIR)/vzcode.wasm.map
	@echo "Cleaned build artifacts"

serve:
	@echo "Starting development server on http://localhost:8000"
	@cd $(WEB_DIR) && python3 -m http.server 8000

help:
	@echo "VZCode Visualization System Build"
	@echo ""
	@echo "Available targets:"
	@echo "  make          - Build WASM module (release)"
	@echo "  make wasm     - Build WASM module (release)"
	@echo "  make debug    - Build WASM module (debug)"
	@echo "  make cmake-build - Build using CMake"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make serve    - Start development server"
	@echo "  make help     - Show this help message"
