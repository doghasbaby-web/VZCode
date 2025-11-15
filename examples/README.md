# VZCode Examples

This directory contains sample code files demonstrating various programming languages and code patterns that can be visualized using VZCode.

## Available Examples

### 1. C++ Data Processing Pipeline (`sample_code.cpp`)
**Description:** Demonstrates a data processing pipeline using abstract base classes and inheritance.

**Key Features:**
- Abstract `Processor` base class
- Concrete implementations: `Doubler`, `EvenFilter`, `Sorter`
- Pipeline pattern for chaining processors
- Template method pattern

**Best Visualization Types:**
- `flowchart` - Shows the data flow through the pipeline
- `class-diagram` - Displays class hierarchy and relationships

**Usage:**
```bash
# In the web interface, select:
# - Language: C++
# - Visualization Type: Class Diagram or Flowchart
# - Paste the contents of sample_code.cpp
```

---

### 2. Python Data Processor (`sample_python.py`)
**Description:** A comprehensive data processing system with validators, transformers, and aggregators.

**Key Features:**
- Abstract base class pattern with `DataProcessor`
- Multiple processor implementations: `Validator`, `Transformer`, `Aggregator`
- Pipeline orchestration
- Statistical aggregation

**Best Visualization Types:**
- `class-diagram` - Shows the inheritance hierarchy
- `flowchart` - Illustrates the processing pipeline flow

**Usage:**
```bash
# In the web interface, select:
# - Language: Python
# - Visualization Type: Class Diagram
# - Paste the contents of sample_python.py
```

---

### 3. JavaScript E-Commerce System (`sample_javascript.js`)
**Description:** A complete e-commerce shopping cart system with products, carts, orders, and payment processing.

**Key Features:**
- ES6 classes
- Shopping cart management
- Order processing
- Payment transaction handling
- Multiple class interactions

**Best Visualization Types:**
- `class-diagram` - Shows class structure and relationships
- `sequence` - Demonstrates the order placement flow
- `flowchart` - Illustrates the checkout process

**Usage:**
```bash
# In the web interface, select:
# - Language: JavaScript
# - Visualization Type: Class Diagram or Sequence
# - Paste the contents of sample_javascript.js
```

---

### 4. Java Library Management (`SampleJava.java`)
**Description:** A library management system with books, magazines, DVDs, members, and borrowing operations.

**Key Features:**
- Abstract class `LibraryItem` with inheritance
- Multiple item types: `Book`, `Magazine`, `DVD`
- Member management
- Late fee calculation
- Search and statistics

**Best Visualization Types:**
- `class-diagram` - Perfect for showing the inheritance hierarchy
- `flowchart` - Shows checkout/return workflow
- `component` - Displays high-level system architecture

**Usage:**
```bash
# In the web interface, select:
# - Language: Java
# - Visualization Type: Class Diagram
# - Paste the contents of SampleJava.java
```

---

## Visualization Type Guide

### Flowchart
Best for: Procedural code, algorithms, control flow

Shows: Execution paths, decision points, loops, function calls

### Class Diagram
Best for: Object-oriented code, inheritance hierarchies

Shows: Classes, inheritance, composition, relationships, methods

### Sequence Diagram
Best for: Interaction patterns, message flows

Shows: Object interactions, method calls, timing, lifecycle

### Component Diagram
Best for: System architecture, module dependencies

Shows: High-level components, dependencies, interfaces

---

## Creating Your Own Examples

When creating new example files:

1. **Choose a clear domain** - Library, E-commerce, Data Processing, etc.
2. **Use meaningful names** - Classes, methods, and variables should be descriptive
3. **Include variety** - Demonstrate different language features
4. **Add comments** - Explain the purpose and key concepts
5. **Keep it focused** - 100-300 lines is ideal for visualization

### Example Structure:
```
1. Header comment explaining the purpose
2. Class/Interface definitions
3. Implementation details
4. Main/demo function showing usage
5. Comments explaining key patterns
```

---

## Tips for Better Visualizations

1. **Clean Code:** Well-structured code produces better diagrams
2. **Naming:** Use descriptive names for classes and methods
3. **Modularity:** Break complex logic into smaller components
4. **Comments:** Include comments when visualization type is set to include them
5. **Size:** Very large files may produce complex diagrams - consider splitting

---

## Testing Examples

### Via Web Interface:
1. Start the development server: `make serve`
2. Open `http://localhost:8000` in your browser
3. Enter your Gemini API key
4. Select the appropriate language and visualization type
5. Paste the example code
6. Click "Analyze Code"

### Via Command Line (if implemented):
```bash
./vzcode_demo --file examples/sample_python.py --type class-diagram
```

---

## Contributing Examples

We welcome new examples! When contributing:

1. Follow the existing example structure
2. Include a variety of language features
3. Test with multiple visualization types
4. Document the key patterns demonstrated
5. Keep examples self-contained

---

## License

All examples are provided under the same MIT License as VZCode.
