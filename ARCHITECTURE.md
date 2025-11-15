# VZCode Architecture - Vizzu C++ Kernel Integration

## Overview

VZCode is a WASM-based code visualization system that integrates the **Vizzu C++ kernel** directly to provide native-performance animated visualizations. This document describes the architecture and implementation of the Vizzu C++ kernel integration.

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         User Code Input                          │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│              Gemini AI API (Code Analysis)                       │
│         - Analyzes code structure and logic                      │
│         - Generates Mermaid diagram description                  │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│                   Mermaid Parser (C++)                           │
│         - Parses Mermaid syntax                                  │
│         - Extracts nodes, edges, and relationships               │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│                 Vizzu Data Converter (C++)                       │
│         - Converts Mermaid structure to Vizzu format             │
│         - Creates series (dimensions and measures)               │
│         - Generates animation sequences                          │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│             Vizzu Kernel Wrapper (C++ Shell)                     │
│         ┌───────────────────────────────────────┐               │
│         │   Vizzu C++ Kernel (third_party)      │               │
│         │   - Chart rendering engine             │               │
│         │   - Animation system                   │               │
│         │   - Data management                    │               │
│         │   - Canvas operations                  │               │
│         └───────────────────────────────────────┘               │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│              WASM Bindings (Emscripten)                          │
│         - Exposes C++ API to JavaScript                          │
│         - Manages memory between JS and C++                      │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│         Interactive Animated Visualization (Browser)             │
└─────────────────────────────────────────────────────────────────┘
```

## Key Components

### 1. Vizzu Kernel Wrapper (`src/vizzu/vizzu_kernel_wrapper.hpp/cpp`)

**NEW C++ SHELL AROUND VIZZU KERNEL**

This is the core wrapper that directly interfaces with the Vizzu C++ library kernel:

```cpp
class VizzuKernelWrapper {
public:
    // Initialize Vizzu chart instance using C++ API
    bool initialize();

    // Add data using Vizzu C++ kernel functions
    bool add_dimension(const std::string& name, const std::vector<std::string>& categories);
    bool add_measure(const std::string& name, const std::vector<double>& values);

    // Configure chart using Vizzu C++ kernel
    bool set_config(const std::string& path, const std::string& value);
    bool set_style(const std::string& path, const std::string& value);

    // Animation control using Vizzu C++ kernel
    bool begin_keyframe();
    bool set_animation_config(const std::string& path, const std::string& value);
    bool animate();

    // Rendering using Vizzu C++ kernel
    bool update(double time_ms);
    bool render(double width, double height);

private:
    Vizzu::Interface* vizzu_interface_;  // Direct access to Vizzu C++ kernel
    void* chart_handle_;                  // Vizzu chart instance
    void* canvas_handle_;                 // Vizzu canvas instance
};
```

**Key Features:**
- **Direct C++ API Access**: Uses `Vizzu::Interface` from `third_party/vizzu-lib/src/apps/weblib/interface.h`
- **Memory Management**: Properly manages Vizzu object handles
- **Error Handling**: Comprehensive error handling with detailed messages
- **Animation Support**: Full animation sequence control via C++ kernel

### 2. Vizzu Integration (`src/vizzu/vizzu_integration.hpp/cpp`)

**ENHANCED TO USE C++ KERNEL**

Updated to use the kernel wrapper instead of just generating JavaScript:

```cpp
class VizzuIntegration {
public:
    // Initialize using C++ kernel directly
    bool initialize_kernel();

    // Render using C++ kernel
    bool render_with_kernel(double width, double height);

    // Animate using C++ kernel
    bool animate_with_kernel();

    // Get direct access to kernel wrapper
    VizzuKernelWrapper* get_kernel_wrapper() const;

private:
    std::unique_ptr<VizzuKernelWrapper> kernel_wrapper_;  // The actual Vizzu C++ kernel!
};
```

### 3. WASM Bindings (`src/bindings/wasm_bindings.cpp`)

**UPDATED TO EXPOSE KERNEL FUNCTIONS**

New functions exposed to JavaScript:

```cpp
class VZCodeAPI {
public:
    // Check if Vizzu C++ kernel is available
    bool isVizzuKernelAvailable() const;

    // Get Vizzu kernel version
    std::string getVizzuKernelVersion() const;

    // Render using C++ kernel directly
    std::string renderWithKernel(const std::string& mermaid_text, int width, int height);
};
```

### 4. CMake Build System (`CMakeLists.txt`)

**CONFIGURED TO BUILD WITH VIZZU C++ KERNEL**

```cmake
# Enable Vizzu C++ kernel integration
option(ENABLE_VIZZU_KERNEL "Enable Vizzu C++ kernel integration" ON)

