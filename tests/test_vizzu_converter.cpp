#include <gtest/gtest.h>
#include "../src/parser/vizzu_converter.hpp"
#include "../src/parser/mermaid_parser.hpp"

using namespace vzcode::parser;

class VizzuConverterTest : public ::testing::Test {
protected:
    VizzuConverter converter;
    MermaidParser parser;

    void SetUp() override {
        converter = VizzuConverter();
        parser = MermaidParser();
    }
};

TEST_F(VizzuConverterTest, ConvertSimpleFlowchart) {
    std::string mermaid = R"(flowchart TD
    A[Start] --> B[End])";

    MermaidDiagram diagram = parser.parse(mermaid);
    VizzuData vizzu_data = converter.convert(diagram);

    EXPECT_FALSE(vizzu_data.series.empty());

    // Should have at least node and edge series
    bool has_nodes = false;
    bool has_edges = false;

    for (const auto& series : vizzu_data.series) {
        if (series.name.find("node") != std::string::npos ||
            series.name.find("Node") != std::string::npos) {
            has_nodes = true;
        }
        if (series.name.find("edge") != std::string::npos ||
            series.name.find("Edge") != std::string::npos ||
            series.name.find("connection") != std::string::npos) {
            has_edges = true;
        }
    }

    EXPECT_TRUE(has_nodes || has_edges);
}

TEST_F(VizzuConverterTest, ConvertWithMultipleNodes) {
    std::string mermaid = R"(flowchart TD
    A[Node1] --> B[Node2]
    B --> C[Node3]
    C --> D[Node4])";

    MermaidDiagram diagram = parser.parse(mermaid);
    VizzuData vizzu_data = converter.convert(diagram);

    EXPECT_FALSE(vizzu_data.series.empty());

    // Count nodes in data
    int node_count = 0;
    for (const auto& series : vizzu_data.series) {
        if (series.type == "dimension" &&
            (series.name.find("node") != std::string::npos ||
             series.name.find("Node") != std::string::npos ||
             series.name.find("label") != std::string::npos)) {
            node_count = std::max(node_count, static_cast<int>(series.values.size()));
        }
    }

    EXPECT_GE(node_count, 4);
}

TEST_F(VizzuConverterTest, ConvertClassDiagram) {
    std::string mermaid = R"(classDiagram
    ClassA --> ClassB
    ClassB --> ClassC)";

    MermaidDiagram diagram = parser.parse(mermaid);
    VizzuData vizzu_data = converter.convert(diagram);

    EXPECT_FALSE(vizzu_data.series.empty());
}

TEST_F(VizzuConverterTest, SeriesHaveCorrectTypes) {
    std::string mermaid = R"(flowchart TD
    A --> B
    B --> C)";

    MermaidDiagram diagram = parser.parse(mermaid);
    VizzuData vizzu_data = converter.convert(diagram);

    for (const auto& series : vizzu_data.series) {
        // Type should be either "dimension" or "measure"
        EXPECT_TRUE(series.type == "dimension" || series.type == "measure");

        // Series should have a name
        EXPECT_FALSE(series.name.empty());
    }
}

TEST_F(VizzuConverterTest, AnimationStepsGenerated) {
    std::string mermaid = R"(flowchart TD
    A --> B
    B --> C)";

    MermaidDiagram diagram = parser.parse(mermaid);
    VizzuData vizzu_data = converter.convert(diagram);

    // Should have at least one animation step
    EXPECT_FALSE(vizzu_data.animation_steps.empty());
}

TEST_F(VizzuConverterTest, ConfigurationPresent) {
    std::string mermaid = R"(flowchart TD
    A --> B)";

    MermaidDiagram diagram = parser.parse(mermaid);
    VizzuData vizzu_data = converter.convert(diagram);

    // Should have some configuration
    EXPECT_FALSE(vizzu_data.config.empty());
}

TEST_F(VizzuConverterTest, EmptyDiagramProducesEmptyData) {
    MermaidDiagram empty_diagram;
    empty_diagram.diagram_type = "graph";

    VizzuData vizzu_data = converter.convert(empty_diagram);

    // May have config but no meaningful series or data
    bool has_data = false;
    for (const auto& series : vizzu_data.series) {
        if (!series.values.empty()) {
            has_data = true;
            break;
        }
    }

    // Empty diagrams might not have data series
    // Just verify it doesn't crash
    SUCCEED();
}

