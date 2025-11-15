# VZCode Test Suite

Comprehensive test suite for the VZCode visualization system using Google Test framework.

## Overview

The test suite covers:
- **Unit Tests**: Individual component testing
- **Integration Tests**: Multi-component pipeline testing
- **Edge Cases**: Boundary conditions and error handling

## Test Structure

```
tests/
├── test_json_utils.cpp        # JSON parsing and serialization tests
├── test_mermaid_parser.cpp    # Mermaid diagram parsing tests
├── test_vizzu_converter.cpp   # Mermaid to Vizzu conversion tests
├── test_code_analyzer.cpp     # Code analysis and preprocessing tests
├── test_integration.cpp       # End-to-end integration tests
└── README.md                  # This file
```

## Building and Running Tests

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- Internet connection (for downloading Google Test)

### Build Tests

```bash
# Create build directory
mkdir -p build
cd build

# Configure with tests enabled (native build, not WASM)
cmake -DBUILD_WASM=OFF -DBUILD_TESTS=ON -DENABLE_VIZZU_KERNEL=OFF ..

# Build
cmake --build .

# Or use make
make
```

### Run All Tests

```bash
# Using CTest
ctest

# Or with verbose output
ctest --verbose

# Or with output on failure
ctest --output-on-failure
```

### Run Specific Tests

```bash
# Run individual test executable
./bin/test_json_utils
./bin/test_mermaid_parser
./bin/test_vizzu_converter
./bin/test_code_analyzer
./bin/test_integration

# Run specific test cases with gtest filter
./bin/test_json_utils --gtest_filter=JsonUtilsTest.ParseNumber
./bin/test_mermaid_parser --gtest_filter=MermaidParserTest.ParseSimpleFlowchart
```

## Test Coverage

### test_json_utils.cpp (60+ tests)

Tests the JSON utility library including:
- **Value Creation**: Null, Boolean, Number, String, Array, Object
- **Serialization**: Converting JsonValue to JSON strings
- **Parsing**: JSON string to JsonValue conversion
- **Round-trip**: Parse → Stringify → Parse consistency
- **Error Handling**: Invalid JSON detection
- **Edge Cases**: Empty strings, large numbers, nested structures, scientific notation

Example tests:
```cpp
TEST_F(JsonUtilsTest, ParseNumber)
TEST_F(JsonUtilsTest, StringifyNestedStructure)
TEST_F(JsonUtilsTest, RoundTripSimple)
```

### test_mermaid_parser.cpp (35+ tests)

Tests Mermaid diagram parsing including:
- **Diagram Types**: Flowchart, Graph, Class Diagram, Sequence Diagram
- **Node Types**: Rectangle, Rounded, Diamond, Circle
- **Edge Types**: Solid, Dotted, Thick arrows
- **Edge Labels**: Labeled connections
- **Validation**: Diagram correctness checking
- **Comments**: Comment skipping
- **Whitespace**: Handling of extra spaces

Example tests:
```cpp
TEST_F(MermaidParserTest, ParseSimpleFlowchart)
TEST_F(MermaidParserTest, ParseNodeTypes)
TEST_F(MermaidParserTest, ParseEdgeLabels)
```

### test_vizzu_converter.cpp (20+ tests)

Tests conversion from Mermaid to Vizzu format:
- **Data Structure Conversion**: Nodes and edges to Vizzu series
- **Animation Generation**: Creating animation sequences
- **Configuration**: Vizzu chart configuration
- **Multiple Diagram Types**: Flowchart, Class, Sequence diagrams
- **JSON Generation**: Vizzu data to JSON format
- **Large Graphs**: Performance with many nodes

Example tests:
```cpp
TEST_F(VizzuConverterTest, ConvertSimpleFlowchart)
TEST_F(VizzuConverterTest, AnimationStepsGenerated)
TEST_F(VizzuConverterTest, ToJsonFormat)
```

### test_code_analyzer.cpp (25+ tests)

Tests code preprocessing and analysis:
- **Comment Removal**: Single-line and multi-line comments
- **String Literal Handling**: Avoiding false comment detection
- **Whitespace Preservation**: Maintaining code structure
- **Insight Extraction**: Key phrase identification
- **Diagram Merging**: Combining multiple diagrams
- **Options Handling**: Analysis configuration
- **Edge Cases**: Empty code, only comments, nested comments

Example tests:
```cpp
TEST_F(CodeAnalyzerTest, PreprocessCodeRemovesComments)
TEST_F(CodeAnalyzerTest, ExtractInsightsFindsKeyPhrases)
TEST_F(CodeAnalyzerTest, MergeMultipleDiagrams)
```

### test_integration.cpp (15+ tests)

End-to-end integration tests:
- **Full Pipeline**: Mermaid → Parse → Convert → JSON
- **Multiple Diagram Types**: Testing all supported types
- **Round-trip Processing**: Complete data flow
- **Error Handling**: Pipeline error propagation
- **Large Data**: Performance with complex diagrams
- **Complex Nested Structures**: Deep JSON hierarchies

