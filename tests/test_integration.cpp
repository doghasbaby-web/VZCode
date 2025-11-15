#include <gtest/gtest.h>
#include "../src/parser/mermaid_parser.hpp"
#include "../src/parser/vizzu_converter.hpp"
#include "../src/core/code_analyzer.hpp"
#include "../src/utils/json_utils.hpp"

using namespace vzcode;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup for integration tests
    }
};

// Test Mermaid Parser -> Vizzu Converter pipeline
TEST_F(IntegrationTest, MermaidToVizzuPipeline) {
    parser::MermaidParser parser;
    parser::VizzuConverter converter;

    std::string mermaid = R"(flowchart TD
    Start[Start] --> Process[Process Data]
    Process --> Decision{Is Valid?}
    Decision -->|Yes| Save[Save]
    Decision -->|No| Error[Error]
    Save --> End[End]
    Error --> End)";

    // Step 1: Parse Mermaid
    parser::MermaidDiagram diagram = parser.parse(mermaid);

    EXPECT_EQ(diagram.diagram_type, "flowchart");
    EXPECT_EQ(diagram.direction, "TD");
    EXPECT_GE(diagram.nodes.size(), 5);
    EXPECT_GE(diagram.edges.size(), 5);

    // Step 2: Convert to Vizzu
    parser::VizzuData vizzu_data = converter.convert(diagram);

    EXPECT_FALSE(vizzu_data.series.empty());
    EXPECT_FALSE(vizzu_data.animation_steps.empty());

    // Step 3: Generate JSON
    utils::JsonValue json = converter.to_json(vizzu_data);
    std::string json_str = json.stringify();

    EXPECT_FALSE(json_str.empty());
    EXPECT_NE(json_str.find("series"), std::string::npos);
}

TEST_F(IntegrationTest, ClassDiagramFullPipeline) {
    parser::MermaidParser parser;
    parser::VizzuConverter converter;

    std::string mermaid = R"(classDiagram
    Animal <|-- Dog
    Animal <|-- Cat
    Animal : +name
    Animal : +age
    Dog : +breed
    Cat : +color)";

    parser::MermaidDiagram diagram = parser.parse(mermaid);
    EXPECT_EQ(diagram.diagram_type, "classDiagram");

    parser::VizzuData vizzu_data = converter.convert(diagram);
    EXPECT_FALSE(vizzu_data.series.empty());

    utils::JsonValue json = converter.to_json(vizzu_data);
    EXPECT_FALSE(json.stringify().empty());
}

TEST_F(IntegrationTest, SequenceDiagramFullPipeline) {
    parser::MermaidParser parser;
    parser::VizzuConverter converter;

    std::string mermaid = R"(sequenceDiagram
    Client->>Server: Request
    Server->>Database: Query
    Database-->>Server: Result
    Server-->>Client: Response)";

    parser::MermaidDiagram diagram = parser.parse(mermaid);
    EXPECT_EQ(diagram.diagram_type, "sequenceDiagram");

    parser::VizzuData vizzu_data = converter.convert(diagram);
    EXPECT_FALSE(vizzu_data.series.empty());
}

TEST_F(IntegrationTest, CodeAnalyzerPreprocessing) {
    core::CodeAnalyzer analyzer("test-key");

    std::string code = R"(
    // Main function
    int main() {
        /* Initialize variables */
        int x = 10;
        return 0; // Return success
    }
    )";

    core::CodeAnalyzer::AnalysisOptions options;
    options.include_comments = false;

    std::string processed = analyzer.preprocess_code(code, options);

    // Verify comments removed
    EXPECT_EQ(processed.find("Main function"), std::string::npos);
    EXPECT_EQ(processed.find("Initialize variables"), std::string::npos);
    EXPECT_EQ(processed.find("Return success"), std::string::npos);

    // Verify code preserved
    EXPECT_NE(processed.find("int main()"), std::string::npos);
    EXPECT_NE(processed.find("int x = 10;"), std::string::npos);
}

TEST_F(IntegrationTest, JsonUtilsRoundTrip) {
    // Create complex JSON structure
    utils::JsonValue obj = utils::JsonValue::object();
    obj["name"] = utils::JsonValue("VZCode");
    obj["version"] = utils::JsonValue(1.0);
    obj["active"] = utils::JsonValue(true);

    utils::JsonValue features = utils::JsonValue::array();
    features.push_back(utils::JsonValue("visualization"));
    features.push_back(utils::JsonValue("code-analysis"));
    obj["features"] = features;

    // Stringify
    std::string json_str = obj.stringify();

    // Parse back
    utils::JsonValue parsed = utils::JsonValue::parse(json_str);

    // Verify
    EXPECT_EQ(parsed["name"].as_string(), "VZCode");
    EXPECT_DOUBLE_EQ(parsed["version"].as_number(), 1.0);
    EXPECT_TRUE(parsed["active"].as_bool());
    EXPECT_EQ(parsed["features"].array_size(), 2);
}

TEST_F(IntegrationTest, MultipleFileAnalysis) {
    core::CodeAnalyzer analyzer("test-key");

    core::CodeAnalyzer::CodeFile file1;
    file1.path = "file1.cpp";
    file1.content = "int add(int a, int b) { return a + b; }";
    file1.language = "cpp";

    core::CodeAnalyzer::CodeFile file2;
    file2.path = "file2.cpp";
    file2.content = "int multiply(int a, int b) { return a * b; }";
    file2.language = "cpp";

    std::vector<core::CodeAnalyzer::CodeFile> files = {file1, file2};

    // Note: This would normally call Gemini API, but we're testing the structure
    // In real scenarios, you'd mock the API or use a test API key
}

