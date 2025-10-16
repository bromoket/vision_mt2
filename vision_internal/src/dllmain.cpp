#include <windows.h>
#include <vision/common/logger.hpp>
#include <vision/common/dependencies.hpp>
#include <vision/common/hook_manager.hpp>
#include <hooks/winapi/message_box_hook.hpp>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        
        // Initialize logger
        vision::common::logger::instance().set_file_output("vision_internal.log");
        
        LOG_INFO("Vision Internal DLL loaded successfully");
        LOG_INFO("Process ID: {}", GetCurrentProcessId());
        LOG_INFO("Module Base: 0x{:X}", reinterpret_cast<uintptr_t>(hModule));
        
        // Test dependencies
        if (vision::common::verify_dependencies()) {
            LOG_INFO("All dependencies verified successfully (MinHook, Zydis, fmt, JSON)");
        } else {
            LOG_ERROR("Dependency verification failed!");
        }
        
        // Initialize hook manager
        if (VISION_HOOK_MANAGER.initialize()) {
            LOG_INFO("Hook manager initialized");
            
            // Install test hook
            if (vision::hooks::winapi::install_message_box_hook()) {
                LOG_INFO("MessageBox hook installed successfully");
                
                // Test the hook
                MessageBoxA(NULL, "This is a test message", "Test", MB_OK);
            }
        }
        
        break;
        
    case DLL_PROCESS_DETACH:
        LOG_INFO("Vision Internal DLL unloading");
        break;
        
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    
    return TRUE;
}
