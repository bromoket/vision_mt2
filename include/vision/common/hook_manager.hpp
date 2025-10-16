#pragma once

#include <MinHook.h>
#include <vision/common/logger.hpp>
#include <unordered_map>
#include <string>
#include <functional>

namespace vision {
namespace common {

class hook_manager {
public:
    static hook_manager& instance() {
        static hook_manager instance;
        return instance;
    }

    bool initialize() {
        if (initialized_) return true;
        
        MH_STATUS status = MH_Initialize();
        if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
            LOG_ERROR("Failed to initialize MinHook: {}", MH_StatusToString(status));
            return false;
        }
        
        initialized_ = true;
        LOG_INFO("Hook manager initialized successfully");
        return true;
    }

    bool install_hook(const std::string& name, void* target, void* detour, void** original) {
        if (!initialized_) {
            LOG_ERROR("Hook manager not initialized");
            return false;
        }

        MH_STATUS status = MH_CreateHook(target, detour, original);
        if (status != MH_OK) {
            LOG_ERROR("Failed to create hook '{}': {}", name, MH_StatusToString(status));
            return false;
        }

        status = MH_EnableHook(target);
        if (status != MH_OK) {
            LOG_ERROR("Failed to enable hook '{}': {}", name, MH_StatusToString(status));
            MH_RemoveHook(target);
            return false;
        }

        hooks_[name] = target;
        LOG_INFO("Hook '{}' installed successfully at 0x{:X}", name, reinterpret_cast<uintptr_t>(target));
        return true;
    }

    bool remove_hook(const std::string& name) {
        auto it = hooks_.find(name);
        if (it == hooks_.end()) {
            LOG_WARNING("Hook '{}' not found for removal", name);
            return false;
        }

        MH_STATUS status = MH_DisableHook(it->second);
        if (status != MH_OK) {
            LOG_ERROR("Failed to disable hook '{}': {}", name, MH_StatusToString(status));
        }

        status = MH_RemoveHook(it->second);
        if (status != MH_OK) {
            LOG_ERROR("Failed to remove hook '{}': {}", name, MH_StatusToString(status));
            return false;
        }

        hooks_.erase(it);
        LOG_INFO("Hook '{}' removed successfully", name);
        return true;
    }

    void remove_all_hooks() {
        for (const auto& hook : hooks_) {
            MH_DisableHook(hook.second);
            MH_RemoveHook(hook.second);
        }
        hooks_.clear();
        LOG_INFO("All hooks removed");
    }

    void shutdown() {
        if (!initialized_) return;
        
        remove_all_hooks();
        MH_Uninitialize();
        initialized_ = false;
        LOG_INFO("Hook manager shutdown");
    }

    ~hook_manager() {
        shutdown();
    }

private:
    hook_manager() : initialized_(false) {}
    
    bool initialized_;
    std::unordered_map<std::string, void*> hooks_;
};

} // namespace common
} // namespace vision

// Convenient macros for hook installation
#define VISION_HOOK_MANAGER vision::common::hook_manager::instance()

#define VISION_INSTALL_HOOK(name, target_func, detour_func, original_func) \
    VISION_HOOK_MANAGER.install_hook(name, reinterpret_cast<void*>(target_func), \
                                     reinterpret_cast<void*>(detour_func), \
                                     reinterpret_cast<void**>(&original_func))

#define VISION_REMOVE_HOOK(name) \
    VISION_HOOK_MANAGER.remove_hook(name)

// Helper macro for WinAPI hooks
#define VISION_HOOK_WINAPI(api_name, detour_func, original_func) \
    VISION_INSTALL_HOOK(#api_name, GetProcAddress(GetModuleHandleA("kernel32.dll"), #api_name), \
                       detour_func, original_func)

// Helper macro for module-based hooks  
#define VISION_HOOK_MODULE(module_name, func_name, detour_func, original_func) \
    VISION_INSTALL_HOOK(#func_name, GetProcAddress(GetModuleHandleA(module_name), #func_name), \
                       detour_func, original_func)
