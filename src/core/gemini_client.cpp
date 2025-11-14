#include "gemini_client.hpp"
#include <sstream>
#include <algorithm>

namespace vzcode {
namespace core {

GeminiClient::GeminiClient(const std::string& api_key)
    : api_key_(api_key),
      endpoint_("https://generativelanguage.googleapis.com/v1beta/models/"),
      model_("gemini-pro") {}

GeminiClient::~GeminiClient() {}

void GeminiClient::set_endpoint(const std::string& endpoint) {
    endpoint_ = endpoint;
}

void GeminiClient::set_model(const std::string& model) {
    model_ = model;
}

std::string GeminiClient::build_prompt(const std::string& code_content,
                                       const std::string& language,
                                       const std::string& viz_type) {
    std::stringstream ss;

    ss << "You are a software architecture analysis expert. Analyze the following " << language
       << " code and generate a " << viz_type << " diagram using Mermaid syntax.\n\n";

    ss << "Requirements:\n";
    ss << "1. Generate ONLY the Mermaid diagram code, wrapped in ```mermaid and ``` tags\n";
    ss << "2. For flowchart: Show the main execution flow and logic branches\n";
    ss << "3. For class-diagram: Show classes, their relationships, and key methods\n";
    ss << "4. For sequence: Show the interaction between different components\n";
    ss << "5. For component: Show the high-level architecture and module dependencies\n";
    ss << "6. Keep the diagram clear and focused on the most important elements\n";
    ss << "7. Use descriptive labels for nodes and edges\n\n";

    ss << "Code to analyze:\n\n```" << language << "\n" << code_content << "\n```\n\n";

    ss << "Please provide the " << viz_type << " diagram in Mermaid format.";

    return ss.str();
}

utils::JsonValue GeminiClient::build_request_body(const std::string& prompt) {
    utils::JsonValue body = utils::JsonValue::object();

    utils::JsonValue contents = utils::JsonValue::array();
    utils::JsonValue content = utils::JsonValue::object();
    utils::JsonValue parts = utils::JsonValue::array();
    utils::JsonValue part = utils::JsonValue::object();

    part["text"] = utils::JsonValue(prompt);
    parts.push_back(part);
    content["parts"] = parts;
    contents.push_back(content);

    body["contents"] = contents;

    // Generation config
    utils::JsonValue gen_config = utils::JsonValue::object();
    gen_config["temperature"] = utils::JsonValue(0.4);
    gen_config["topK"] = utils::JsonValue(32);
    gen_config["topP"] = utils::JsonValue(1.0);
    gen_config["maxOutputTokens"] = utils::JsonValue(2048);

    body["generationConfig"] = gen_config;

    return body;
}

std::string GeminiClient::extract_mermaid_from_text(const std::string& text) {
    // Find ```mermaid and ``` tags
    size_t start = text.find("```mermaid");
    if (start == std::string::npos) {
        start = text.find("```");
        if (start == std::string::npos) {
            return text; // No code block found, return as-is
        }
    }

    start = text.find('\n', start) + 1;
    size_t end = text.find("```", start);

    if (end == std::string::npos) {
        return text.substr(start);
    }

    return text.substr(start, end - start);
}

GeminiClient::AnalysisResult GeminiClient::parse_response(const utils::HttpClient::Response& response) {
    AnalysisResult result;

    if (!response.success) {
        result.success = false;
        result.error_message = response.error_message;
        return result;
    }

    try {
        utils::JsonValue json = utils::JsonValue::parse(response.body);

        if (json.has_key("error")) {
            result.success = false;
            result.error_message = json["error"]["message"].as_string();
            return result;
        }

        if (json.has_key("candidates") && json["candidates"].array_size() > 0) {
            auto candidate = json["candidates"][0];
            auto content = candidate["content"];

            if (content.has_key("parts") && content["parts"].array_size() > 0) {
                std::string text = content["parts"][0]["text"].as_string();

                result.success = true;
                result.content = text;
                result.mermaid_diagram = extract_mermaid_from_text(text);

                // Extract description (text before or after the diagram)
                size_t diagram_start = text.find("```");
                if (diagram_start != std::string::npos) {
                    result.description = text.substr(0, diagram_start);
                } else {
                    result.description = text;
                }
            }
        } else {
            result.success = false;
            result.error_message = "No response candidates from Gemini API";
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = std::string("Failed to parse response: ") + e.what();
    }

    return result;
}

GeminiClient::AnalysisResult GeminiClient::analyze_code(const std::string& code_content,
                                                        const std::string& language) {
    return generate_visualization(code_content, "flowchart", language);
}

GeminiClient::AnalysisResult GeminiClient::generate_visualization(const std::string& code_content,
                                                                  const std::string& viz_type,
                                                                  const std::string& language) {
    std::string prompt = build_prompt(code_content, language, viz_type);
    utils::JsonValue request_body = build_request_body(prompt);

    std::string url = endpoint_ + model_ + ":generateContent?key=" + api_key_;

    std::map<std::string, std::string> headers;
    headers["Content-Type"] = "application/json";

    utils::HttpClient::Response http_response = http_client_.post(url, request_body.stringify(), headers);

    return parse_response(http_response);
}

void GeminiClient::analyze_code_async(const std::string& code_content,
                                      AnalysisCallback callback,
                                      const std::string& language) {
    // For now, use synchronous call
    // In production, would use async HTTP
    AnalysisResult result = analyze_code(code_content, language);
    callback(result);
}

} // namespace core
} // namespace vzcode
