#include "animation_wrapper.hpp"
#include <sstream>
#include <ctime>

namespace vzcode {
namespace vizzu {

AnimationWrapper::AnimationWrapper()
    : current_step_(0),
      total_steps_(0),
      is_playing_(false),
      transition_effect_(TransitionEffect::Default),
      speed_multiplier_(1.0) {}

AnimationWrapper::~AnimationWrapper() {}

bool AnimationWrapper::load_data(const parser::VizzuData& data) {
    bool success = vizzu_.initialize(data, ChartConfig());
    if (success) {
        total_steps_ = static_cast<int>(data.animation_steps.size());
        current_step_ = 0;
    }
    return success;
}

bool AnimationWrapper::start() {
    if (total_steps_ == 0) return false;

    is_playing_ = true;
    current_step_ = 0;

    AnimationEvent event;
    event.type = "start";
    event.step_index = 0;
    event.message = "Animation started";
    event.timestamp = static_cast<double>(std::time(nullptr));

    emit_event(event);

    return true;
}

void AnimationWrapper::pause() {
    if (is_playing_) {
        is_playing_ = false;

        AnimationEvent event;
        event.type = "pause";
        event.step_index = current_step_;
        event.message = "Animation paused";
        event.timestamp = static_cast<double>(std::time(nullptr));

        emit_event(event);
    }
}

void AnimationWrapper::resume() {
    if (!is_playing_) {
        is_playing_ = true;

        AnimationEvent event;
        event.type = "resume";
        event.step_index = current_step_;
        event.message = "Animation resumed";
        event.timestamp = static_cast<double>(std::time(nullptr));

        emit_event(event);
    }
}

void AnimationWrapper::stop() {
    is_playing_ = false;
    current_step_ = 0;

    AnimationEvent event;
    event.type = "stop";
    event.step_index = 0;
    event.message = "Animation stopped";
    event.timestamp = static_cast<double>(std::time(nullptr));

    emit_event(event);
}

bool AnimationWrapper::go_to_step(int step_index) {
    if (step_index < 0 || step_index >= total_steps_) {
        return false;
    }

    current_step_ = step_index;

    AnimationEvent event;
    event.type = "step";
    event.step_index = step_index;
    event.message = "Jumped to step " + std::to_string(step_index);
    event.timestamp = static_cast<double>(std::time(nullptr));

    emit_event(event);

    return true;
}

void AnimationWrapper::set_transition_effect(TransitionEffect effect) {
    transition_effect_ = effect;
}

void AnimationWrapper::set_speed(double speed_multiplier) {
    speed_multiplier_ = speed_multiplier;
}

void AnimationWrapper::on_event(AnimationEventCallback callback) {
    event_callbacks_.push_back(callback);
}

void AnimationWrapper::emit_event(const AnimationEvent& event) {
    for (const auto& callback : event_callbacks_) {
        callback(event);
    }
}

std::string AnimationWrapper::transition_effect_to_string(TransitionEffect effect) const {
    switch (effect) {
        case TransitionEffect::Fade: return "fade";
        case TransitionEffect::Slide: return "slide";
        case TransitionEffect::Zoom: return "zoom";
        case TransitionEffect::Morph: return "morph";
        case TransitionEffect::Default: return "default";
        default: return "default";
    }
}

std::string AnimationWrapper::generate_control_js() const {
    std::stringstream ss;

    ss << "// Animation Control Functions\n";
    ss << "class AnimationController {\n";
    ss << "  constructor(chart) {\n";
    ss << "    this.chart = chart;\n";
    ss << "    this.currentStep = 0;\n";
    ss << "    this.totalSteps = " << total_steps_ << ";\n";
    ss << "    this.isPlaying = false;\n";
    ss << "    this.speed = " << speed_multiplier_ << ";\n";
    ss << "  }\n\n";

    ss << "  async start() {\n";
    ss << "    this.isPlaying = true;\n";
    ss << "    this.currentStep = 0;\n";
    ss << "    await this.playStep();\n";
    ss << "  }\n\n";

    ss << "  pause() {\n";
    ss << "    this.isPlaying = false;\n";
    ss << "  }\n\n";

    ss << "  resume() {\n";
    ss << "    this.isPlaying = true;\n";
    ss << "    this.playStep();\n";
    ss << "  }\n\n";

    ss << "  stop() {\n";
    ss << "    this.isPlaying = false;\n";
    ss << "    this.currentStep = 0;\n";
    ss << "  }\n\n";

    ss << "  async playStep() {\n";
    ss << "    if (!this.isPlaying || this.currentStep >= this.totalSteps) {\n";
    ss << "      this.isPlaying = false;\n";
    ss << "      return;\n";
    ss << "    }\n\n";

    ss << "    // Trigger animation step\n";
    ss << "    const duration = 1000 / this.speed;\n";
    ss << "    // await this.chart.animate(...);\n\n";

    ss << "    this.currentStep++;\n";
    ss << "    if (this.currentStep < this.totalSteps) {\n";
    ss << "      setTimeout(() => this.playStep(), duration);\n";
    ss << "    }\n";
    ss << "  }\n\n";

    ss << "  goToStep(step) {\n";
    ss << "    if (step >= 0 && step < this.totalSteps) {\n";
    ss << "      this.currentStep = step;\n";
    ss << "      // Trigger step change\n";
    ss << "    }\n";
    ss << "  }\n\n";

    ss << "  setSpeed(speed) {\n";
    ss << "    this.speed = speed;\n";
    ss << "  }\n";
    ss << "}\n";

    return ss.str();
}

} // namespace vizzu
} // namespace vzcode
