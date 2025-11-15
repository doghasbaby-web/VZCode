#include <gtest/gtest.h>
#include "../src/parser/mermaid_parser.hpp"

using namespace vzcode::parser;

class MermaidParserTest : public ::testing::Test {
protected:
    MermaidParser parser;

    void SetUp() override {
        parser = MermaidParser();
    }
};

// Test Basic Parsing
TEST_F(MermaidParserTest, ParseSimpleFlowchart) {
    std::string mermaid = R"(flowchart TD
    A[Start] --> B[Process]
    B --> C[End])";

    MermaidDiagram diagram = parser.parse(mermaid);

    EXPECT_EQ(diagram.diagram_type, "flowchart");
    EXPECT_EQ(diagram.direction, "TD");
    EXPECT_EQ(diagram.nodes.size(), 3);
    EXPECT_EQ(diagram.edges.size(), 2);
}

TEST_F(MermaidParserTest, ParseGraphLR) {
    std::string mermaid = R"(graph LR
    A --> B)";

    MermaidDiagram diagram = parser.parse(mermaid);

    EXPECT_EQ(diagram.diagram_type, "graph");
    EXPECT_EQ(diagram.direction, "LR");
}

TEST_F(MermaidParserTest, ParseNodeTypes) {
    std::string mermaid = R"(graph TD
    A[Rectangle]
    B(Rounded)
    C{Diamond}
    D((Circle)))";

    MermaidDiagram diagram = parser.parse(mermaid);

    EXPECT_EQ(diagram.nodes.size(), 4);

    // Find each node and check its type
    for (const auto& node : diagram.nodes) {
        if (node.id == "A") {
            EXPECT_EQ(node.type, "rectangle");
            EXPECT_EQ(node.label, "Rectangle");
        } else if (node.id == "B") {
            EXPECT_EQ(node.type, "rounded");
            EXPECT_EQ(node.label, "Rounded");
        } else if (node.id == "C") {
            EXPECT_EQ(node.type, "rhombus");
            EXPECT_EQ(node.label, "Diamond");
        } else if (node.id == "D") {
            EXPECT_EQ(node.type, "circle");
            EXPECT_EQ(node.label, "Circle");
        }
    }
}

TEST_F(MermaidParserTest, ParseEdgeTypes) {
    std::string mermaid = R"(graph TD
    A --> B
    B -.-> C
    C ==> D)";

    MermaidDiagram diagram = parser.parse(mermaid);

    EXPECT_EQ(diagram.edges.size(), 3);

    for (const auto& edge : diagram.edges) {
        if (edge.from == "A" && edge.to == "B") {
            EXPECT_EQ(edge.type, "solid");
        } else if (edge.from == "B" && edge.to == "C") {
            EXPECT_EQ(edge.type, "dotted");
        } else if (edge.from == "C" && edge.to == "D") {
            EXPECT_EQ(edge.type, "thick");
        }
    }
}

TEST_F(MermaidParserTest, ParseEdgeLabels) {
    std::string mermaid = R"(graph TD
    A -->|Yes| B
    B -->|No| C)";

    MermaidDiagram diagram = parser.parse(mermaid);

    EXPECT_EQ(diagram.edges.size(), 2);

    for (const auto& edge : diagram.edges) {
        if (edge.from == "A" && edge.to == "B") {
            EXPECT_EQ(edge.label, "Yes");
        } else if (edge.from == "B" && edge.to == "C") {
            EXPECT_EQ(edge.label, "No");
        }
    }
}

TEST_F(MermaidParserTest, ParseClassDiagram) {
    std::string mermaid = R"(classDiagram
    ClassA --> ClassB
    ClassB <|-- ClassC)";

    MermaidDiagram diagram = parser.parse(mermaid);

    EXPECT_EQ(diagram.diagram_type, "classDiagram");
    EXPECT_EQ(diagram.edges.size(), 2);
}

TEST_F(MermaidParserTest, ParseSequenceDiagram) {
    std::string mermaid = R"(sequenceDiagram
    Alice->>Bob: Hello
    Bob-->>Alice: Hi)";

    MermaidDiagram diagram = parser.parse(mermaid);

    EXPECT_EQ(diagram.diagram_type, "sequenceDiagram");
    EXPECT_EQ(diagram.edges.size(), 2);
}

TEST_F(MermaidParserTest, SkipComments) {
    std::string mermaid = R"(graph TD
    %% This is a comment
    A --> B
    %% Another comment
    B --> C)";

    MermaidDiagram diagram = parser.parse(mermaid);

    EXPECT_EQ(diagram.edges.size(), 2);
}

TEST_F(MermaidParserTest, HandleWhitespace) {
    std::string mermaid = R"(graph TD
        A   -->   B
        B   -->   C  )";

    MermaidDiagram diagram = parser.parse(mermaid);

    EXPECT_EQ(diagram.edges.size(), 2);
}

TEST_F(MermaidParserTest, ExtractNodeId) {
    EXPECT_EQ(parser.extract_node_id("A[Label]"), "A");
    EXPECT_EQ(parser.extract_node_id("B(Label)"), "B");
    EXPECT_EQ(parser.extract_node_id("C{Label}"), "C");
    EXPECT_EQ(parser.extract_node_id("SimpleNode"), "SimpleNode");
}

