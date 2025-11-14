#include "vizzu_converter.hpp"
#include <algorithm>
#include <cmath>

namespace vzcode {
namespace parser {

VizzuConverter::VizzuConverter()
    : animation_duration_ms_(1000), default_geometry_("circle") {}

VizzuConverter::~VizzuConverter() {}

void VizzuConverter::set_animation_duration(int duration_ms) {
    animation_duration_ms_ = duration_ms;
}

void VizzuConverter::set_default_geometry(const std::string& geometry) {
    default_geometry_ = geometry;
}

std::string VizzuConverter::map_node_type_to_geometry(const std::string& mermaid_type) {
    if (mermaid_type == "circle") return "circle";
    if (mermaid_type == "rhombus") return "rectangle";
    if (mermaid_type == "rounded") return "circle";
    if (mermaid_type == "rectangle") return "rectangle";
    return default_geometry_;
}

VizzuSeries VizzuConverter::create_node_series(const std::vector<MermaidNode>& nodes) {
    VizzuSeries series;
    series.name = "Nodes";
    series.type = "dimension";

    for (const auto& node : nodes) {
        series.values.push_back(node.label.empty() ? node.id : node.label);
    }

    return series;
}

VizzuSeries VizzuConverter::create_edge_series(const std::vector<MermaidEdge>& edges) {
    VizzuSeries series;
    series.name = "Edges";
    series.type = "dimension";

    for (const auto& edge : edges) {
        std::string edge_label = edge.from + " → " + edge.to;
        if (!edge.label.empty()) {
            edge_label += " (" + edge.label + ")";
        }
        series.values.push_back(edge_label);
    }

    return series;
}

VizzuSeries VizzuConverter::create_weight_series(const std::vector<MermaidEdge>& edges) {
    VizzuSeries series;
    series.name = "Weight";
    series.type = "measure";

    for (const auto& edge : edges) {
        series.values.push_back(std::to_string(edge.weight));
    }

    return series;
}

std::vector<int> VizzuConverter::calculate_node_positions(const MermaidDiagram& diagram) {
    // Simple layout: assign positions based on connectivity
    std::vector<int> positions(diagram.nodes.size(), 0);

    // Count incoming and outgoing edges for each node
    std::map<std::string, int> levels;

    for (const auto& node : diagram.nodes) {
        levels[node.id] = 0;
    }

    // Calculate levels using BFS-like approach
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& edge : diagram.edges) {
            if (levels[edge.to] <= levels[edge.from]) {
                levels[edge.to] = levels[edge.from] + 1;
                changed = true;
            }
        }
    }

    // Convert to positions
    for (size_t i = 0; i < diagram.nodes.size(); ++i) {
        positions[i] = levels[diagram.nodes[i].id];
    }

    return positions;
}

VizzuData VizzuConverter::convert_flowchart(const MermaidDiagram& diagram) {
    VizzuData data;

    // Create series for nodes
    VizzuSeries node_series;
    node_series.name = "Component";
    node_series.type = "dimension";

    VizzuSeries type_series;
    type_series.name = "Type";
    type_series.type = "dimension";

    VizzuSeries level_series;
    level_series.name = "Level";
    level_series.type = "measure";

    std::vector<int> positions = calculate_node_positions(diagram);

    for (size_t i = 0; i < diagram.nodes.size(); ++i) {
        const auto& node = diagram.nodes[i];
        node_series.values.push_back(node.label.empty() ? node.id : node.label);
        type_series.values.push_back(node.type);
        level_series.values.push_back(std::to_string(positions[i]));
    }

    data.series.push_back(node_series);
    data.series.push_back(type_series);
    data.series.push_back(level_series);

    // Create connection strength series
    VizzuSeries connection_series;
    connection_series.name = "Connections";
    connection_series.type = "measure";

    std::map<std::string, int> connection_count;
    for (const auto& node : diagram.nodes) {
        connection_count[node.id] = 0;
    }

    for (const auto& edge : diagram.edges) {
        connection_count[edge.from]++;
        connection_count[edge.to]++;
    }

    for (const auto& node : diagram.nodes) {
        connection_series.values.push_back(std::to_string(connection_count[node.id] + 1));
    }

    data.series.push_back(connection_series);

    // Configuration
    data.config["title"] = "Code Flow Visualization";
    data.config["subtitle"] = diagram.diagram_type;

    return data;
}

