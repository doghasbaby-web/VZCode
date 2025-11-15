#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <emscripten/val.h>
#endif

#include "../core/code_analyzer.hpp"
#include "../parser/mermaid_parser.hpp"
#include "../parser/vizzu_converter.hpp"
#include "../vizzu/animation_wrapper.hpp"
#include "../vizzu/vizzu_kernel_wrapper.hpp"
#include <memory>
#include <string>

using namespace vzcode;
using namespace emscripten;

// Wrapper class for JavaScript bindings
class VZCodeAPI {
public:
    VZCodeAPI() : api_key_set_(false) {}

    // Set Gemini API key
    void setApiKey(const std::string& api_key) {
        api_key_ = api_key;
        api_key_set_ = true;
        analyzer_ = std::make_unique<core::CodeAnalyzer>(api_key);
    }

    // Analyze code and get Mermaid diagram
    std::string analyzeCode(const std::string& code,
                           const std::string& language,
                           const std::string& vizType) {
        if (!api_key_set_) {
            return "{\"success\": false, \"error\": \"API key not set\"}";
        }

        core::CodeAnalyzer::AnalysisOptions options;
        options.language = language;
        options.visualization_type = vizType;

        auto result = analyzer_->analyze_code(code, options);

        utils::JsonValue response = utils::JsonValue::object();
        response["success"] = utils::JsonValue(result.success);

        if (result.success) {
            response["mermaid"] = utils::JsonValue(result.mermaid_diagram);
            response["description"] = utils::JsonValue(result.description);

            utils::JsonValue insights_array = utils::JsonValue::array();
            for (const auto& insight : result.insights) {
                insights_array.push_back(utils::JsonValue(insight));
            }
            response["insights"] = insights_array;
        } else {
            response["error"] = utils::JsonValue(result.error_message);
        }

        return response.stringify();
    }

    // Parse Mermaid diagram
    std::string parseMermaid(const std::string& mermaid_text) {
        try {
            parser::MermaidParser parser;
            auto diagram = parser.parse(mermaid_text);

            utils::JsonValue response = utils::JsonValue::object();
            response["success"] = utils::JsonValue(true);
            response["diagramType"] = utils::JsonValue(diagram.diagram_type);
            response["direction"] = utils::JsonValue(diagram.direction);

            utils::JsonValue nodes_array = utils::JsonValue::array();
            for (const auto& node : diagram.nodes) {
                utils::JsonValue node_obj = utils::JsonValue::object();
                node_obj["id"] = utils::JsonValue(node.id);
                node_obj["label"] = utils::JsonValue(node.label);
                node_obj["type"] = utils::JsonValue(node.type);
                nodes_array.push_back(node_obj);
            }
            response["nodes"] = nodes_array;

            utils::JsonValue edges_array = utils::JsonValue::array();
            for (const auto& edge : diagram.edges) {
                utils::JsonValue edge_obj = utils::JsonValue::object();
                edge_obj["from"] = utils::JsonValue(edge.from);
                edge_obj["to"] = utils::JsonValue(edge.to);
                edge_obj["label"] = utils::JsonValue(edge.label);
                edge_obj["type"] = utils::JsonValue(edge.type);
                edges_array.push_back(edge_obj);
            }
            response["edges"] = edges_array;

            return response.stringify();
        } catch (const std::exception& e) {
            utils::JsonValue response = utils::JsonValue::object();
            response["success"] = utils::JsonValue(false);
            response["error"] = utils::JsonValue(e.what());
            return response.stringify();
        }
    }

    // Convert Mermaid to Vizzu format
    std::string mermaidToVizzu(const std::string& mermaid_text) {
        try {
            parser::MermaidParser parser;
            auto diagram = parser.parse(mermaid_text);

            parser::VizzuConverter converter;
            auto vizzu_data = converter.convert(diagram);

            utils::JsonValue data_json = converter.to_json(vizzu_data);

            utils::JsonValue response = utils::JsonValue::object();
            response["success"] = utils::JsonValue(true);
            response["data"] = data_json;

            return response.stringify();
        } catch (const std::exception& e) {
            utils::JsonValue response = utils::JsonValue::object();
            response["success"] = utils::JsonValue(false);
            response["error"] = utils::JsonValue(e.what());
            return response.stringify();
        }
    }

