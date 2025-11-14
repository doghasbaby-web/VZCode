# Contributing to VZCode

Thank you for your interest in contributing to VZCode! This document provides guidelines and instructions for contributing.

## Getting Started

1. Fork the repository
2. Clone your fork: `git clone https://github.com/YOUR_USERNAME/VZCode.git`
3. Create a new branch: `git checkout -b feature/your-feature-name`
4. Make your changes
5. Test your changes
6. Commit and push
7. Create a Pull Request

## Development Setup

### Prerequisites

- Emscripten SDK
- CMake 3.15+
- C++17 compatible compiler
- Python 3 (for local server)

### Building

```bash
# Install Emscripten (first time only)
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
cd ..

# Build the project
make wasm

# Run development server
make serve
```

## Code Style

### C++ Guidelines

- Follow C++17 standards
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and concise
- Use RAII principles
- Prefer smart pointers over raw pointers

Example:
```cpp
// Good
std::unique_ptr<DataProcessor> createProcessor(const std::string& type) {
    if (type == "doubler") {
        return std::make_unique<Doubler>();
    }
    return nullptr;
}

// Avoid
DataProcessor* createProcessor(const char* type) {
    return new Doubler();  // Memory leak risk
}
```

### JavaScript Guidelines

- Use modern ES6+ syntax
- Follow consistent naming conventions
- Add JSDoc comments for public APIs
- Use async/await for asynchronous code
- Handle errors gracefully

## Testing

Currently, VZCode doesn't have automated tests. We welcome contributions to add:

- Unit tests for C++ components
- Integration tests for the pipeline
- End-to-end tests for the web interface

## Areas for Contribution

### High Priority

- [ ] Add comprehensive test suite
- [ ] Improve error handling
- [ ] Add support for more programming languages
- [ ] Optimize WASM bundle size
- [ ] Add caching for API responses

### Medium Priority

- [ ] Custom visualization themes
- [ ] Export functionality (PNG, SVG, PDF)
- [ ] Advanced animation controls
- [ ] Batch processing support
- [ ] Configuration file support

### Documentation

- [ ] Add more code examples
- [ ] Create video tutorials
- [ ] Write API documentation
- [ ] Add troubleshooting guide
- [ ] Translate documentation

## Pull Request Process

1. **Update Documentation**: Ensure README and relevant docs are updated
2. **Test Your Changes**: Verify the build works and functionality is correct
3. **Write Clear Commits**: Use descriptive commit messages
4. **Reference Issues**: Link to related issues in your PR description
5. **Be Responsive**: Address review comments promptly

### Commit Message Format

```
type(scope): subject

body

footer
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `test`: Adding tests
- `chore`: Maintenance tasks

Example:
```
feat(parser): add support for state diagrams

Implement parsing logic for Mermaid state diagrams,
including state transitions and nested states.

Closes #123
```

## Code Review

All submissions require review. We use GitHub pull requests for this purpose. Reviewers will check:

- Code quality and style
- Functionality and correctness
- Test coverage
- Documentation completeness
- Performance implications

## Community

- Be respectful and constructive
- Help others learn and grow
- Share knowledge and ideas
- Follow the Code of Conduct

## Questions?

Feel free to:
- Open an issue for bugs or feature requests
- Start a discussion for questions
- Reach out to maintainers

Thank you for contributing to VZCode!
