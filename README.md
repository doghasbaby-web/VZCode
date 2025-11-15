# VZCode - AI-Powered Code Visualization

VZCode is a WebAssembly-based code visualization system that leverages **Gemini AI** to analyze code and generate interactive, animated visualizations using **Vizzu**.

## 🌟 Features

- **AI-Powered Analysis**: Uses Google's Gemini API to intelligently analyze code structure
- **Multiple Diagram Types**: Supports flowcharts, class diagrams, sequence diagrams, and component diagrams
- **Mermaid Integration**: Generates Mermaid diagrams as an intermediate representation
- **Interactive Visualizations**: Converts Mermaid to Vizzu for animated, interactive charts
- **WebAssembly Performance**: C++ core compiled to WASM for high performance
- **Multi-Language Support**: Analyze code in C++, JavaScript, Python, Java, Go, Rust, and more
- **Animated Transitions**: Smooth animations between different visualization states

## 🏗️ Architecture

```
Code Input
    ↓
Gemini API Analysis (AI-powered code understanding)
    ↓
Mermaid Diagram Generation
    ↓
Mermaid Parser (C++)
    ↓
Vizzu Data Converter (C++)
    ↓
Vizzu Animation Wrapper (C++)
    ↓
Interactive Visualization
```

## 🚀 Quick Start

### Prerequisites

