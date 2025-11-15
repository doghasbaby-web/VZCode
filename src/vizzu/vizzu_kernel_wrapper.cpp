#include "vizzu_kernel_wrapper.hpp"

#ifdef VIZZU_KERNEL_AVAILABLE
// Include Vizzu C++ library headers
#include "../../third_party/vizzu-lib/src/apps/weblib/cinterface.h"
#include "../../third_party/vizzu-lib/src/apps/weblib/interface.h"
#endif

#include <iostream>
#include <cstring>

namespace vzcode {
namespace vizzu {

VizzuKernelWrapper::VizzuKernelWrapper()
    : initialized_(false),
      chart_handle_(nullptr),
      canvas_handle_(nullptr),
      snapshot_handle_(nullptr),
      vizzu_interface_(nullptr) {}

VizzuKernelWrapper::~VizzuKernelWrapper() {
    cleanup();
}

bool VizzuKernelWrapper::initialize() {
#ifdef VIZZU_KERNEL_AVAILABLE
    try {
        // Get Vizzu interface singleton
        vizzu_interface_ = &Vizzu::Interface::getInstance();

        // Create chart instance
        chart_handle_ = reinterpret_cast<void*>(vizzu_interface_->createChart());

        // Create canvas instance
        canvas_handle_ = reinterpret_cast<void*>(vizzu_interface_->createCanvas());

        if (!chart_handle_ || !canvas_handle_) {
            set_error("Failed to create Vizzu chart or canvas");
            return false;
        }

        initialized_ = true;
        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Vizzu initialization error: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available - compiled without VIZZU_KERNEL_AVAILABLE");
    return false;
#endif
}

bool VizzuKernelWrapper::add_dimension(const std::string& name,
                                       const std::vector<std::string>& categories) {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        // Convert categories to C string array
        std::vector<const char*> category_ptrs;
        for (const auto& cat : categories) {
            category_ptrs.push_back(cat.c_str());
        }

        // Create indices (sequential for simple case)
        std::vector<std::uint32_t> indices(categories.size());
        for (size_t i = 0; i < categories.size(); ++i) {
            indices[i] = static_cast<std::uint32_t>(i);
        }

        vizzu_interface_->addDimension(
            reinterpret_cast<ObjectRegistryHandle>(chart_handle_),
            name.c_str(),
            category_ptrs.data(),
            static_cast<std::uint32_t>(categories.size()),
            indices.data(),
            static_cast<std::uint32_t>(indices.size()),
            true  // isContiguous
        );

        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to add dimension: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

bool VizzuKernelWrapper::add_measure(const std::string& name,
                                     const std::vector<double>& values,
                                     const std::string& unit) {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        vizzu_interface_->addMeasure(
            reinterpret_cast<ObjectRegistryHandle>(chart_handle_),
            name.c_str(),
            unit.empty() ? "" : unit.c_str(),
            values.data(),
            static_cast<std::uint32_t>(values.size())
        );

        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to add measure: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

bool VizzuKernelWrapper::add_record(const std::vector<std::string>& cells) {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        std::vector<const char*> cell_ptrs;
        for (const auto& cell : cells) {
            cell_ptrs.push_back(cell.c_str());
        }

        vizzu_interface_->addRecord(
            reinterpret_cast<ObjectRegistryHandle>(chart_handle_),
            cell_ptrs.data(),
            static_cast<std::uint32_t>(cells.size())
        );

        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to add record: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

bool VizzuKernelWrapper::set_config(const std::string& path, const std::string& value) {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        vizzu_interface_->setChartValue(
            reinterpret_cast<ObjectRegistryHandle>(chart_handle_),
            path.c_str(),
            value.c_str()
        );
        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to set config: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

bool VizzuKernelWrapper::set_style(const std::string& path, const std::string& value) {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        vizzu_interface_->setStyleValue(
            reinterpret_cast<ObjectRegistryHandle>(chart_handle_),
            path.c_str(),
            value.c_str()
        );
        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to set style: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

bool VizzuKernelWrapper::begin_keyframe() {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        vizzu_interface_->setKeyframe(
            reinterpret_cast<ObjectRegistryHandle>(chart_handle_)
        );
        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to set keyframe: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

bool VizzuKernelWrapper::set_animation_config(const std::string& path, const std::string& value) {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        vizzu_interface_->setAnimValue(
            reinterpret_cast<ObjectRegistryHandle>(chart_handle_),
            path.c_str(),
            value.c_str()
        );
        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to set animation config: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

bool VizzuKernelWrapper::set_animation_duration(double duration_ms) {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        std::string duration_str = std::to_string(static_cast<int>(duration_ms)) + "ms";
        vizzu_interface_->setAnimControlValue(
            reinterpret_cast<ObjectRegistryHandle>(chart_handle_),
            "duration",
            duration_str.c_str()
        );
        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to set animation duration: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

bool VizzuKernelWrapper::animate() {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        // Trigger animation with a simple callback
        vizzu_interface_->animate(
            reinterpret_cast<ObjectRegistryHandle>(chart_handle_),
            [](bool success) {
                // Animation callback
                if (!success) {
                    std::cerr << "Animation failed" << std::endl;
                }
            }
        );
        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to animate: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

bool VizzuKernelWrapper::update(double time_ms) {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        vizzu_interface_->update(
            reinterpret_cast<ObjectRegistryHandle>(chart_handle_),
            time_ms
        );
        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to update: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

bool VizzuKernelWrapper::render(double width, double height) {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        vizzu_interface_->render(
            reinterpret_cast<ObjectRegistryHandle>(chart_handle_),
            reinterpret_cast<ObjectRegistryHandle>(canvas_handle_),
            width,
            height
        );
        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to render: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

std::string VizzuKernelWrapper::get_version() const {
#ifdef VIZZU_KERNEL_AVAILABLE
    try {
        return Vizzu::Interface::version();
    } catch (...) {
        return "unknown";
    }
#else
    return "kernel-not-available";
#endif
}

bool VizzuKernelWrapper::load_vizzu_data(const parser::VizzuData& data) {
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        // Add all series from VizzuData
        for (const auto& series : data.series) {
            if (series.type == "dimension") {
                if (!add_dimension(series.name, series.values)) {
                    return false;
                }
            } else if (series.type == "measure") {
                std::vector<double> numeric_values;
                for (const auto& val_str : series.values) {
                    try {
                        numeric_values.push_back(std::stod(val_str));
                    } catch (...) {
                        numeric_values.push_back(0.0);
                    }
                }
                if (!add_measure(series.name, numeric_values)) {
                    return false;
                }
            }
        }

        // Set initial configuration from data.config
        for (const auto& [key, value] : data.config) {
            set_config("config." + key, value);
        }

        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to load Vizzu data: ") + e.what());
        return false;
    }
}

bool VizzuKernelWrapper::store_snapshot() {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_) {
        set_error("Vizzu not initialized");
        return false;
    }

    try {
        snapshot_handle_ = reinterpret_cast<void*>(
            vizzu_interface_->storeChart(
                reinterpret_cast<ObjectRegistryHandle>(chart_handle_)
            )
        );
        return snapshot_handle_ != nullptr;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to store snapshot: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

bool VizzuKernelWrapper::restore_snapshot() {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (!initialized_ || !snapshot_handle_) {
        set_error("Vizzu not initialized or no snapshot available");
        return false;
    }

    try {
        vizzu_interface_->restoreChart(
            reinterpret_cast<ObjectRegistryHandle>(chart_handle_),
            reinterpret_cast<ObjectRegistryHandle>(snapshot_handle_)
        );
        return true;
    } catch (const std::exception& e) {
        set_error(std::string("Failed to restore snapshot: ") + e.what());
        return false;
    }
#else
    set_error("Vizzu kernel not available");
    return false;
#endif
}

void VizzuKernelWrapper::cleanup() {
#ifdef VIZZU_KERNEL_AVAILABLE
    if (vizzu_interface_) {
        if (snapshot_handle_) {
            vizzu_interface_->freeObj(reinterpret_cast<ObjectRegistryHandle>(snapshot_handle_));
            snapshot_handle_ = nullptr;
        }
        if (canvas_handle_) {
            vizzu_interface_->freeObj(reinterpret_cast<ObjectRegistryHandle>(canvas_handle_));
            canvas_handle_ = nullptr;
        }
        if (chart_handle_) {
            vizzu_interface_->freeObj(reinterpret_cast<ObjectRegistryHandle>(chart_handle_));
            chart_handle_ = nullptr;
        }
    }
#endif
    initialized_ = false;
    vizzu_interface_ = nullptr;
}

void VizzuKernelWrapper::set_error(const std::string& error) {
    last_error_ = error;
    std::cerr << "VizzuKernelWrapper Error: " << error << std::endl;
}

} // namespace vizzu
} // namespace vzcode
