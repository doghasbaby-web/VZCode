#ifndef GEMINI_CLIENT_HPP
#define GEMINI_CLIENT_HPP

#include <string>
#include <vector>
#include <functional>
#include "../utils/http_client.hpp"
#include "../utils/json_utils.hpp"

namespace vzcode {
namespace core {

/**
 * Client for interacting with Google's Gemini API
 * Supports code analysis and diagram generation
 */
class GeminiClient {
public:
    struct AnalysisResult {
        bool success;
        std::string content;
        std::string error_message;
        std::string mermaid_diagram;
        std::string description;
    };

    using AnalysisCallback = std::function<void(const AnalysisResult&)>;

    explicit GeminiClient(const std::string& api_key);
    ~GeminiClient();

    /**
     * Analyze code and generate a software architecture diagram
     * @param code_content The source code to analyze
     * @param language Programming language (cpp, js, python, etc.)
     * @return Analysis result with Mermaid diagram
     */
    AnalysisResult analyze_code(const std::string& code_content,
                                 const std::string& language = "cpp");

    /**
     * Asynchronous version of analyze_code
     */
    void analyze_code_async(const std::string& code_content,
                            AnalysisCallback callback,
                            const std::string& language = "cpp");

    /**
     * Generate a specific type of visualization
     * Types: "flowchart", "class-diagram", "sequence", "component"
     */
    AnalysisResult generate_visualization(const std::string& code_content,
                                          const std::string& viz_type,
                                          const std::string& language = "cpp");

    /**
     * Set the API endpoint (useful for testing or different API versions)
     */
    void set_endpoint(const std::string& endpoint);

    /**
     * Set the model to use (default: gemini-pro)
     */
    void set_model(const std::string& model);

private:
    std::string api_key_;
    std::string endpoint_;
    std::string model_;
    utils::HttpClient http_client_;

    std::string build_prompt(const std::string& code_content,
                             const std::string& language,
                             const std::string& viz_type);

    utils::JsonValue build_request_body(const std::string& prompt);
    AnalysisResult parse_response(const utils::HttpClient::Response& response);
    std::string extract_mermaid_from_text(const std::string& text);
};

} // namespace core
} // namespace vzcode

#endif // GEMINI_CLIENT_HPP
