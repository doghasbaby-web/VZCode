#ifndef ANIMATION_WRAPPER_HPP
#define ANIMATION_WRAPPER_HPP

#include "vizzu_integration.hpp"
#include "../parser/mermaid_parser.hpp"
#include <string>
#include <vector>
#include <functional>

namespace vzcode {
namespace vizzu {

/**
 * Animation transition effect
 */
enum class TransitionEffect {
    Fade,
    Slide,
    Zoom,
    Morph,
    Default
};

/**
 * Animation event
 */
struct AnimationEvent {
    std::string type;  // "start", "step", "complete", "error"
    int step_index;
    std::string message;
    double timestamp;
};

using AnimationEventCallback = std::function<void(const AnimationEvent&)>;

/**
 * Enhanced animation wrapper for Vizzu
 * Provides additional animation control and customization
 */
class AnimationWrapper {
public:
    AnimationWrapper();
    ~AnimationWrapper();

    /**
     * Load data and prepare animation
     */
    bool load_data(const parser::VizzuData& data);

    /**
     * Start animation sequence
     */
    bool start();

    /**
     * Pause animation
     */
    void pause();

    /**
     * Resume animation
     */
    void resume();

    /**
     * Stop and reset animation
     */
    void stop();

    /**
     * Jump to specific animation step
     */
    bool go_to_step(int step_index);

    /**
     * Set transition effect
     */
    void set_transition_effect(TransitionEffect effect);

    /**
     * Set animation speed multiplier (1.0 = normal, 2.0 = double speed, etc.)
     */
    void set_speed(double speed_multiplier);

    /**
     * Register event callback
     */
    void on_event(AnimationEventCallback callback);

    /**
     * Get current step index
     */
    int current_step() const { return current_step_; }

    /**
     * Get total number of steps
     */
    int total_steps() const { return total_steps_; }

    /**
     * Check if animation is playing
     */
    bool is_playing() const { return is_playing_; }

    /**
     * Generate animation control JavaScript code
     */
    std::string generate_control_js() const;

    /**
     * Get Vizzu integration instance
     */
    VizzuIntegration& get_vizzu() { return vizzu_; }

private:
    VizzuIntegration vizzu_;
    int current_step_;
    int total_steps_;
    bool is_playing_;
    TransitionEffect transition_effect_;
    double speed_multiplier_;
    std::vector<AnimationEventCallback> event_callbacks_;

    void emit_event(const AnimationEvent& event);
    std::string transition_effect_to_string(TransitionEffect effect) const;
};

} // namespace vizzu
} // namespace vzcode

#endif // ANIMATION_WRAPPER_HPP
