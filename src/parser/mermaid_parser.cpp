#include "mermaid_parser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace vzcode {
namespace parser {

MermaidParser::MermaidParser() {}

MermaidParser::~MermaidParser() {}

std::vector<std::string> MermaidParser::supported_types() {
    return {"flowchart", "graph", "classDiagram", "sequenceDiagram", "stateDiagram", "erDiagram"};
}

std::vector<std::string> MermaidParser::split_lines(const std::string& text) {
    std::vector<std::string> lines;
    std::stringstream ss(text);
    std::string line;

    while (std::getline(ss, line)) {
        line = trim(line);
        if (!line.empty() && line[0] != '%') { // Skip comments
            lines.push_back(line);
        }
    }

    return lines;
}

std::string MermaidParser::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();

    while (start < end && std::isspace(str[start])) ++start;
    while (end > start && std::isspace(str[end - 1])) --end;

    return str.substr(start, end - start);
}

bool MermaidParser::starts_with(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

bool MermaidParser::validate(const std::string& mermaid_text, std::string& error_message) {
    try {
        parse(mermaid_text);
        return true;
    } catch (const std::exception& e) {
        error_message = e.what();
        return false;
    }
}

MermaidDiagram MermaidParser::parse(const std::string& mermaid_text) {
    MermaidDiagram diagram;
    std::vector<std::string> lines = split_lines(mermaid_text);

    if (lines.empty()) {
        throw std::runtime_error("Empty Mermaid diagram");
    }

    // Determine diagram type from first line
    std::string first_line = lines[0];

    if (starts_with(first_line, "flowchart")) {
        diagram.diagram_type = "flowchart";
        if (first_line.length() > 10) {
            diagram.direction = trim(first_line.substr(9));
        }
        parse_flowchart(mermaid_text, diagram);
    } else if (starts_with(first_line, "graph")) {
        diagram.diagram_type = "graph";
        if (first_line.length() > 6) {
            diagram.direction = trim(first_line.substr(5));
        }
        parse_graph(mermaid_text, diagram);
    } else if (starts_with(first_line, "classDiagram")) {
        diagram.diagram_type = "classDiagram";
        parse_class_diagram(mermaid_text, diagram);
    } else if (starts_with(first_line, "sequenceDiagram")) {
        diagram.diagram_type = "sequenceDiagram";
        parse_sequence_diagram(mermaid_text, diagram);
    } else {
        // Default to graph
        diagram.diagram_type = "graph";
        parse_graph(mermaid_text, diagram);
    }

    return diagram;
}

std::string MermaidParser::extract_node_id(const std::string& node_str) {
    size_t bracket_pos = node_str.find('[');
    size_t paren_pos = node_str.find('(');
    size_t brace_pos = node_str.find('{');

    size_t delim_pos = std::min({bracket_pos, paren_pos, brace_pos});

    if (delim_pos == std::string::npos) {
        return trim(node_str);
    }

    return trim(node_str.substr(0, delim_pos));
}

std::string MermaidParser::extract_node_label(const std::string& node_str) {
    size_t start = node_str.find_first_of("[({");
    size_t end = node_str.find_last_of("])}");

    if (start == std::string::npos || end == std::string::npos) {
        return extract_node_id(node_str);
    }

    return trim(node_str.substr(start + 1, end - start - 1));
}

std::string MermaidParser::determine_node_type(const std::string& node_str) {
    if (node_str.find('[') != std::string::npos) return "rectangle";
    if (node_str.find('(') != std::string::npos) return "rounded";
    if (node_str.find('{') != std::string::npos) return "rhombus";
    if (node_str.find("((") != std::string::npos) return "circle";
    if (node_str.find(">") != std::string::npos) return "asymmetric";
    return "rectangle";
}

MermaidNode MermaidParser::parse_node(const std::string& node_def) {
    MermaidNode node;
    node.id = extract_node_id(node_def);
    node.label = extract_node_label(node_def);
    node.type = determine_node_type(node_def);
    return node;
}

MermaidEdge MermaidParser::parse_edge(const std::string& edge_def) {
    MermaidEdge edge;

    // Find arrow types: -->, --->, -.->", ==>
    size_t arrow_pos = edge_def.find("-->");
    if (arrow_pos == std::string::npos) arrow_pos = edge_def.find("--->");
    if (arrow_pos == std::string::npos) arrow_pos = edge_def.find(".->");
    if (arrow_pos == std::string::npos) arrow_pos = edge_def.find("==>");
    if (arrow_pos == std::string::npos) arrow_pos = edge_def.find("--");

    if (arrow_pos != std::string::npos) {
        std::string from_part = trim(edge_def.substr(0, arrow_pos));
        edge.from = extract_node_id(from_part);

        // Find the arrow end
        size_t arrow_end = edge_def.find('>', arrow_pos);
        if (arrow_end == std::string::npos) {
            arrow_end = arrow_pos + 2;
        } else {
            arrow_end++;
        }

        std::string to_part = trim(edge_def.substr(arrow_end));

        // Extract label if present
        size_t pipe_pos = to_part.find('|');
        if (pipe_pos != std::string::npos) {
            size_t pipe_end = to_part.find('|', pipe_pos + 1);
            if (pipe_end != std::string::npos) {
                edge.label = trim(to_part.substr(pipe_pos + 1, pipe_end - pipe_pos - 1));
                to_part = trim(to_part.substr(pipe_end + 1));
            }
        }

        edge.to = extract_node_id(to_part);

        // Determine edge type
        std::string arrow = edge_def.substr(arrow_pos, arrow_end - arrow_pos);
        if (arrow.find(".-") != std::string::npos) edge.type = "dotted";
        else if (arrow.find("==") != std::string::npos) edge.type = "thick";
        else edge.type = "solid";
    }

    return edge;
}

void MermaidParser::parse_flowchart(const std::string& content, MermaidDiagram& diagram) {
    parse_graph(content, diagram); // Flowchart uses same syntax as graph
}

void MermaidParser::parse_graph(const std::string& content, MermaidDiagram& diagram) {
    std::vector<std::string> lines = split_lines(content);

    for (size_t i = 1; i < lines.size(); ++i) {
        const std::string& line = lines[i];

        // Check if it's an edge definition
        if (line.find("-->") != std::string::npos ||
            line.find("-.-") != std::string::npos ||
            line.find("==>") != std::string::npos ||
            line.find("---") != std::string::npos) {

            MermaidEdge edge = parse_edge(line);
            if (!edge.from.empty() && !edge.to.empty()) {
                diagram.edges.push_back(edge);

                // Add nodes if not already present
                bool from_exists = false, to_exists = false;
                for (const auto& node : diagram.nodes) {
                    if (node.id == edge.from) from_exists = true;
                    if (node.id == edge.to) to_exists = true;
                }

                if (!from_exists) {
                    MermaidNode node;
                    node.id = edge.from;
                    node.label = edge.from;
                    node.type = "rectangle";
                    diagram.nodes.push_back(node);
                }

                if (!to_exists) {
                    MermaidNode node;
                    node.id = edge.to;
                    node.label = edge.to;
                    node.type = "rectangle";
                    diagram.nodes.push_back(node);
                }
            }
        } else {
            // Standalone node definition
            MermaidNode node = parse_node(line);
            if (!node.id.empty()) {
                // Check if node already exists
                bool exists = false;
                for (auto& existing_node : diagram.nodes) {
                    if (existing_node.id == node.id) {
                        existing_node = node; // Update with new info
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    diagram.nodes.push_back(node);
                }
            }
        }
    }
}

void MermaidParser::parse_class_diagram(const std::string& content, MermaidDiagram& diagram) {
    std::vector<std::string> lines = split_lines(content);

    for (size_t i = 1; i < lines.size(); ++i) {
        const std::string& line = lines[i];

        // Parse class relationships
        if (line.find("-->") != std::string::npos ||
            line.find("<|--") != std::string::npos ||
            line.find("*--") != std::string::npos) {

            MermaidEdge edge = parse_edge(line);
            if (!edge.from.empty() && !edge.to.empty()) {
                diagram.edges.push_back(edge);
            }
        }
    }
}

void MermaidParser::parse_sequence_diagram(const std::string& content, MermaidDiagram& diagram) {
    std::vector<std::string> lines = split_lines(content);

    for (size_t i = 1; i < lines.size(); ++i) {
        const std::string& line = lines[i];

        // Parse sequence interactions
        if (line.find("->>") != std::string::npos ||
            line.find("-->>") != std::string::npos) {

            MermaidEdge edge = parse_edge(line);
            if (!edge.from.empty() && !edge.to.empty()) {
                diagram.edges.push_back(edge);
            }
        }
    }
}

} // namespace parser
} // namespace vzcode
