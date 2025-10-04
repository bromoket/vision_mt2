#pragma once

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>

namespace vision::logger {

enum class log_level : int {
    debug = 0,
    info = 1,
    warning = 2,
    error = 3
};

class c_logger {
public:
    c_logger() = default;
    ~c_logger() = default;

    // Non-copyable, non-movable
    c_logger(const c_logger&) = delete;
    c_logger& operator=(const c_logger&) = delete;
    c_logger(c_logger&&) = delete;
    c_logger& operator=(c_logger&&) = delete;

    void initialize(const std::string& name, log_level level = log_level::info, bool to_file = true);
    void shutdown();

    template <typename... Args>
    void log_debug(fmt::format_string<Args...> format_str, Args&&... args) {
        log(log_level::debug, format_str, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void log_info(fmt::format_string<Args...> format_str, Args&&... args) {
        log(log_level::info, format_str, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void log_warning(fmt::format_string<Args...> format_str, Args&&... args) {
        log(log_level::warning, format_str, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void log_error(fmt::format_string<Args...> format_str, Args&&... args) {
        log(log_level::error, format_str, std::forward<Args>(args)...);
    }

private:
    template <typename... Args>
    void log(log_level level, fmt::format_string<Args...> format_str, Args&&... args) {
        if (level < m_min_level) {
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        
        const auto message = fmt::format(format_str, std::forward<Args>(args)...);
        const auto timestamp = get_timestamp();
        const auto level_str = level_to_string(level);
        
        // Console output with colors
        if (m_console_enabled) {
            const auto color = level_to_color(level);
            fmt::print(stderr, "[{}] [{}] [{}] {}\n", 
                      fmt::styled(timestamp, fmt::fg(fmt::color::gray)),
                      fmt::styled(level_str, fmt::fg(color)),
                      fmt::styled(m_name, fmt::fg(fmt::color::cyan)),
                      message);
        }
        
        // File output
        if (m_file_enabled && m_file_stream.is_open()) {
            m_file_stream << fmt::format("[{}] [{}] [{}] {}\n", timestamp, level_str, m_name, message);
            m_file_stream.flush();
        }
    }

    std::string get_timestamp() const;
    std::string level_to_string(log_level level) const;
    fmt::color level_to_color(log_level level) const;
    void create_log_directory() const;

    std::string m_name;
    log_level m_min_level = log_level::info;
    bool m_console_enabled = true;
    bool m_file_enabled = false;
    std::ofstream m_file_stream;
    mutable std::mutex m_mutex;
};

// Global logger instance
inline std::unique_ptr<c_logger> g_logger = std::make_unique<c_logger>();

// Convenience functions
inline void initialize(const std::string& name, log_level level = log_level::info, bool to_file = true) {
    g_logger->initialize(name, level, to_file);
}

inline void shutdown() {
    g_logger->shutdown();
}

template <typename... Args>
void log_debug(fmt::format_string<Args...> format_str, Args&&... args) {
    g_logger->log_debug(format_str, std::forward<Args>(args)...);
}

template <typename... Args>
void log_info(fmt::format_string<Args...> format_str, Args&&... args) {
    g_logger->log_info(format_str, std::forward<Args>(args)...);
}

template <typename... Args>
void log_warning(fmt::format_string<Args...> format_str, Args&&... args) {
    g_logger->log_warning(format_str, std::forward<Args>(args)...);
}

template <typename... Args>
void log_error(fmt::format_string<Args...> format_str, Args&&... args) {
    g_logger->log_error(format_str, std::forward<Args>(args)...);
}

}  // namespace vision::logger
