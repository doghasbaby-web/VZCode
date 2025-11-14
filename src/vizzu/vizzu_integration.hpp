#ifndef VIZZU_INTEGRATION_HPP
#define VIZZU_INTEGRATION_HPP

#include "../parser/vizzu_converter.hpp"
#include "../utils/json_utils.hpp"
#include <string>
#include <vector>
#include <memory>

namespace vzcode {
namespace vizzu {

/**
 * Chart configuration for Vizzu
 */
struct ChartConfig {
    int width = 800;
    int height = 600;
    std::string title;
    std::string theme = "default";
    bool show_tooltip = true;
    bool show_legend = true;
};

/**
 * Animation state for transitions
 */
struct AnimationState {
    int current_step = 0;
    int total_steps = 0;
    bool is_playing = false;
    bool loop = false;
    int delay_between_steps_ms = 500;
};

/**
 * Integration layer for Vizzu library
 * Wraps Vizzu functionality with VZCode-specific features
 */
class VizzuIntegration {
public:
    VizzuIntegration();
    ~VizzuIntegration();

    /**
     * Initialize Vizzu with data
     */
    bool initialize(const parser::VizzuData& data, const ChartConfig& config);

    /**
     * Get initialization config as JSON for JavaScript
     */
    std::string get_init_config_json() const;

    /**
     * Get animation steps as JSON array
     */
    std::string get_animation_steps_json() const;

    /**
     * Set chart configuration
     */
    void set_config(const ChartConfig& config);

    /**
     * Get current chart config
     */
    const ChartConfig& get_config() const { return config_; }

    /**
     * Set animation state
     */
    void set_animation_state(const AnimationState& state);

    /**
     * Get current animation state
     */
    const AnimationState& get_animation_state() const { return animation_state_; }

    /**
     * Generate complete Vizzu JavaScript initialization code
     */
    std::string generate_js_code() const;

private:
    parser::VizzuData data_;
    ChartConfig config_;
    AnimationState animation_state_;

    utils::JsonValue build_vizzu_config() const;
    utils::JsonValue build_data_json() const;
    utils::JsonValue build_style_json() const;
};

} // namespace vizzu
} // namespace vzcode

#endif // VIZZU_INTEGRATION_HPP
