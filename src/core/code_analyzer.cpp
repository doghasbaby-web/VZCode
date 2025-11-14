#include "code_analyzer.hpp"
#include <algorithm>
#include <sstream>

namespace vzcode {
namespace core {

CodeAnalyzer::CodeAnalyzer(const std::string& gemini_api_key)
    : gemini_client_(std::make_unique<GeminiClient>(gemini_api_key)) {}

CodeAnalyzer::~CodeAnalyzer() {}

std::string CodeAnalyzer::preprocess_code(const std::string& code, const AnalysisOptions& options) {
    std::string processed = code;

    if (!options.include_comments) {
        // Simple comment removal (C++ style)
        std::stringstream ss;
        bool in_multiline_comment = false;
        bool in_single_line_comment = false;

        for (size_t i = 0; i < processed.length(); ++i) {
            if (!in_multiline_comment && !in_single_line_comment) {
                if (i + 1 < processed.length() && processed[i] == '/' && processed[i + 1] == '*') {
                    in_multiline_comment = true;
                    ++i;
                    continue;
                } else if (i + 1 < processed.length() && processed[i] == '/' && processed[i + 1] == '/') {
                    in_single_line_comment = true;
                    ++i;
                    continue;
                }
            }

            if (in_multiline_comment) {
                if (i + 1 < processed.length() && processed[i] == '*' && processed[i + 1] == '/') {
                    in_multiline_comment = false;
                    ++i;
                }
                continue;
            }

            if (in_single_line_comment) {
                if (processed[i] == '\n') {
                    in_single_line_comment = false;
                    ss << '\n';
                }
                continue;
            }

            ss << processed[i];
        }

        processed = ss.str();
    }

    return processed;
}

std::vector<std::string> CodeAnalyzer::extract_insights(const std::string& description) {
    std::vector<std::string> insights;

    // Split description into lines and extract key points
    std::stringstream ss(description);
    std::string line;

    while (std::getline(ss, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (!line.empty() && (line.find("shows") != std::string::npos ||
                             line.find("represents") != std::string::npos ||
                             line.find("illustrates") != std::string::npos)) {
            insights.push_back(line);
        }
    }

    return insights;
}

std::string CodeAnalyzer::merge_multiple_diagrams(const std::vector<std::string>& diagrams) {
    if (diagrams.empty()) return "";
    if (diagrams.size() == 1) return diagrams[0];

    // For now, concatenate diagrams with subgraphs
    std::stringstream ss;
    ss << "graph TB\n";

    for (size_t i = 0; i < diagrams.size(); ++i) {
        ss << "  subgraph File" << (i + 1) << "\n";

        // Extract content from each diagram
        std::stringstream diagram_ss(diagrams[i]);
        std::string line;
        bool skip_first = true;

        while (std::getline(diagram_ss, line)) {
            if (skip_first && (line.find("graph") != std::string::npos ||
                              line.find("flowchart") != std::string::npos)) {
                skip_first = false;
                continue;
            }
            if (!line.empty()) {
                ss << "    " << line << "\n";
            }
        }

        ss << "  end\n";
    }

    return ss.str();
}

CodeAnalyzer::AnalysisOutput CodeAnalyzer::analyze_code(const std::string& code,
                                                        const AnalysisOptions& options) {
    AnalysisOutput output;

    std::string processed_code = preprocess_code(code, options);

    GeminiClient::AnalysisResult gemini_result =
        gemini_client_->generate_visualization(processed_code, options.visualization_type, options.language);

    output.success = gemini_result.success;
    output.mermaid_diagram = gemini_result.mermaid_diagram;
    output.description = gemini_result.description;
    output.error_message = gemini_result.error_message;

    if (output.success) {
        output.insights = extract_insights(output.description);
    }

    return output;
}

CodeAnalyzer::AnalysisOutput CodeAnalyzer::analyze_file(const CodeFile& file,
                                                        const AnalysisOptions& options) {
    AnalysisOptions file_options = options;
    if (!file.language.empty()) {
        file_options.language = file.language;
    }

    return analyze_code(file.content, file_options);
}

CodeAnalyzer::AnalysisOutput CodeAnalyzer::analyze_project(const std::vector<CodeFile>& files,
                                                           const AnalysisOptions& options) {
    AnalysisOutput output;

    if (files.empty()) {
        output.success = false;
        output.error_message = "No files to analyze";
        return output;
    }

    std::vector<std::string> diagrams;
    std::vector<std::string> all_descriptions;

    for (const auto& file : files) {
        AnalysisOutput file_output = analyze_file(file, options);

        if (file_output.success) {
            diagrams.push_back(file_output.mermaid_diagram);
            all_descriptions.push_back(file_output.description);

            for (const auto& insight : file_output.insights) {
                output.insights.push_back(insight);
            }
        }
    }

    if (diagrams.empty()) {
        output.success = false;
        output.error_message = "Failed to analyze any files";
        return output;
    }

    output.success = true;
    output.mermaid_diagram = merge_multiple_diagrams(diagrams);

    // Combine descriptions
    std::stringstream desc_ss;
    desc_ss << "Analysis of " << files.size() << " file(s):\n\n";
    for (size_t i = 0; i < all_descriptions.size(); ++i) {
        desc_ss << "File " << (i + 1) << ":\n" << all_descriptions[i] << "\n\n";
    }
    output.description = desc_ss.str();

    return output;
}

} // namespace core
} // namespace vzcode
