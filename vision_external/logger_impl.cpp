#include <vision/logger.hpp>

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace vision::logger {

std::string c_logger::get_timestamp() const {
    const auto now = std::chrono::system_clock::now();
    const auto time_t = std::chrono::system_clock::to_time_t(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string c_logger::level_to_string(log_level level) const {
    switch (level) {
    case log_level::debug:   return "DEBUG";
    case log_level::info:    return "INFO ";
    case log_level::warning: return "WARN ";
    case log_level::error:   return "ERROR";
    default:                 return "UNKN ";
    }
}

fmt::color c_logger::level_to_color(log_level level) const {
    switch (level) {
    case log_level::debug:   return fmt::color::white;
    case log_level::info:    return fmt::color::light_blue;
    case log_level::warning: return fmt::color::yellow;
    case log_level::error:   return fmt::color::red;
    default:                 return fmt::color::white;
    }
}

void c_logger::create_log_directory() const {
    try {
        std::filesystem::create_directories("logs");
    } catch (...) {
        // Ignore errors - logging will just go to console
    }
}

void c_logger::initialize(const std::string& name, log_level level, bool to_file) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_name = name;
    m_min_level = level;
    m_console_enabled = true;
    m_file_enabled = to_file;
    
    if (m_file_enabled) {
        create_log_directory();
        
        const auto log_filename = fmt::format("logs/{}.log", name);
        m_file_stream.open(log_filename, std::ios::out | std::ios::app);
        
        if (m_file_stream.is_open()) {
            m_file_stream << fmt::format("\n=== Logger initialized at {} ===\n", get_timestamp());
            m_file_stream.flush();
        } else {
            m_file_enabled = false;
        }
    }
}

void c_logger::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_file_enabled && m_file_stream.is_open()) {
        m_file_stream << fmt::format("=== Logger shutdown at {} ===\n", get_timestamp());
        m_file_stream.close();
    }
    
    m_file_enabled = false;
    m_console_enabled = false;
}

}  // namespace vision::logger
