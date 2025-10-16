#include <windows.h>
#include <iostream>
#include <exception>
#include <vision/common/logger.hpp>
#include <memory/c_memory.hpp>
#include <ui/d3d9_window.hpp>
#include <ui/imgui_manager.hpp>

int main() {
    try {
        // Initialize logger
        vision::common::logger::instance().set_file_output("vision_external.log");
        vision::common::logger::instance().set_level(vision::common::log_level::debug);
        
        LOG_INFO("Vision External starting up");
        LOG_INFO("Process ID: {}", GetCurrentProcessId());
        
        std::cout << "Vision - Modern Metin2 Enhancement Tool\n";
        std::cout << "Version: 1.0.0\n";
        std::cout << "Architecture: 32-bit\n\n";
        
        LOG_DEBUG("Debug logging enabled");
        LOG_INFO("Application initialized successfully");
        
        // Initialize D3D9 window and ImGui
        auto& window = vision::ui::d3d9_window::instance();
        if (!window.initialize("Vision - Modern Metin2 Enhancement Tool", 640, 360)) {
            LOG_ERROR("Failed to initialize D3D9 window");
            std::cout << "ERROR: Failed to initialize D3D9 window. Check the log file for details.\n";
            std::cout << "Possible causes:\n";
            std::cout << "1. DirectX 9 is not installed or not available\n";
            std::cout << "2. Graphics drivers are outdated\n";
            std::cout << "3. Hardware acceleration is disabled\n";
            std::cout << "4. Missing DLL dependencies\n\n";
            std::cout << "Press any key to exit...";
            std::cin.get();
            return 1;
        }
        
        LOG_INFO("ImGui interface initialized");
        std::cout << "ImGui interface started. Press INSERT to toggle menu visibility.\n";
        
        // Main application loop
        while (window.is_running()) {
            window.process_messages();
            window.present();
            
            // Small sleep to prevent 100% CPU usage
            Sleep(1);
        }
        
        // Cleanup
        window.shutdown();
        LOG_INFO("Vision External shutting down");
        
    } catch (const std::exception& e) {
        LOG_ERROR("Unhandled exception: {}", e.what());
        std::cout << "FATAL ERROR: " << e.what() << "\n";
        std::cout << "Press any key to exit...";
        std::cin.get();
        return 1;
    } catch (...) {
        LOG_ERROR("Unknown exception occurred");
        std::cout << "FATAL ERROR: Unknown exception occurred\n";
        std::cout << "Press any key to exit...";
        std::cin.get();
        return 1;
    }
    
    return 0;
}
