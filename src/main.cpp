#include "core/code_analyzer.hpp"
#include "parser/mermaid_parser.hpp"
#include "parser/vizzu_converter.hpp"
#include "vizzu/animation_wrapper.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace vzcode;

void print_usage() {
    std::cout << "VZCode - Code Visualization Tool\n";
    std::cout << "Usage: vzcode [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --api-key <key>       Set Gemini API key\n";
    std::cout << "  --file <path>         Analyze code from file\n";
    std::cout << "  --code <code>         Analyze code from string\n";
    std::cout << "  --language <lang>     Programming language (default: cpp)\n";
    std::cout << "  --viz-type <type>     Visualization type: flowchart, class-diagram, sequence\n";
    std::cout << "  --output <path>       Output file for visualization\n";
    std::cout << "  --help                Show this help message\n";
}

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void write_file(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot write to file: " + path);
    }
    file << content;
}

int main(int argc, char* argv[]) {
    std::string api_key;
    std::string code;
    std::string language = "cpp";
    std::string viz_type = "flowchart";
    std::string output_file;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            print_usage();
            return 0;
        } else if (arg == "--api-key" && i + 1 < argc) {
            api_key = argv[++i];
        } else if (arg == "--file" && i + 1 < argc) {
            std::string file_path = argv[++i];
            code = read_file(file_path);
        } else if (arg == "--code" && i + 1 < argc) {
            code = argv[++i];
        } else if (arg == "--language" && i + 1 < argc) {
            language = argv[++i];
        } else if (arg == "--viz-type" && i + 1 < argc) {
            viz_type = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            output_file = argv[++i];
        }
    }

    if (api_key.empty()) {
        // Try to read from environment variable
        const char* env_key = std::getenv("GEMINI_API_KEY");
        if (env_key) {
            api_key = env_key;
        } else {
            std::cerr << "Error: No API key provided. Use --api-key or set GEMINI_API_KEY environment variable.\n";
            return 1;
        }
    }

    if (code.empty()) {
        std::cerr << "Error: No code provided. Use --file or --code.\n";
        return 1;
    }

    try {
        // Create analyzer
        core::CodeAnalyzer analyzer(api_key);

        // Set options
        core::CodeAnalyzer::AnalysisOptions options;
        options.language = language;
        options.visualization_type = viz_type;

        std::cout << "Analyzing code...\n";

        // Analyze code
        auto result = analyzer.analyze_code(code, options);

        if (!result.success) {
            std::cerr << "Error: " << result.error_message << "\n";
            return 1;
        }

        std::cout << "\nAnalysis successful!\n\n";
        std::cout << "Description:\n" << result.description << "\n\n";

        std::cout << "Mermaid Diagram:\n";
        std::cout << "```mermaid\n" << result.mermaid_diagram << "\n```\n\n";

        // Parse and convert to Vizzu
        parser::MermaidParser parser;
        auto diagram = parser.parse(result.mermaid_diagram);

        parser::VizzuConverter converter;
        auto vizzu_data = converter.convert(diagram);

        vizzu_data.animation_steps = converter.generate_animation_sequence(diagram);

        // Generate visualization
        vizzu::VizzuIntegration vizzu;
        vizzu::ChartConfig config;
        config.title = "Code Visualization";

        vizzu.initialize(vizzu_data, config);

        std::string js_code = vizzu.generate_js_code();

        if (!output_file.empty()) {
            // Generate HTML file with visualization
            std::stringstream html;
            html << "<!DOCTYPE html>\n";
            html << "<html>\n<head>\n";
            html << "  <title>VZCode Visualization</title>\n";
            html << "  <script src=\"https://cdn.jsdelivr.net/npm/vizzu@latest/dist/vizzu.min.js\"></script>\n";
            html << "  <style>\n";
            html << "    body { margin: 0; padding: 20px; font-family: Arial, sans-serif; }\n";
            html << "    #vizzu-container { width: 800px; height: 600px; }\n";
            html << "  </style>\n";
            html << "</head>\n<body>\n";
            html << "  <h1>Code Visualization</h1>\n";
            html << "  <div id=\"vizzu-container\"></div>\n";
            html << "  <script>\n";
            html << js_code << "\n";
            html << "    initializeVisualization('vizzu-container');\n";
            html << "  </script>\n";
            html << "</body>\n</html>\n";

            write_file(output_file, html.str());
            std::cout << "Visualization saved to: " << output_file << "\n";
        } else {
            std::cout << "JavaScript Code:\n";
            std::cout << js_code << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
