#ifndef VIZZU_CONVERTER_HPP
#define VIZZU_CONVERTER_HPP

#include "mermaid_parser.hpp"
#include "../utils/json_utils.hpp"
#include <string>
#include <vector>

namespace vzcode {
namespace parser {

/**
 * Vizzu data series structure
 */
struct VizzuSeries {
    std::string name;
    std::vector<std::string> values;
    std::string type;  // "dimension" or "measure"
};

/**
 * Vizzu animation config
 */
struct VizzuAnimationConfig {
    std::string x;
    std::string y;
    std::string color;
    std::string label;
    std::string title;
    std::string geometry;  // circle, rectangle, line, area
    int duration_ms = 1000;
};

/**
 * Complete Vizzu data structure
 */
struct VizzuData {
    std::vector<VizzuSeries> series;
    std::vector<VizzuAnimationConfig> animation_steps;
    std::map<std::string, std::string> config;
};

/**
 * Converts parsed Mermaid diagrams to Vizzu data format
 */
class VizzuConverter {
public:
    VizzuConverter();
    ~VizzuConverter();

    /**
     * Convert a Mermaid diagram to Vizzu data format
     * @param diagram Parsed Mermaid diagram
     * @return Vizzu-compatible data structure
     */
    VizzuData convert(const MermaidDiagram& diagram);

    /**
     * Convert to JSON format for Vizzu JS library
     */
    utils::JsonValue to_json(const VizzuData& vizzu_data);

    /**
     * Generate animation sequence from diagram
     */
    std::vector<VizzuAnimationConfig> generate_animation_sequence(const MermaidDiagram& diagram);

    /**
     * Set animation parameters
     */
    void set_animation_duration(int duration_ms);
    void set_default_geometry(const std::string& geometry);

private:
    int animation_duration_ms_;
    std::string default_geometry_;

    VizzuData convert_flowchart(const MermaidDiagram& diagram);
    VizzuData convert_class_diagram(const MermaidDiagram& diagram);
    VizzuData convert_sequence_diagram(const MermaidDiagram& diagram);

    VizzuSeries create_node_series(const std::vector<MermaidNode>& nodes);
    VizzuSeries create_edge_series(const std::vector<MermaidEdge>& edges);
    VizzuSeries create_weight_series(const std::vector<MermaidEdge>& edges);

    std::string map_node_type_to_geometry(const std::string& mermaid_type);
    std::vector<int> calculate_node_positions(const MermaidDiagram& diagram);
};

} // namespace parser
} // namespace vzcode

#endif // VIZZU_CONVERTER_HPP
