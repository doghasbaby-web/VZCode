#ifndef VIZZU_KERNEL_WRAPPER_HPP
#define VIZZU_KERNEL_WRAPPER_HPP

#include "../parser/vizzu_converter.hpp"
#include <string>
#include <vector>
#include <memory>

// Forward declarations for Vizzu C++ API
namespace Vizzu {
    class Interface;
}

namespace vzcode {
namespace vizzu {

/**
 * Low-level wrapper around Vizzu C++ kernel
 * This wraps the actual Vizzu library C++ code, providing
 * a C++ shell to interface with the Vizzu rendering engine
 */
class VizzuKernelWrapper {
public:
    VizzuKernelWrapper();
    ~VizzuKernelWrapper();

    /**
     * Initialize Vizzu chart instance
     */
    bool initialize();

    /**
     * Add data dimension to Vizzu chart
     */
    bool add_dimension(const std::string& name, const std::vector<std::string>& categories);

    /**
     * Add data measure to Vizzu chart
     */
    bool add_measure(const std::string& name, const std::vector<double>& values, const std::string& unit = "");

    /**
     * Add a record (row) to Vizzu chart
     */
    bool add_record(const std::vector<std::string>& cells);

    /**
     * Set chart configuration via JSON-like path
     * e.g., "config.channels.x" = "Component"
     */
    bool set_config(const std::string& path, const std::string& value);

    /**
     * Set chart style via path
     */
    bool set_style(const std::string& path, const std::string& value);

    /**
     * Begin animation keyframe
     */
    bool begin_keyframe();

    /**
     * Set animation configuration
     */
    bool set_animation_config(const std::string& path, const std::string& value);

    /**
     * Set animation duration
     */
    bool set_animation_duration(double duration_ms);

    /**
     * Trigger animation
     */
    bool animate();

    /**
     * Update chart (for animation tick)
     */
    bool update(double time_ms);

    /**
     * Render chart to canvas
     */
    bool render(double width, double height);

    /**
     * Get Vizzu version
     */
    std::string get_version() const;

    /**
     * Load data from VizzuData structure
     */
    bool load_vizzu_data(const parser::VizzuData& data);

    /**
     * Store current chart state (snapshot)
     */
    bool store_snapshot();

    /**
     * Restore chart from snapshot
     */
    bool restore_snapshot();

    /**
     * Free resources
     */
    void cleanup();

    /**
     * Check if kernel is initialized
     */
    bool is_initialized() const { return initialized_; }

    /**
     * Get last error message
     */
    std::string get_last_error() const { return last_error_; }

private:
    bool initialized_;
    std::string last_error_;

    // Opaque handles to Vizzu C++ objects
    void* chart_handle_;
    void* canvas_handle_;
    void* snapshot_handle_;

    // Vizzu interface instance
    Vizzu::Interface* vizzu_interface_;

    void set_error(const std::string& error);
};

} // namespace vizzu
} // namespace vzcode

#endif // VIZZU_KERNEL_WRAPPER_HPP
