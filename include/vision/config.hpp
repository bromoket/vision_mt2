#pragma once

#include <string_view>

namespace vision::config {

// Version information
constexpr std::string_view version = "1.0.0";
constexpr std::string_view build_date = __DATE__ " " __TIME__;

// Debug configuration
#ifdef DEBUG
constexpr bool debug_mode = true;
#else
constexpr bool debug_mode = false;
#endif

// Logging configuration
constexpr bool enable_file_logging = true;
constexpr bool enable_console_logging = true;
constexpr std::string_view log_directory = "logs";

// Memory scanning configuration
constexpr std::size_t pattern_scan_alignment = 1;
constexpr std::size_t max_scan_size = 0x10000000;  // 256MB

// Process configuration
constexpr std::string_view target_process_name = "metin2client.exe";
constexpr std::string_view target_window_class = "METIN2_GAME";

// DLL configuration
constexpr std::string_view internal_dll_name = "vision_internal.dll";

// Feature toggles
namespace features {
constexpr bool enable_pattern_caching = true;
constexpr bool enable_address_validation = true;
constexpr bool enable_debug_console = debug_mode;
constexpr bool enable_imgui_demo = debug_mode;
}  // namespace features

}  // namespace vision::config
