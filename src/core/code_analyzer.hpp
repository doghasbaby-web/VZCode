#ifndef CODE_ANALYZER_HPP
#define CODE_ANALYZER_HPP

#include <string>
#include <vector>
#include <memory>
#include "gemini_client.hpp"

// Forward declare FRIEND_TEST for test access
#ifndef FRIEND_TEST
#define FRIEND_TEST(test_case_name, test_name)
#endif

namespace vzcode {
namespace core {

/**
 * High-level code analysis coordinator
 * Manages the analysis pipeline from code to visualization
 */
class CodeAnalyzer {
public:
    struct AnalysisOptions {
        std::string language = "cpp";
        std::string visualization_type = "flowchart"; // flowchart, class-diagram, sequence, component
        bool include_comments = true;
        bool analyze_dependencies = true;
        int max_depth = 3;
    };

    struct CodeFile {
        std::string path;
        std::string content;
        std::string language;
    };

    struct AnalysisOutput {
        bool success;
        std::string mermaid_diagram;
        std::string description;
        std::vector<std::string> insights;
        std::string error_message;
    };

    explicit CodeAnalyzer(const std::string& gemini_api_key);
    ~CodeAnalyzer();

    /**
     * Analyze a single code file
     */
    AnalysisOutput analyze_file(const CodeFile& file,
                                 const AnalysisOptions& options);
    AnalysisOutput analyze_file(const CodeFile& file);

    /**
     * Analyze multiple code files and generate a comprehensive diagram
     */
    AnalysisOutput analyze_project(const std::vector<CodeFile>& files,
                                   const AnalysisOptions& options);
    AnalysisOutput analyze_project(const std::vector<CodeFile>& files);

    /**
     * Analyze code from string
     */
    AnalysisOutput analyze_code(const std::string& code,
                               const AnalysisOptions& options);
    AnalysisOutput analyze_code(const std::string& code);

    // Public for testing - these are implementation details but heavily tested
    std::string preprocess_code(const std::string& code, const AnalysisOptions& options);
    std::string merge_multiple_diagrams(const std::vector<std::string>& diagrams);
    std::vector<std::string> extract_insights(const std::string& description);

private:
    std::unique_ptr<GeminiClient> gemini_client_;
};

} // namespace core
} // namespace vzcode

#endif // CODE_ANALYZER_HPP
