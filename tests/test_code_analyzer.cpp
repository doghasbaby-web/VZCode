#include <gtest/gtest.h>
#include "../src/core/code_analyzer.hpp"

using namespace vzcode::core;

class CodeAnalyzerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Note: These tests don't require a real API key for preprocessing tests
    }
};

TEST_F(CodeAnalyzerTest, PreprocessCodeKeepsCodeWithComments) {
    CodeAnalyzer analyzer("dummy-key");
    CodeAnalyzer::AnalysisOptions options;
    options.include_comments = true;

    std::string code = R"(// This is a comment
int main() {
    /* Block comment */
    return 0;
})";

    std::string processed = analyzer.preprocess_code(code, options);

    EXPECT_NE(processed.find("//"), std::string::npos);
    EXPECT_NE(processed.find("/*"), std::string::npos);
}

TEST_F(CodeAnalyzerTest, PreprocessCodeRemovesComments) {
    CodeAnalyzer analyzer("dummy-key");
    CodeAnalyzer::AnalysisOptions options;
    options.include_comments = false;

    std::string code = R"(// This is a comment
int main() {
    /* Block comment */
    return 0;
})";

    std::string processed = analyzer.preprocess_code(code, options);

    // Comments should be removed
    EXPECT_EQ(processed.find("This is a comment"), std::string::npos);
    EXPECT_EQ(processed.find("Block comment"), std::string::npos);
    // Code should remain
    EXPECT_NE(processed.find("int main()"), std::string::npos);
    EXPECT_NE(processed.find("return 0;"), std::string::npos);
}

TEST_F(CodeAnalyzerTest, PreprocessRemovesSingleLineComments) {
    CodeAnalyzer analyzer("dummy-key");
    CodeAnalyzer::AnalysisOptions options;
    options.include_comments = false;

    std::string code = R"(int x = 5; // Set x to 5
int y = 10; // Set y to 10
// This is a full line comment
int z = 15;)";

    std::string processed = analyzer.preprocess_code(code, options);

    EXPECT_EQ(processed.find("Set x to 5"), std::string::npos);
    EXPECT_EQ(processed.find("Set y to 10"), std::string::npos);
    EXPECT_EQ(processed.find("full line comment"), std::string::npos);
    EXPECT_NE(processed.find("int x = 5;"), std::string::npos);
    EXPECT_NE(processed.find("int y = 10;"), std::string::npos);
    EXPECT_NE(processed.find("int z = 15;"), std::string::npos);
}

TEST_F(CodeAnalyzerTest, PreprocessRemovesMultiLineComments) {
    CodeAnalyzer analyzer("dummy-key");
    CodeAnalyzer::AnalysisOptions options;
    options.include_comments = false;

    std::string code = R"(int main() {
    /* This is a
       multi-line
       comment */
    return 0;
})";

    std::string processed = analyzer.preprocess_code(code, options);

    EXPECT_EQ(processed.find("multi-line"), std::string::npos);
    EXPECT_NE(processed.find("int main()"), std::string::npos);
    EXPECT_NE(processed.find("return 0;"), std::string::npos);
}

TEST_F(CodeAnalyzerTest, PreprocessPreservesNewlines) {
    CodeAnalyzer analyzer("dummy-key");
    CodeAnalyzer::AnalysisOptions options;
    options.include_comments = false;

    std::string code = "line1\n// comment\nline2\nline3";
    std::string processed = analyzer.preprocess_code(code, options);

    // Count newlines
    int newline_count = 0;
    for (char c : processed) {
        if (c == '\n') newline_count++;
    }

    EXPECT_GE(newline_count, 2);
}

TEST_F(CodeAnalyzerTest, ExtractInsightsFindsKeyPhrases) {
    CodeAnalyzer analyzer("dummy-key");

    std::string description = R"(This shows the main execution flow.
The diagram represents the class hierarchy.
It illustrates the interaction between components.
Random text without keywords.)";

    std::vector<std::string> insights = analyzer.extract_insights(description);

    EXPECT_GE(insights.size(), 3);

    bool found_shows = false;
    bool found_represents = false;
    bool found_illustrates = false;

    for (const auto& insight : insights) {
        if (insight.find("shows") != std::string::npos) found_shows = true;
        if (insight.find("represents") != std::string::npos) found_represents = true;
        if (insight.find("illustrates") != std::string::npos) found_illustrates = true;
    }

    EXPECT_TRUE(found_shows);
    EXPECT_TRUE(found_represents);
    EXPECT_TRUE(found_illustrates);
}

TEST_F(CodeAnalyzerTest, ExtractInsightsIgnoresEmptyLines) {
    CodeAnalyzer analyzer("dummy-key");

    std::string description = "\n\nThis shows something.\n\n\n";
    std::vector<std::string> insights = analyzer.extract_insights(description);

    EXPECT_EQ(insights.size(), 1);
}

TEST_F(CodeAnalyzerTest, ExtractInsightsTrimsWhitespace) {
    CodeAnalyzer analyzer("dummy-key");

    std::string description = "   This shows the flow.   \n  This represents data.  ";
    std::vector<std::string> insights = analyzer.extract_insights(description);

    for (const auto& insight : insights) {
        // Check no leading/trailing whitespace
        EXPECT_NE(insight[0], ' ');
        EXPECT_NE(insight[insight.length() - 1], ' ');
    }
}