TEST_F(IntegrationTest, VizzuDataSerialization) {
    parser::VizzuData data;

    // Add series
    parser::VizzuSeries series1;
    series1.name = "nodes";
    series1.type = "dimension";
    series1.values = {"A", "B", "C"};
    data.series.push_back(series1);

    parser::VizzuSeries series2;
    series2.name = "values";
    series2.type = "measure";
    series2.values = {"1", "2", "3"};
    data.series.push_back(series2);

    // Add animation step
    parser::VizzuAnimationConfig anim;
    anim.x = "nodes";
    anim.y = "values";
    anim.geometry = "circle";
    anim.duration_ms = 1000;
    data.animation_steps.push_back(anim);

    // Add config
    data.config["title"] = "Test Visualization";

    // Convert to JSON
    parser::VizzuConverter converter;
    utils::JsonValue json = converter.to_json(data);

    std::string json_str = json.stringify();

    EXPECT_FALSE(json_str.empty());
    EXPECT_NE(json_str.find("nodes"), std::string::npos);
    EXPECT_NE(json_str.find("values"), std::string::npos);
}

TEST_F(IntegrationTest, MermaidValidationBeforeConversion) {
    parser::MermaidParser parser;
    parser::VizzuConverter converter;

    std::string valid_mermaid = R"(flowchart TD
        A --> B)";

    std::string invalid_mermaid = "";

    // Validate before parsing
    std::string error_msg;
    EXPECT_TRUE(parser.validate(valid_mermaid, error_msg));

    std::string error_msg2;
    EXPECT_FALSE(parser.validate(invalid_mermaid, error_msg2));
    EXPECT_FALSE(error_msg2.empty());
}

TEST_F(IntegrationTest, ComplexNestedJsonParsing) {
    std::string complex_json = R"({
        "visualization": {
            "type": "flowchart",
            "data": {
                "nodes": [
                    {"id": "A", "label": "Start"},
                    {"id": "B", "label": "End"}
                ],
                "edges": [
                    {"from": "A", "to": "B", "weight": 1.5}
                ]
            },
            "config": {
                "animation": {
                    "duration": 1000,
                    "easing": "linear"
                }
            }
        }
    })";

    utils::JsonValue json = utils::JsonValue::parse(complex_json);

    EXPECT_EQ(json["visualization"]["type"].as_string(), "flowchart");
    EXPECT_EQ(json["visualization"]["data"]["nodes"].array_size(), 2);
    EXPECT_EQ(json["visualization"]["data"]["nodes"][0]["id"].as_string(), "A");
    EXPECT_DOUBLE_EQ(json["visualization"]["data"]["edges"][0]["weight"].as_number(), 1.5);
    EXPECT_DOUBLE_EQ(json["visualization"]["config"]["animation"]["duration"].as_number(), 1000.0);
}

TEST_F(IntegrationTest, MermaidParserEdgeCases) {
    parser::MermaidParser parser;

    // Test with extra whitespace
    std::string mermaid1 = R"(
        flowchart    TD
            A    -->    B
            B    -->    C
    )";

    parser::MermaidDiagram diagram1 = parser.parse(mermaid1);
    EXPECT_GE(diagram1.edges.size(), 2);

    // Test with comments
    std::string mermaid2 = R"(flowchart TD
        %% This is a comment
        A --> B
        %% Another comment)";

    parser::MermaidDiagram diagram2 = parser.parse(mermaid2);
    EXPECT_EQ(diagram2.edges.size(), 1);
}

TEST_F(IntegrationTest, VizzuConverterAnimationConfig) {
    parser::VizzuConverter converter;

    converter.set_animation_duration(2000);
    converter.set_default_geometry("rectangle");

    parser::MermaidParser parser;
    std::string mermaid = "flowchart TD\nA --> B";

    parser::MermaidDiagram diagram = parser.parse(mermaid);
    parser::VizzuData data = converter.convert(diagram);

    // Verify animation configuration is applied
    bool has_animation = !data.animation_steps.empty();
    EXPECT_TRUE(has_animation);
}

TEST_F(IntegrationTest, ErrorHandlingInPipeline) {
    parser::MermaidParser parser;

    // Test invalid mermaid
    std::string invalid = "not a valid mermaid diagram";

    // Should throw or return error
    EXPECT_THROW(parser.parse(invalid), std::exception);
}

TEST_F(IntegrationTest, LargeDataHandling) {
    // Create a large mermaid diagram
    std::stringstream ss;
    ss << "flowchart TD\n";

    for (int i = 0; i < 100; ++i) {
        ss << "    N" << i << "[Node " << i << "] --> N" << (i + 1) << "[Node " << (i + 1) << "]\n";
    }

    parser::MermaidParser parser;
    parser::VizzuConverter converter;

    std::string large_mermaid = ss.str();

    parser::MermaidDiagram diagram = parser.parse(large_mermaid);
    EXPECT_GE(diagram.nodes.size(), 100);
    EXPECT_GE(diagram.edges.size(), 100);

    parser::VizzuData data = converter.convert(diagram);
    EXPECT_FALSE(data.series.empty());

    utils::JsonValue json = converter.to_json(data);
    std::string json_str = json.stringify();

    EXPECT_GT(json_str.length(), 1000);
}
