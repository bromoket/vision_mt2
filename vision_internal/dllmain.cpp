#include <vision/config.hpp>
#include <vision/logger.hpp>
#include <vision/result.hpp>
#include <vision/utils/memory/memory.hpp>
#include <vision/utils/xorstr.hpp>
#include <vision/metin2_sdk/sdk.hpp>

#include <windows.h>
#include <thread>

using namespace vision;
using namespace vision::utils::memory;

// Global variables
static bool g_initialized = false;
static std::thread g_main_thread;

// Forward declarations
void main_thread();
result<void> initialize_vision();
void shutdown_vision();
result<void> load_addresses();

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // Disable thread notifications for performance
        ::DisableThreadLibraryCalls(hModule);
        
        // Start main thread
        g_main_thread = std::thread(main_thread);
        break;
        
    case DLL_PROCESS_DETACH:
        // Cleanup
        shutdown_vision();
        
        if (g_main_thread.joinable()) {
            g_main_thread.join();
        }
        break;
        
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    
    return TRUE;
}

void main_thread() {
    // Initialize vision system
    auto init_result = initialize_vision();
    if (!init_result) {
        // Can't log yet, so just return
        return;
    }
    
    logger::log_info("Vision internal DLL loaded successfully");
    logger::log_info("Version: {}", config::version);
    logger::log_info("Build date: {}", config::build_date);
    
    // Load game addresses
    auto load_result = load_addresses();
    if (!load_result) {
        logger::log_error("Failed to load addresses: {}", load_result.message());
        return;
    }
    
    logger::log_info("All addresses loaded successfully");
    
    // Main loop - keep the DLL alive
    while (g_initialized) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Add your main logic here
        // Example: monitor game state, hook functions, etc.
    }
    
    logger::log_info("Vision internal shutting down");
}

result<void> initialize_vision() {
    try {
        // Initialize logger with secure string
        auto logger_name = xorstr_secure("vision_internal");
        logger::initialize(logger_name.get(), logger::log_level::debug, config::enable_file_logging);
        
        g_initialized = true;
        return result<void>::ok();
    } catch (const std::exception& e) {
        return result<void>::error("Initialization failed: " + std::string(e.what()));
    }
}

void shutdown_vision() {
    g_initialized = false;
    logger::shutdown();
}

result<void> load_addresses() {
    logger::log_info("Loading game addresses...");
    
    try {
        // Get main module
        c_memory main_module;
        if (!main_module.is_valid()) {
            return result<void>::error("Failed to get main module");
        }
        
        logger::log_info("Main module base: 0x{:X}", main_module.base_address());
        logger::log_info("Main module size: 0x{:X}", main_module.size());
        
        // Example: Find a pattern
        auto pattern_result = main_module.find_pattern(metin2_sdk::signatures::send_packet);
        if (pattern_result) {
            logger::log_info("Found send_packet at: 0x{:X}", pattern_result->raw());
        } else {
            logger::log_warning("send_packet pattern not found: {}", pattern_result.message());
        }
        
        // Example: Find a string using secure XOR
        xorstr_call("Send Battle", [&](const char* search_str) {
            auto string_result = main_module.find_string(search_str);
            if (string_result) {
                logger::log_info("Found '{}' string at: 0x{:X}", search_str, string_result->raw());
                
                // Example: Read the string to verify
                auto str_ptr = string_result->as<const char*>();
                logger::log_debug("String content: '{}'", str_ptr);
            } else {
                logger::log_warning("'{}' string not found: {}", search_str, string_result.message());
            }
        });
        
        // Example: Get an export (from a system DLL) using secure XOR
        c_memory ntdll("ntdll.dll");
        if (ntdll.is_valid()) {
            xorstr_call("NtQueryInformationProcess", [&](const char* export_name) {
                auto export_result = ntdll.get_export(export_name);
                if (export_result) {
                    logger::log_info("Found {} at: 0x{:X}", export_name, export_result->raw());
                }
            });
        }
        
        return result<void>::ok();
    } catch (const std::exception& e) {
        return result<void>::error("Exception during address loading: " + std::string(e.what()));
    }
}