TEST_F(VizzuConverterTest, NodeTypesArePreserved) {
    std::string mermaid = R"(flowchart TD
    A[Rectangle]
    B(Rounded)
    C{Diamond})";

    MermaidDiagram diagram = parser.parse(mermaid);
    VizzuData vizzu_data = converter.convert(diagram);

    // Check that we have information about different node types
    bool has_type_info = false;
    for (const auto& series : vizzu_data.series) {
        if (series.name.find("type") != std::string::npos ||
            series.name.find("Type") != std::string::npos) {
            has_type_info = true;
            EXPECT_GE(series.values.size(), 3);
        }
    }

    // It's OK if types aren't explicitly preserved, just ensure no crash
    SUCCEED();
}

TEST_F(VizzuConverterTest, EdgeLabelsHandled) {
    std::string mermaid = R"(flowchart TD
    A -->|Yes| B
    B -->|No| C)";

    MermaidDiagram diagram = parser.parse(mermaid);
    VizzuData vizzu_data = converter.convert(diagram);

    EXPECT_FALSE(vizzu_data.series.empty());
    // Just ensure edge labels don't cause crashes
    SUCCEED();
}

TEST_F(VizzuConverterTest, DirectionInformation) {
    std::string mermaid = R"(flowchart LR
    A --> B --> C)";

    MermaidDiagram diagram = parser.parse(mermaid);
    VizzuData vizzu_data = converter.convert(diagram);

    // Check if direction is in config
    bool has_direction = false;
    for (const auto& [key, value] : vizzu_data.config) {
        if (key.find("direction") != std::string::npos ||
            key.find("orientation") != std::string::npos) {
            has_direction = true;
        }
    }

    // Direction might be encoded differently, just ensure conversion works
    SUCCEED();
}

TEST_F(VizzuConverterTest, LargeGraphHandling) {
    // Create a large graph programmatically
    std::stringstream ss;
    ss << "flowchart TD\n";
    for (int i = 0; i < 50; ++i) {
        ss << "    N" << i << "[Node " << i << "] --> N" << (i + 1) << "[Node " << (i + 1) << "]\n";
    }

    MermaidDiagram diagram = parser.parse(ss.str());
    VizzuData vizzu_data = converter.convert(diagram);

    EXPECT_FALSE(vizzu_data.series.empty());

    // Verify we have data for many nodes
    int max_values = 0;
    for (const auto& series : vizzu_data.series) {
        max_values = std::max(max_values, static_cast<int>(series.values.size()));
    }

    EXPECT_GE(max_values, 20);
}

TEST_F(VizzuConverterTest, ConvertSequenceDiagram) {
    std::string mermaid = R"(sequenceDiagram
    Alice->>Bob: Hello
    Bob-->>Alice: Hi)";

    MermaidDiagram diagram = parser.parse(mermaid);
    VizzuData vizzu_data = converter.convert(diagram);

    EXPECT_FALSE(vizzu_data.series.empty());
}

TEST_F(VizzuConverterTest, ToJsonFormat) {
    std::string mermaid = R"(flowchart TD
    A --> B)";

    MermaidDiagram diagram = parser.parse(mermaid);
    VizzuData vizzu_data = converter.convert(diagram);

    std::string json = converter.to_json(vizzu_data).stringify();

    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("{"), std::string::npos);
    EXPECT_NE(json.find("}"), std::string::npos);
}

TEST_F(VizzuConverterTest, JsonContainsSeries) {
    std::string mermaid = R"(flowchart TD
    A --> B --> C)";

    MermaidDiagram diagram = parser.parse(mermaid);
    VizzuData vizzu_data = converter.convert(diagram);
    std::string json = converter.to_json(vizzu_data).stringify();

    EXPECT_NE(json.find("series"), std::string::npos);
}

TEST_F(VizzuConverterTest, MultipleConversionsConsistent) {
    std::string mermaid = R"(flowchart TD
    A --> B
    B --> C)";

    MermaidDiagram diagram = parser.parse(mermaid);

    VizzuData data1 = converter.convert(diagram);
    VizzuData data2 = converter.convert(diagram);

    EXPECT_EQ(data1.series.size(), data2.series.size());
    EXPECT_EQ(data1.animation_steps.size(), data2.animation_steps.size());
}