    // Generate complete Vizzu visualization code
    std::string generateVisualization(const std::string& code,
                                     const std::string& language,
                                     const std::string& vizType) {
        if (!api_key_set_) {
            return "{\"success\": false, \"error\": \"API key not set\"}";
        }

        try {
            // Step 1: Analyze code with Gemini
            core::CodeAnalyzer::AnalysisOptions options;
            options.language = language;
            options.visualization_type = vizType;

            auto analysis_result = analyzer_->analyze_code(code, options);

            if (!analysis_result.success) {
                utils::JsonValue response = utils::JsonValue::object();
                response["success"] = utils::JsonValue(false);
                response["error"] = utils::JsonValue(analysis_result.error_message);
                return response.stringify();
            }

            // Step 2: Parse Mermaid
            parser::MermaidParser parser;
            auto diagram = parser.parse(analysis_result.mermaid_diagram);

            // Step 3: Convert to Vizzu
            parser::VizzuConverter converter;
            auto vizzu_data = converter.convert(diagram);

            // Generate animation steps
            vizzu_data.animation_steps = converter.generate_animation_sequence(diagram);

            // Step 4: Create visualization
            vizzu::VizzuIntegration vizzu;
            vizzu::ChartConfig chart_config;
            chart_config.title = "Code Visualization: " + language;
            chart_config.width = 800;
            chart_config.height = 600;

            vizzu.initialize(vizzu_data, chart_config);

            // Generate JavaScript code
            std::string js_code = vizzu.generate_js_code();

            utils::JsonValue response = utils::JsonValue::object();
            response["success"] = utils::JsonValue(true);
            response["mermaid"] = utils::JsonValue(analysis_result.mermaid_diagram);
            response["description"] = utils::JsonValue(analysis_result.description);
            response["vizzuData"] = converter.to_json(vizzu_data);
            response["jsCode"] = utils::JsonValue(js_code);

            return response.stringify();
        } catch (const std::exception& e) {
            utils::JsonValue response = utils::JsonValue::object();
            response["success"] = utils::JsonValue(false);
            response["error"] = utils::JsonValue(e.what());
            return response.stringify();
        }
    }

    // Get version
    std::string getVersion() const {
        return "1.0.0";
    }

    // Get Vizzu kernel version (if available)
    std::string getVizzuKernelVersion() const {
#ifdef VIZZU_KERNEL_AVAILABLE
        vizzu::VizzuKernelWrapper kernel;
        return kernel.get_version();
#else
        return "not-available";
#endif
    }

    // Check if Vizzu kernel is available
    bool isVizzuKernelAvailable() const {
#ifdef VIZZU_KERNEL_AVAILABLE
        return true;
#else
        return false;
#endif
    }

    // Initialize and render using C++ kernel directly (returns status)
    std::string renderWithKernel(const std::string& mermaid_text,
                                 int width,
                                 int height) {
#ifdef VIZZU_KERNEL_AVAILABLE
        try {
            // Parse Mermaid
            parser::MermaidParser parser;
            auto diagram = parser.parse(mermaid_text);

            // Convert to Vizzu
            parser::VizzuConverter converter;
            auto vizzu_data = converter.convert(diagram);

            // Initialize Vizzu integration
            vizzu::VizzuIntegration vizzu_integration;
            vizzu::ChartConfig config;
            config.width = width;
            config.height = height;

            if (!vizzu_integration.initialize(vizzu_data, config)) {
                return "{\"success\": false, \"error\": \"Failed to initialize Vizzu kernel\"}";
            }

            // Render using kernel
            if (!vizzu_integration.render_with_kernel(width, height)) {
                return "{\"success\": false, \"error\": \"Failed to render with kernel\"}";
            }

            utils::JsonValue response = utils::JsonValue::object();
            response["success"] = utils::JsonValue(true);
            response["message"] = utils::JsonValue("Rendered using Vizzu C++ kernel");

            return response.stringify();
        } catch (const std::exception& e) {
            utils::JsonValue response = utils::JsonValue::object();
            response["success"] = utils::JsonValue(false);
            response["error"] = utils::JsonValue(e.what());
            return response.stringify();
        }
#else
        utils::JsonValue response = utils::JsonValue::object();
        response["success"] = utils::JsonValue(false);
        response["error"] = utils::JsonValue("Vizzu kernel not available");
        return response.stringify();
#endif
    }

private:
    std::string api_key_;
    bool api_key_set_;
    std::unique_ptr<core::CodeAnalyzer> analyzer_;
};

#ifdef __EMSCRIPTEN__

// Emscripten bindings
EMSCRIPTEN_BINDINGS(vzcode_module) {
    class_<VZCodeAPI>("VZCodeAPI")
        .constructor<>()
        .function("setApiKey", &VZCodeAPI::setApiKey)
        .function("analyzeCode", &VZCodeAPI::analyzeCode)
        .function("parseMermaid", &VZCodeAPI::parseMermaid)
        .function("mermaidToVizzu", &VZCodeAPI::mermaidToVizzu)
        .function("generateVisualization", &VZCodeAPI::generateVisualization)
        .function("getVersion", &VZCodeAPI::getVersion)
        .function("getVizzuKernelVersion", &VZCodeAPI::getVizzuKernelVersion)
        .function("isVizzuKernelAvailable", &VZCodeAPI::isVizzuKernelAvailable)
        .function("renderWithKernel", &VZCodeAPI::renderWithKernel);
}

#endif