TEST_F(MermaidParserTest, ExtractNodeLabel) {
    EXPECT_EQ(parser.extract_node_label("A[My Label]"), "My Label");
    EXPECT_EQ(parser.extract_node_label("B(Round Label)"), "Round Label");
    EXPECT_EQ(parser.extract_node_label("C{Decision}"), "Decision");
}

TEST_F(MermaidParserTest, DetermineNodeType) {
    EXPECT_EQ(parser.determine_node_type("A[Label]"), "rectangle");
    EXPECT_EQ(parser.determine_node_type("B(Label)"), "rounded");
    EXPECT_EQ(parser.determine_node_type("C{Label}"), "rhombus");
    EXPECT_EQ(parser.determine_node_type("D((Label))"), "circle");
}

TEST_F(MermaidParserTest, SupportedTypes) {
    std::vector<std::string> types = parser.supported_types();

    EXPECT_NE(std::find(types.begin(), types.end(), "flowchart"), types.end());
    EXPECT_NE(std::find(types.begin(), types.end(), "graph"), types.end());
    EXPECT_NE(std::find(types.begin(), types.end(), "classDiagram"), types.end());
    EXPECT_NE(std::find(types.begin(), types.end(), "sequenceDiagram"), types.end());
}

TEST_F(MermaidParserTest, ValidateMermaid) {
    std::string valid = R"(graph TD
        A --> B)";

    std::string error_msg;
    EXPECT_TRUE(parser.validate(valid, error_msg));
    EXPECT_TRUE(error_msg.empty());
}

TEST_F(MermaidParserTest, ValidateInvalidMermaid) {
    std::string invalid = "";

    std::string error_msg;
    EXPECT_FALSE(parser.validate(invalid, error_msg));
    EXPECT_FALSE(error_msg.empty());
}

TEST_F(MermaidParserTest, ComplexFlowchart) {
    std::string mermaid = R"(flowchart TD
    Start[Start] --> Input[Get Input]
    Input --> Process{Is Valid?}
    Process -->|Yes| Save[Save Data]
    Process -->|No| Error[Show Error]
    Save --> End[End]
    Error --> Input)";

    MermaidDiagram diagram = parser.parse(mermaid);

    EXPECT_EQ(diagram.diagram_type, "flowchart");
    EXPECT_EQ(diagram.direction, "TD");
    EXPECT_GE(diagram.nodes.size(), 5);
    EXPECT_EQ(diagram.edges.size(), 6);
}

TEST_F(MermaidParserTest, MultipleEdgesFromSameNode) {
    std::string mermaid = R"(graph TD
    A --> B
    A --> C
    A --> D)";

    MermaidDiagram diagram = parser.parse(mermaid);

    int edges_from_a = 0;
    for (const auto& edge : diagram.edges) {
        if (edge.from == "A") {
            edges_from_a++;
        }
    }

    EXPECT_EQ(edges_from_a, 3);
}

TEST_F(MermaidParserTest, MultipleEdgesToSameNode) {
    std::string mermaid = R"(graph TD
    A --> D
    B --> D
    C --> D)";

    MermaidDiagram diagram = parser.parse(mermaid);

    int edges_to_d = 0;
    for (const auto& edge : diagram.edges) {
        if (edge.to == "D") {
            edges_to_d++;
        }
    }

    EXPECT_EQ(edges_to_d, 3);
}

TEST_F(MermaidParserTest, NodeWithLongLabel) {
    std::string mermaid = R"(graph TD
    A[This is a very long label with many words in it] --> B[Short])";

    MermaidDiagram diagram = parser.parse(mermaid);

    bool found = false;
    for (const auto& node : diagram.nodes) {
        if (node.id == "A") {
            EXPECT_EQ(node.label, "This is a very long label with many words in it");
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(MermaidParserTest, EmptyDiagramThrows) {
    EXPECT_THROW(parser.parse(""), std::exception);
}

TEST_F(MermaidParserTest, OnlyCommentsThrows) {
    std::string mermaid = R"(%% Just comments
    %% Nothing else)";

    EXPECT_THROW(parser.parse(mermaid), std::exception);
}

TEST_F(MermaidParserTest, DirectionVariations) {
    std::vector<std::string> directions = {"TD", "TB", "BT", "RL", "LR"};

    for (const auto& dir : directions) {
        std::string mermaid = "graph " + dir + "\nA --> B";
        MermaidDiagram diagram = parser.parse(mermaid);
        EXPECT_EQ(diagram.direction, dir);
    }
}

TEST_F(MermaidParserTest, NodeUpdateWithEdge) {
    // If a node is defined first, then appears in an edge with more info,
    // it should be updated
    std::string mermaid = R"(graph TD
    A
    A[Start Node] --> B[End Node])";

    MermaidDiagram diagram = parser.parse(mermaid);

    bool found = false;
    for (const auto& node : diagram.nodes) {
        if (node.id == "A") {
            EXPECT_EQ(node.label, "Start Node");
            found = true;
        }
    }
    EXPECT_TRUE(found);
}
