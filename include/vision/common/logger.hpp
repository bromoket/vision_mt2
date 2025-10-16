#pragma once

#include <fmt/format.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/std.h>
#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <windows.h>
#include <io.h>

namespace vision {
namespace common {

enum class log_level {
    debug = 0,
    info = 1,
    warning = 2,
    error = 3
};

class logger {
public:
    static logger& instance() {
        static logger instance;
        return instance;
    }

    template<typename... Args>
    void debug(const std::string& format, Args&&... args) {
        log(log_level::debug, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(const std::string& format, Args&&... args) {
        log(log_level::info, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warning(const std::string& format, Args&&... args) {
        log(log_level::warning, format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(const std::string& format, Args&&... args) {
        log(log_level::error, format, std::forward<Args>(args)...);
    }

    void set_level(log_level level) {
        std::lock_guard<std::mutex> lock(mutex_);
        current_level_ = level;
    }

    void set_file_output(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (file_stream_.is_open()) {
            file_stream_.close();
        }
        file_stream_.open(filename, std::ios::app);
        file_output_enabled_ = file_stream_.is_open();
    }

    void set_console_output(bool enabled) {
        std::lock_guard<std::mutex> lock(mutex_);
        console_output_enabled_ = enabled;
    }

    void set_colors_enabled(bool enabled) {
        std::lock_guard<std::mutex> lock(mutex_);
        colors_supported_ = enabled;
    }

private:
    logger() : current_level_(log_level::info), console_output_enabled_(true), file_output_enabled_(false), colors_supported_(false) {
        enable_console_colors();
    }
    ~logger() {
        if (file_stream_.is_open()) {
            file_stream_.close();
        }
    }

    void enable_console_colors() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) {
            colors_supported_ = false;
            return;
        }

        // Check if output is redirected
        if (!_isatty(_fileno(stdout))) {
            colors_supported_ = false;
            return;
        }

        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) {
            colors_supported_ = false;
            return;
        }

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (SetConsoleMode(hOut, dwMode)) {
            colors_supported_ = true;
        } else {
            colors_supported_ = false;
        }
    }

    template<typename... Args>
    void log(log_level level, const std::string& format, Args&&... args) {
        if (level < current_level_) {
            return;
        }

        std::lock_guard<std::mutex> lock(mutex_);
        
        auto now = std::chrono::system_clock::now();
        std::string formatted_message;
        
        if constexpr (sizeof...(args) > 0) {
            formatted_message = fmt::format(fmt::runtime(format), std::forward<Args>(args)...);
        } else {
            formatted_message = format;
        }
        
        // Get level info with colors
        std::string level_str;
        fmt::color level_color = fmt::color::white;
        switch (level) {
            case log_level::debug:   
                level_str = "DEBUG"; 
                level_color = fmt::color::cyan; 
                break;
            case log_level::info:    
                level_str = "INFO";  
                level_color = fmt::color::green; 
                break;
            case log_level::warning: 
                level_str = "WARN";  
                level_color = fmt::color::yellow; 
                break;
            case log_level::error:   
                level_str = "ERROR"; 
                level_color = fmt::color::red; 
                break;
        }

        // Console output with colors
        if (console_output_enabled_) {
            if (colors_supported_) {
                try {
                    fmt::print(fg(fmt::color::gray), "[{:%H:%M:%S}] ", now);
                    fmt::print(fg(level_color) | fmt::emphasis::bold, "[{}] ", level_str);
                    fmt::print("{}\n", formatted_message);
                } catch (...) {
                    // Fallback to plain text if colors fail
                    fmt::print("[{:%H:%M:%S}] [{}] {}\n", now, level_str, formatted_message);
                }
            } else {
                // Plain text output when colors not supported
                fmt::print("[{:%H:%M:%S}] [{}] {}\n", now, level_str, formatted_message);
            }
        }

        // File output (plain text)
        if (file_output_enabled_ && file_stream_.is_open()) {
            auto log_line = fmt::format("[{:%Y-%m-%d %H:%M:%S}] [{}] {}\n", 
                                       now, level_str, formatted_message);
            file_stream_ << log_line;
            file_stream_.flush();
        }

#ifdef _DEBUG
        // Debug output (plain text)
        auto debug_line = fmt::format("[{:%H:%M:%S}] [{}] {}\n", 
                                     now, level_str, formatted_message);
        OutputDebugStringA(debug_line.c_str());
#endif
    }

    std::mutex mutex_;
    std::ofstream file_stream_;
    log_level current_level_;
    bool console_output_enabled_;
    bool file_output_enabled_;
    bool colors_supported_;
};

// Convenience macros
#define LOG_DEBUG(...) vision::common::logger::instance().debug(__VA_ARGS__)
#define LOG_INFO(...) vision::common::logger::instance().info(__VA_ARGS__)
#define LOG_WARNING(...) vision::common::logger::instance().warning(__VA_ARGS__)
#define LOG_ERROR(...) vision::common::logger::instance().error(__VA_ARGS__)

} // namespace common
} // namespace vision