Example tests:
```cpp
TEST_F(IntegrationTest, MermaidToVizzuPipeline)
TEST_F(IntegrationTest, ComplexNestedJsonParsing)
TEST_F(IntegrationTest, LargeDataHandling)
```

## Test Statistics

| Component | Test File | Tests | Lines |
|-----------|-----------|-------|-------|
| JSON Utils | test_json_utils.cpp | 60+ | ~650 |
| Mermaid Parser | test_mermaid_parser.cpp | 35+ | ~450 |
| Vizzu Converter | test_vizzu_converter.cpp | 20+ | ~350 |
| Code Analyzer | test_code_analyzer.cpp | 25+ | ~400 |
| Integration | test_integration.cpp | 15+ | ~400 |
| **Total** | | **155+** | **~2,250** |

## Writing New Tests

### Test Structure

Follow the Google Test pattern:

```cpp
#include <gtest/gtest.h>
#include "../src/component/header.hpp"

class ComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test fixtures
    }

    void TearDown() override {
        // Cleanup
    }

    // Helper methods
};

TEST_F(ComponentTest, TestName) {
    // Arrange
    // Act
    // Assert
    EXPECT_EQ(expected, actual);
}
```

### Assertions

Common Google Test assertions:
```cpp
EXPECT_TRUE(condition);
EXPECT_FALSE(condition);
EXPECT_EQ(expected, actual);
EXPECT_NE(not_expected, actual);
EXPECT_LT(val1, val2);
EXPECT_GT(val1, val2);
EXPECT_DOUBLE_EQ(expected, actual);
EXPECT_THROW(statement, exception_type);
EXPECT_NO_THROW(statement);
```

### Best Practices

1. **Descriptive Names**: Use clear, descriptive test names
2. **Single Responsibility**: Each test should verify one thing
3. **Arrange-Act-Assert**: Follow AAA pattern
4. **Independence**: Tests should not depend on each other
5. **Coverage**: Test normal cases, edge cases, and errors
6. **Performance**: Keep tests fast; use fixtures for expensive setup

### Adding Tests to Build

Edit `CMakeLists.txt`:

```cmake
add_executable(test_new_component tests/test_new_component.cpp)
target_link_libraries(test_new_component vzcode_lib gtest gtest_main)
add_test(NAME NewComponentTest COMMAND test_new_component)
```

## Continuous Integration

### GitHub Actions Example

```yaml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Build
        run: |
          mkdir build && cd build
          cmake -DBUILD_WASM=OFF -DBUILD_TESTS=ON -DENABLE_VIZZU_KERNEL=OFF ..
          make
      - name: Run Tests
        run: cd build && ctest --output-on-failure
```

## Troubleshooting

### Tests Not Building

1. Check CMake configuration:
   ```bash
   cmake -DBUILD_WASM=OFF -DBUILD_TESTS=ON ..
   ```

2. Ensure BUILD_TESTS is ON and BUILD_WASM is OFF (tests don't work with WASM build)

3. Check compiler supports C++17

### Tests Failing

1. Run with verbose output:
   ```bash
   ctest --verbose
   ```

2. Run individual test:
   ```bash
   ./bin/test_json_utils --gtest_filter=JsonUtilsTest.FailingTest
   ```

3. Check for missing dependencies or include paths

### Google Test Not Downloading

1. Check internet connection
2. Try manual download:
   ```bash
   cd build/_deps
   git clone https://github.com/google/googletest.git
   ```

## Code Coverage (Optional)

### Using gcov/lcov

```bash
# Build with coverage flags
cmake -DBUILD_WASM=OFF -DBUILD_TESTS=ON -DCMAKE_CXX_FLAGS="--coverage" ..
make

# Run tests
ctest

# Generate coverage report
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info
lcov --list coverage.info
```

### Using llvm-cov

```bash
# Build with clang coverage
cmake -DBUILD_WASM=OFF -DBUILD_TESTS=ON \
      -DCMAKE_CXX_FLAGS="-fprofile-instr-generate -fcoverage-mapping" ..
make

# Run tests
LLVM_PROFILE_FILE="test.profraw" ctest

# Generate report
llvm-profdata merge -sparse test.profraw -o test.profdata
llvm-cov show ./bin/test_json_utils -instr-profile=test.profdata
```

## Performance Testing

Run tests with timing:
```bash
ctest --verbose --timeout 10
```

Individual test timing:
```bash
./bin/test_integration --gtest_filter=IntegrationTest.LargeDataHandling
```

## Future Improvements

- [ ] Add mocking for Gemini API calls
- [ ] Add performance benchmarks
- [ ] Increase code coverage to 90%+
- [ ] Add fuzz testing
- [ ] Add memory leak detection (Valgrind)
- [ ] Add thread safety tests
- [ ] Add property-based testing

## Contributing

When adding new features:
1. Write tests first (TDD approach)
2. Ensure all existing tests pass
3. Add tests for new functionality
4. Maintain test coverage above 80%
5. Document complex test scenarios

## License

Tests are part of VZCode and covered under the MIT License.