if(ENABLE_VIZZU_KERNEL)
    add_definitions(-DVIZZU_KERNEL_AVAILABLE)

    # Include Vizzu library headers
    include_directories(
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src/apps/weblib
    )

    # Collect and build Vizzu library source files
    file(GLOB_RECURSE VIZZU_SOURCES
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src/base/*.cpp
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src/chart/*.cpp
        ${CMAKE_SOURCE_DIR}/third_party/vizzu-lib/src/dataframe/*.cpp
    )

    list(APPEND SOURCES ${VIZZU_SOURCES})
endif()
```

## Data Flow

### Complete Visualization Pipeline

1. **Code Input** → User provides code in any supported language

2. **Gemini AI Analysis** →
   - `core::GeminiClient` sends code to Gemini API
   - Receives Mermaid diagram description

3. **Mermaid Parsing** →
   - `parser::MermaidParser` parses Mermaid syntax
   - Extracts graph structure (nodes, edges)

4. **Vizzu Conversion** →
   - `parser::VizzuConverter` converts to Vizzu data format
   - Creates series (dimensions, measures)
   - Generates animation steps

5. **Vizzu Kernel Integration** →
   - `vizzu::VizzuKernelWrapper` initializes Vizzu C++ kernel
   - Loads data using `vizzu_interface_->addDimension()` and `addMeasure()`
   - Configures chart using `vizzu_interface_->setChartValue()`
   - Sets up animation using `vizzu_interface_->setKeyframe()` and `animate()`

6. **Rendering** →
   - Vizzu C++ kernel renders to canvas
   - Updates occur via `vizzu_interface_->update()`
   - Final render via `vizzu_interface_->render()`

7. **WASM Export** →
   - C++ functions exposed to JavaScript via Emscripten
   - Browser can call `vzcode.renderWithKernel()` directly

## Third-Party Dependencies

### Vizzu Library (`third_party/vizzu-lib/`)

- **Source**: https://github.com/vizzuhq/vizzu-lib
- **Integration Point**: `src/apps/weblib/interface.h` and `cinterface.h`
- **Key Files**:
  - `interface.cpp` - Main Vizzu C++ interface
  - `cinterface.cpp` - C API wrapper
  - `src/chart/` - Chart rendering engine
  - `src/base/` - Base utilities and geometry
  - `src/dataframe/` - Data management

## Building

### Build with Vizzu C++ Kernel (Default)

```bash
mkdir build && cd build
emcmake cmake .. -DENABLE_VIZZU_KERNEL=ON
emmake make
```

### Build without Kernel (JavaScript-only mode)

```bash
mkdir build && cd build
emcmake cmake .. -DENABLE_VIZZU_KERNEL=OFF
emmake make
```

## API Usage

### JavaScript API with C++ Kernel

```javascript
const module = await VZCodeModule();
const vzcode = new module.VZCodeAPI();

// Check if kernel is available
if (vzcode.isVizzuKernelAvailable()) {
    console.log('Vizzu kernel version:', vzcode.getVizzuKernelVersion());

    // Use C++ kernel for rendering
    const result = vzcode.renderWithKernel(mermaidDiagram, 800, 600);
    console.log('Rendered with C++ kernel:', JSON.parse(result));
}
```

### C++ API (Direct)

```cpp
#include "vizzu/vizzu_kernel_wrapper.hpp"

vzcode::vizzu::VizzuKernelWrapper kernel;

// Initialize
if (kernel.initialize()) {
    // Add data
    kernel.add_dimension("Component", {"A", "B", "C"});
    kernel.add_measure("Value", {10.0, 20.0, 30.0});

    // Configure
    kernel.set_config("config.channels.x", "{\"set\": [\"Component\"]}");
    kernel.set_config("config.channels.y", "{\"set\": [\"Value\"]}");

    // Animate
    kernel.begin_keyframe();
    kernel.animate();

    // Render
    kernel.render(800, 600);
}
```

## Performance Benefits

Using the C++ kernel provides:

1. **Native Performance**: No JavaScript overhead for data processing
2. **Direct Memory Access**: Efficient data transfer without serialization
3. **Optimized Rendering**: C++ rendering engine compiled to WASM
4. **Animation Control**: Direct animation state management
5. **Memory Efficiency**: Better memory management through C++

## Comparison: Before vs. After

### Before (JavaScript Generation)

```cpp
// vizzu_integration.cpp - OLD
std::string VizzuIntegration::generate_js_code() const {
    ss << "const chart = new Vizzu(containerId, {...});\n";  // Just generates JavaScript strings!
    return ss.str();
}
```

### After (C++ Kernel Integration)

```cpp
// vizzu_integration.cpp - NEW
bool VizzuIntegration::render_with_kernel(double width, double height) {
    return kernel_wrapper_->render(width, height);  // Directly uses Vizzu C++ kernel!
}
```

## Future Enhancements

- [ ] Server-side rendering with C++ kernel
- [ ] Export animations to video using C++ kernel
- [ ] Advanced animation sequences via kernel API
- [ ] Custom chart types using kernel extensibility
- [ ] Performance profiling and optimization

## License

This integration maintains compatibility with both:
- VZCode: MIT License
- Vizzu Library: Apache 2.0 License

---

**Built with ❤️ using C++, WebAssembly, Gemini AI, and Vizzu C++ Kernel**