VizzuData VizzuConverter::convert_class_diagram(const MermaidDiagram& diagram) {
    VizzuData data;

    VizzuSeries class_series;
    class_series.name = "Class";
    class_series.type = "dimension";

    VizzuSeries relationship_count;
    relationship_count.name = "Relationships";
    relationship_count.type = "measure";

    std::map<std::string, int> rel_count;
    for (const auto& node : diagram.nodes) {
        rel_count[node.id] = 0;
    }

    for (const auto& edge : diagram.edges) {
        rel_count[edge.from]++;
        rel_count[edge.to]++;
    }

    for (const auto& node : diagram.nodes) {
        class_series.values.push_back(node.label.empty() ? node.id : node.label);
        relationship_count.values.push_back(std::to_string(rel_count[node.id] + 1));
    }

    data.series.push_back(class_series);
    data.series.push_back(relationship_count);

    data.config["title"] = "Class Diagram Visualization";

    return data;
}

VizzuData VizzuConverter::convert_sequence_diagram(const MermaidDiagram& diagram) {
    VizzuData data;

    VizzuSeries interaction_series;
    interaction_series.name = "Interaction";
    interaction_series.type = "dimension";

    VizzuSeries step_series;
    step_series.name = "Step";
    step_series.type = "measure";

    VizzuSeries actor_series;
    actor_series.name = "Actor";
    actor_series.type = "dimension";

    for (size_t i = 0; i < diagram.edges.size(); ++i) {
        const auto& edge = diagram.edges[i];
        std::string interaction = edge.from + " to " + edge.to;
        if (!edge.label.empty()) {
            interaction += ": " + edge.label;
        }

        interaction_series.values.push_back(interaction);
        step_series.values.push_back(std::to_string(i + 1));
        actor_series.values.push_back(edge.from);
    }

    data.series.push_back(interaction_series);
    data.series.push_back(step_series);
    data.series.push_back(actor_series);

    data.config["title"] = "Sequence Diagram Visualization";

    return data;
}

VizzuData VizzuConverter::convert(const MermaidDiagram& diagram) {
    if (diagram.diagram_type == "flowchart" || diagram.diagram_type == "graph") {
        return convert_flowchart(diagram);
    } else if (diagram.diagram_type == "classDiagram") {
        return convert_class_diagram(diagram);
    } else if (diagram.diagram_type == "sequenceDiagram") {
        return convert_sequence_diagram(diagram);
    } else {
        return convert_flowchart(diagram);
    }
}

utils::JsonValue VizzuConverter::to_json(const VizzuData& vizzu_data) {
    utils::JsonValue json = utils::JsonValue::object();
    utils::JsonValue series_array = utils::JsonValue::array();

    for (const auto& series : vizzu_data.series) {
        utils::JsonValue series_obj = utils::JsonValue::object();
        series_obj["name"] = utils::JsonValue(series.name);

        utils::JsonValue values_array = utils::JsonValue::array();
        for (const auto& value : series.values) {
            // Try to parse as number, otherwise treat as string
            try {
                double num = std::stod(value);
                values_array.push_back(utils::JsonValue(num));
            } catch (...) {
                values_array.push_back(utils::JsonValue(value));
            }
        }

        series_obj["values"] = values_array;

        if (!series.type.empty()) {
            series_obj["type"] = utils::JsonValue(series.type);
        }

        series_array.push_back(series_obj);
    }

    json["series"] = series_array;

    return json;
}

std::vector<VizzuAnimationConfig> VizzuConverter::generate_animation_sequence(const MermaidDiagram& diagram) {
    std::vector<VizzuAnimationConfig> sequence;

    VizzuAnimationConfig step1;
    step1.title = "Initial View";
    step1.geometry = "circle";
    step1.duration_ms = animation_duration_ms_;

    if (!diagram.nodes.empty()) {
        step1.x = "Component";
        step1.y = "Connections";
        step1.color = "Type";
    }

    sequence.push_back(step1);

    // Add transition step
    if (diagram.diagram_type == "flowchart" || diagram.diagram_type == "graph") {
        VizzuAnimationConfig step2;
        step2.title = "Flow Analysis";
        step2.x = "Level";
        step2.y = "Connections";
        step2.color = "Type";
        step2.geometry = "rectangle";
        step2.duration_ms = animation_duration_ms_;
        sequence.push_back(step2);
    }

    return sequence;
}

} // namespace parser
} // namespace vzcode
