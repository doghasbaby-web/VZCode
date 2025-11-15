#include "vizzu_integration.hpp"
#include "vizzu_kernel_wrapper.hpp"

namespace vzcode {
namespace vizzu {

VizzuIntegration::VizzuIntegration()
    : kernel_wrapper_(std::make_unique<VizzuKernelWrapper>()) {}

VizzuIntegration::~VizzuIntegration() {
    if (kernel_wrapper_) {
        kernel_wrapper_->cleanup();
    }
}

bool VizzuIntegration::initialize(const parser::VizzuData& data, const ChartConfig& config) {
    data_ = data;
    config_ = config;
    animation_state_.total_steps = static_cast<int>(data.animation_steps.size());

    // Try to initialize the kernel wrapper
    if (kernel_wrapper_) {
        initialize_kernel();
    }

    return true;
}

bool VizzuIntegration::initialize_kernel() {
    if (!kernel_wrapper_) {
        return false;
    }

    // Initialize Vizzu C++ kernel
    if (!kernel_wrapper_->initialize()) {
        return false;
    }

    // Load data into Vizzu kernel
    if (!kernel_wrapper_->load_vizzu_data(data_)) {
        return false;
    }

    // Set initial configuration
    if (!data_.series.empty()) {
        // Configure chart channels based on available data
        if (data_.series.size() >= 2) {
            kernel_wrapper_->set_config("config.channels.x",
                "{\"set\": [\"" + data_.series[0].name + "\"]}");
            kernel_wrapper_->set_config("config.channels.y",
                "{\"set\": [\"" + data_.series[1].name + "\"]}");
        }

        if (data_.series.size() >= 2) {
            kernel_wrapper_->set_config("config.channels.color",
                "{\"set\": [\"" + data_.series[1].name + "\"]}");
        }
    }

    // Set chart title
    if (!config_.title.empty()) {
        kernel_wrapper_->set_style("title", config_.title);
    }

    return true;
}

bool VizzuIntegration::render_with_kernel(double width, double height) {
    if (!kernel_wrapper_ || !kernel_wrapper_->is_initialized()) {
        return false;
    }

    return kernel_wrapper_->render(width, height);
}

bool VizzuIntegration::animate_with_kernel() {
    if (!kernel_wrapper_ || !kernel_wrapper_->is_initialized()) {
        return false;
    }

    // Execute animation steps
    for (const auto& step : data_.animation_steps) {
        // Set keyframe
        kernel_wrapper_->begin_keyframe();

        // Configure animation
        if (!step.x.empty()) {
            kernel_wrapper_->set_animation_config("config.channels.x",
                "{\"set\": [\"" + step.x + "\"]}");
        }
        if (!step.y.empty()) {
            kernel_wrapper_->set_animation_config("config.channels.y",
                "{\"set\": [\"" + step.y + "\"]}");
        }
        if (!step.color.empty()) {
            kernel_wrapper_->set_animation_config("config.channels.color",
                "{\"set\": [\"" + step.color + "\"]}");
        }

        // Set animation duration
        kernel_wrapper_->set_animation_duration(step.duration_ms);

        // Trigger animation
        if (!kernel_wrapper_->animate()) {
            return false;
        }
    }

    return true;
}

void VizzuIntegration::set_config(const ChartConfig& config) {
    config_ = config;
}

void VizzuIntegration::set_animation_state(const AnimationState& state) {
    animation_state_ = state;
}

utils::JsonValue VizzuIntegration::build_data_json() const {
    utils::JsonValue data_json = utils::JsonValue::object();
    utils::JsonValue series_array = utils::JsonValue::array();

    for (const auto& series : data_.series) {
        utils::JsonValue series_obj = utils::JsonValue::object();
        series_obj["name"] = utils::JsonValue(series.name);

        utils::JsonValue values_array = utils::JsonValue::array();
        for (const auto& value : series.values) {
            // Try to parse as number
            try {
                double num = std::stod(value);
                values_array.push_back(utils::JsonValue(num));
            } catch (...) {
                values_array.push_back(utils::JsonValue(value));
            }
        }

        series_obj["values"] = values_array;
        series_array.push_back(series_obj);
    }

    data_json["series"] = series_array;
    return data_json;
}

utils::JsonValue VizzuIntegration::build_vizzu_config() const {
    utils::JsonValue config = utils::JsonValue::object();

    if (!config_.title.empty()) {
        config["title"] = utils::JsonValue(config_.title);
    }

    utils::JsonValue channels = utils::JsonValue::object();

    // Set default channels if data is available
    if (!data_.series.empty()) {
        if (data_.series.size() >= 2) {
            channels["x"] = utils::JsonValue(data_.series[0].name);
            channels["y"] = utils::JsonValue(data_.series[1].name);
        }

        if (data_.series.size() >= 3) {
            channels["color"] = utils::JsonValue(data_.series[1].name);
        }
    }

    config["channels"] = channels;

    utils::JsonValue style = utils::JsonValue::object();
    style["plot"] = utils::JsonValue::object();

    config["style"] = style;

    return config;
}

utils::JsonValue VizzuIntegration::build_style_json() const {
    utils::JsonValue style = utils::JsonValue::object();

    utils::JsonValue plot = utils::JsonValue::object();
    plot["paddingLeft"] = utils::JsonValue(50);
    plot["paddingRight"] = utils::JsonValue(50);
    plot["paddingTop"] = utils::JsonValue(50);
    plot["paddingBottom"] = utils::JsonValue(50);

    style["plot"] = plot;

    utils::JsonValue title_style = utils::JsonValue::object();
    title_style["fontSize"] = utils::JsonValue(24);

    style["title"] = title_style;

    return style;
}

std::string VizzuIntegration::get_init_config_json() const {
    utils::JsonValue init_obj = utils::JsonValue::object();
    init_obj["data"] = build_data_json();

    return init_obj.stringify();
}

std::string VizzuIntegration::get_animation_steps_json() const {
    utils::JsonValue steps_array = utils::JsonValue::array();

    for (const auto& step : data_.animation_steps) {
        utils::JsonValue step_obj = utils::JsonValue::object();

        utils::JsonValue config = utils::JsonValue::object();

        if (!step.x.empty()) config["x"] = utils::JsonValue(step.x);
        if (!step.y.empty()) config["y"] = utils::JsonValue(step.y);
        if (!step.color.empty()) config["color"] = utils::JsonValue(step.color);
        if (!step.label.empty()) config["label"] = utils::JsonValue(step.label);

        step_obj["config"] = config;

        utils::JsonValue style = utils::JsonValue::object();
        if (!step.title.empty()) {
            style["title"] = utils::JsonValue(step.title);
        }

        step_obj["style"] = style;

        if (!step.geometry.empty()) {
            utils::JsonValue opts = utils::JsonValue::object();
            opts["geometry"] = utils::JsonValue(step.geometry);
            step_obj["options"] = opts;
        }

        steps_array.push_back(step_obj);
    }

    return steps_array.stringify();
}

std::string VizzuIntegration::generate_js_code() const {
    std::stringstream ss;

    ss << "// VZCode Visualization - Generated JavaScript\n";
    ss << "async function initializeVisualization(containerId) {\n";
    ss << "  const chart = new Vizzu(containerId, {\n";
    ss << "    data: " << build_data_json().stringify() << "\n";
    ss << "  });\n\n";

    ss << "  const style = " << build_style_json().stringify() << ";\n";
    ss << "  chart.feature('tooltip', " << (config_.show_tooltip ? "true" : "false") << ");\n\n";

    // Initial animation
    ss << "  await chart.animate({\n";
    ss << "    config: " << build_vizzu_config().stringify() << ",\n";
    ss << "    style: style\n";
    ss << "  });\n\n";

    // Animation steps
    if (!data_.animation_steps.empty()) {
        ss << "  // Animation sequence\n";
        for (size_t i = 0; i < data_.animation_steps.size(); ++i) {
            const auto& step = data_.animation_steps[i];

            ss << "  await chart.animate({\n";
            ss << "    config: {\n";

            if (!step.x.empty()) ss << "      x: '" << step.x << "',\n";
            if (!step.y.empty()) ss << "      y: '" << step.y << "',\n";
            if (!step.color.empty()) ss << "      color: '" << step.color << "',\n";
            if (!step.label.empty()) ss << "      label: '" << step.label << "',\n";
            if (!step.title.empty()) ss << "      title: '" << step.title << "',\n";

            ss << "    },\n";

            if (!step.geometry.empty()) {
                ss << "    style: { plot: { marker: { rectangleSpacing: 0.1 } } }\n";
            }

            ss << "  }, {\n";
            ss << "    duration: " << step.duration_ms << ",\n";
            ss << "    easing: 'cubic-bezier(0.25, 0.1, 0.25, 1)'\n";
            ss << "  });\n\n";
        }
    }

    ss << "  return chart;\n";
    ss << "}\n";

    return ss.str();
}

} // namespace vizzu
} // namespace vzcode
