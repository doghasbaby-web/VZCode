#ifndef MERMAID_PARSER_HPP
#define MERMAID_PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace vzcode {
namespace parser {

/**
 * Represents a node in a Mermaid diagram
 */
struct MermaidNode {
    std::string id;
    std::string label;
    std::string type;  // rectangle, circle, rhombus, etc.
    std::map<std::string, std::string> attributes;
};

/**
 * Represents an edge/relationship in a Mermaid diagram
 */
struct MermaidEdge {
    std::string from;
    std::string to;
    std::string label;
    std::string type;  // arrow, line, dotted, etc.
    int weight = 1;
};

/**
 * Parsed Mermaid diagram structure
 */
struct MermaidDiagram {
    std::string diagram_type;  // flowchart, classDiagram, sequenceDiagram, etc.
    std::string direction;     // TB, LR, RL, BT (for flowcharts)
    std::vector<MermaidNode> nodes;
    std::vector<MermaidEdge> edges;
    std::map<std::string, std::string> metadata;
};

/**
 * Parser for Mermaid diagram syntax
 * Converts Mermaid text format into structured data
 */
class MermaidParser {
public:
    MermaidParser();
    ~MermaidParser();

    /**
     * Parse a Mermaid diagram from string
     * @param mermaid_text The Mermaid diagram text
     * @return Parsed diagram structure
     */
    MermaidDiagram parse(const std::string& mermaid_text);

    /**
     * Validate Mermaid syntax
     */
    bool validate(const std::string& mermaid_text, std::string& error_message);

    /**
     * Get supported diagram types
     */
    static std::vector<std::string> supported_types();

    // Public for testing - these are implementation details but heavily tested
    std::string extract_node_id(const std::string& node_str);
    std::string extract_node_label(const std::string& node_str);
    std::string determine_node_type(const std::string& node_str);
    std::string trim(const std::string& str);
    bool starts_with(const std::string& str, const std::string& prefix);

private:
    void parse_flowchart(const std::string& content, MermaidDiagram& diagram);
    void parse_class_diagram(const std::string& content, MermaidDiagram& diagram);
    void parse_sequence_diagram(const std::string& content, MermaidDiagram& diagram);
    void parse_graph(const std::string& content, MermaidDiagram& diagram);

    MermaidNode parse_node(const std::string& node_def);
    MermaidEdge parse_edge(const std::string& edge_def);

    std::vector<std::string> split_lines(const std::string& text);
};

} // namespace parser
} // namespace vzcode

#endif // MERMAID_PARSER_HPP