TEST_F(CodeAnalyzerTest, MergeEmptyDiagrams) {
    CodeAnalyzer analyzer("dummy-key");

    std::vector<std::string> diagrams;
    std::string merged = analyzer.merge_multiple_diagrams(diagrams);

    EXPECT_TRUE(merged.empty());
}

TEST_F(CodeAnalyzerTest, MergeSingleDiagram) {
    CodeAnalyzer analyzer("dummy-key");

    std::vector<std::string> diagrams = {
        "graph TD\nA --> B"
    };

    std::string merged = analyzer.merge_multiple_diagrams(diagrams);

    EXPECT_EQ(merged, diagrams[0]);
}

TEST_F(CodeAnalyzerTest, MergeMultipleDiagrams) {
    CodeAnalyzer analyzer("dummy-key");

    std::vector<std::string> diagrams = {
        "graph TD\nA --> B",
        "graph TD\nC --> D",
        "graph TD\nE --> F"
    };

    std::string merged = analyzer.merge_multiple_diagrams(diagrams);

    EXPECT_NE(merged.find("graph TB"), std::string::npos);
    EXPECT_NE(merged.find("subgraph"), std::string::npos);
    EXPECT_NE(merged.find("File1"), std::string::npos);
    EXPECT_NE(merged.find("File2"), std::string::npos);
    EXPECT_NE(merged.find("File3"), std::string::npos);
}

TEST_F(CodeAnalyzerTest, MergeDiagramsHandlesFlowchart) {
    CodeAnalyzer analyzer("dummy-key");

    std::vector<std::string> diagrams = {
        "flowchart TD\nA --> B",
        "flowchart LR\nC --> D"
    };

    std::string merged = analyzer.merge_multiple_diagrams(diagrams);

    EXPECT_NE(merged.find("subgraph"), std::string::npos);
}

TEST_F(CodeAnalyzerTest, AnalysisOptionsDefaults) {
    CodeAnalyzer::AnalysisOptions options;

    EXPECT_EQ(options.visualization_type, "flowchart");
    EXPECT_EQ(options.language, "cpp");
    EXPECT_TRUE(options.include_comments);
}

TEST_F(CodeAnalyzerTest, AnalysisOptionsCustomization) {
    CodeAnalyzer::AnalysisOptions options;
    options.visualization_type = "class-diagram";
    options.language = "python";
    options.include_comments = false;

    EXPECT_EQ(options.visualization_type, "class-diagram");
    EXPECT_EQ(options.language, "python");
    EXPECT_FALSE(options.include_comments);
}

TEST_F(CodeAnalyzerTest, CodeFileStructure) {
    CodeAnalyzer::CodeFile file;
    file.path = "test.cpp";
    file.content = "int main() { return 0; }";
    file.language = "cpp";

    EXPECT_EQ(file.path, "test.cpp");
    EXPECT_FALSE(file.content.empty());
    EXPECT_EQ(file.language, "cpp");
}

TEST_F(CodeAnalyzerTest, AnalysisOutputStructure) {
    CodeAnalyzer::AnalysisOutput output;
    output.success = true;
    output.mermaid_diagram = "graph TD\nA --> B";
    output.description = "Test description";

    EXPECT_TRUE(output.success);
    EXPECT_FALSE(output.mermaid_diagram.empty());
    EXPECT_FALSE(output.description.empty());
}

TEST_F(CodeAnalyzerTest, CommentRemovalWithNestedComments) {
    CodeAnalyzer analyzer("dummy-key");
    CodeAnalyzer::AnalysisOptions options;
    options.include_comments = false;

    std::string code = R"(
    /* Outer comment
    // This looks like a comment but is inside a block comment
    Still in block comment */
    int x = 5;
    )";

    std::string processed = analyzer.preprocess_code(code, options);

    // The entire block comment should be removed
    EXPECT_EQ(processed.find("Outer comment"), std::string::npos);
    EXPECT_EQ(processed.find("This looks like"), std::string::npos);
    EXPECT_NE(processed.find("int x = 5"), std::string::npos);
}

TEST_F(CodeAnalyzerTest, EmptyCodeHandling) {
    CodeAnalyzer analyzer("dummy-key");
    CodeAnalyzer::AnalysisOptions options;

    std::string empty = "";
    std::string processed = analyzer.preprocess_code(empty, options);

    EXPECT_TRUE(processed.empty());
}

TEST_F(CodeAnalyzerTest, CodeWithOnlyComments) {
    CodeAnalyzer analyzer("dummy-key");
    CodeAnalyzer::AnalysisOptions options;
    options.include_comments = false;

    std::string code = R"(// Comment 1
// Comment 2
/* Block comment */)";

    std::string processed = analyzer.preprocess_code(code, options);

    // Should be mostly empty after comment removal
    // May have newlines
    for (char c : processed) {
        EXPECT_TRUE(c == '\n' || c == ' ' || c == '\t' || c == '\r');
    }
}

TEST_F(CodeAnalyzerTest, MergeDiagramsWithEmptyContent) {
    CodeAnalyzer analyzer("dummy-key");

    std::vector<std::string> diagrams = {
        "graph TD\nA --> B",
        "",
        "graph TD\nC --> D"
    };

    std::string merged = analyzer.merge_multiple_diagrams(diagrams);

    // Should handle empty diagrams gracefully
    EXPECT_FALSE(merged.empty());
}