- **Emscripten SDK** for compiling to WebAssembly
- **CMake** (3.15+) or use the provided Makefile
- **Gemini API Key** from [Google AI Studio](https://ai.google.dev/)

### Installation

1. **Clone the repository**:
```bash
git clone https://github.com/yourusername/VZCode.git
cd VZCode
```

2. **Install Emscripten**:
```bash
# Follow instructions at https://emscripten.org/docs/getting_started/downloads.html
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
cd ..
```

3. **Build the project**:

Using Make:
```bash
make wasm
```

Or using CMake:
```bash
mkdir build && cd build
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
emmake make
cd ..
```

4. **Run the demo**:
```bash
make serve
# Or manually:
cd web && python3 -m http.server 8000
```

5. **Open in browser**:
Navigate to `http://localhost:8000`

## 📖 Usage

### Web Interface

1. Enter your Gemini API key in the configuration section
2. Paste your code in the editor
3. Select the programming language and visualization type
4. Click "Analyze Code"
5. View the generated Mermaid diagram and interactive Vizzu visualization

### Command Line (Native Build)

```bash
./vzcode_demo \
  --api-key YOUR_GEMINI_API_KEY \
  --file path/to/your/code.cpp \
  --language cpp \
  --viz-type flowchart \
  --output visualization.html
```

### Programmatic Usage (C++)

```cpp
#include "core/code_analyzer.hpp"
#include "vizzu/animation_wrapper.hpp"

// Create analyzer
vzcode::core::CodeAnalyzer analyzer("YOUR_API_KEY");

// Set options
vzcode::core::CodeAnalyzer::AnalysisOptions options;
options.language = "cpp";
options.visualization_type = "flowchart";

// Analyze code
auto result = analyzer.analyze_code(source_code, options);

if (result.success) {
    std::cout << "Mermaid:\n" << result.mermaid_diagram << std::endl;
}
```

### JavaScript API (WebAssembly)

```javascript
// Load WASM module
const module = await VZCodeModule();
const vzcode = new module.VZCodeAPI();

// Set API key
vzcode.setApiKey('YOUR_GEMINI_API_KEY');

// Analyze code
const resultJson = vzcode.generateVisualization(
    code,
    'cpp',
    'flowchart'
);

const result = JSON.parse(resultJson);
if (result.success) {
    console.log('Mermaid:', result.mermaid);
    console.log('Vizzu Data:', result.vizzuData);
}
```

## 📁 Project Structure

```
VZCode/
├── src/
│   ├── core/              # Core analysis logic
│   │   ├── code_analyzer.cpp
│   │   └── gemini_client.cpp
│   ├── parser/            # Mermaid parser and converter
│   │   ├── mermaid_parser.cpp
│   │   └── vizzu_converter.cpp
│   ├── vizzu/             # Vizzu integration and animation
│   │   ├── vizzu_integration.cpp
│   │   └── animation_wrapper.cpp
│   ├── utils/             # Utility classes
│   │   ├── http_client.cpp
│   │   └── json_utils.cpp
│   └── bindings/          # WebAssembly bindings
│       └── wasm_bindings.cpp
├── web/                   # Web demo interface
│   ├── index.html
│   ├── styles.css
│   └── demo.js
├── CMakeLists.txt
├── Makefile
└── README.md
```

## 🔧 Build Options

### Makefile Targets

- `make` or `make wasm` - Build WASM module (release)
- `make debug` - Build WASM module (debug mode)
- `make cmake-build` - Build using CMake
- `make clean` - Remove build artifacts
- `make serve` - Start development server
- `make help` - Show available targets

### CMake Options

- `BUILD_WASM` - Build for WebAssembly (default: ON)
- `BUILD_TESTS` - Build test suite (default: OFF)
- `CMAKE_BUILD_TYPE` - Release or Debug

## 🎨 Visualization Types

1. **Flowchart**: Shows program execution flow and control structures
2. **Class Diagram**: Displays class relationships and hierarchies
3. **Sequence Diagram**: Illustrates interactions between components
4. **Component Diagram**: Shows high-level architecture and dependencies

## 🔑 API Configuration

### Getting a Gemini API Key

1. Visit [Google AI Studio](https://ai.google.dev/)
2. Sign in with your Google account
3. Create a new API key
4. Copy the key and use it in VZCode

### Environment Variables

You can also set the API key via environment variable:

```bash
export GEMINI_API_KEY="your-api-key-here"
```

## 🌐 Browser Support

- Chrome/Edge (recommended)
- Firefox
- Safari
- Any modern browser with WebAssembly support

## 📦 Dependencies

### Runtime Dependencies
- [Vizzu](https://github.com/vizzuhq/vizzu-lib) - Visualization library (loaded via CDN)
- Gemini API - Code analysis (API key required)

### Build Dependencies
- Emscripten - WebAssembly compiler
- CMake 3.15+ - Build system
- C++17 compiler

## 🧪 Testing

VZCode now includes a comprehensive test suite with 155+ tests using Google Test framework.

### Running Tests

```bash
# Build with tests enabled (native build)
mkdir build && cd build
cmake -DBUILD_WASM=OFF -DBUILD_TESTS=ON -DENABLE_VIZZU_KERNEL=OFF ..
make

# Run all tests
ctest

# Or run with verbose output
ctest --verbose

# Run specific test suite
./bin/test_json_utils
./bin/test_mermaid_parser
./bin/test_vizzu_converter
./bin/test_code_analyzer
./bin/test_integration
```

### Test Coverage

- **test_json_utils.cpp** (60+ tests): JSON parsing, serialization, validation
- **test_mermaid_parser.cpp** (35+ tests): Mermaid diagram parsing and validation
- **test_vizzu_converter.cpp** (20+ tests): Mermaid to Vizzu conversion
- **test_code_analyzer.cpp** (25+ tests): Code preprocessing and analysis
- **test_integration.cpp** (15+ tests): End-to-end integration tests

See [tests/README.md](tests/README.md) for detailed testing documentation.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- [Google Gemini](https://ai.google.dev/gemini-api) for AI-powered code analysis
- [Vizzu](https://github.com/vizzuhq/vizzu-lib) for animated visualizations
- [Mermaid](https://mermaid.js.org/) for diagram syntax inspiration
- [Emscripten](https://emscripten.org/) for WebAssembly compilation

## 📞 Support

For questions, issues, or feature requests, please open an issue on GitHub.

## 🔮 Future Enhancements

- [ ] Support for more programming languages
- [ ] Custom visualization themes
- [ ] Export to various formats (PNG, SVG, PDF)
- [ ] Real-time collaboration features
- [ ] Integration with code repositories (GitHub, GitLab)
- [ ] Advanced animation controls
- [ ] Custom diagram styling options

---

**Built with ❤️ using C++, WebAssembly, Gemini AI, and Vizzu**
