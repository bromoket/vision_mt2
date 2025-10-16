#include <windows.h>
#include <vision/common/logger.hpp>
#include <vision/common/hook_manager.hpp>

namespace vision {
namespace hooks {
namespace winapi {

// Original function pointer
typedef int (WINAPI* MessageBoxA_t)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
MessageBoxA_t original_message_box_a = nullptr;

// Hook function
int WINAPI hooked_message_box_a(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    LOG_INFO("MessageBoxA hooked! Caption: '{}', Text: '{}'", 
             lpCaption ? lpCaption : "NULL", 
             lpText ? lpText : "NULL");
    
    // Modify the message
    std::string new_text = std::string("HOOKED: ") + (lpText ? lpText : "");
    std::string new_caption = std::string("Vision Hook - ") + (lpCaption ? lpCaption : "");
    
    // Call original function with modified parameters
    return original_message_box_a(hWnd, new_text.c_str(), new_caption.c_str(), uType);
}

bool install_message_box_hook() {
    HMODULE user32 = GetModuleHandleA("user32.dll");
    if (!user32) {
        LOG_ERROR("Failed to get user32.dll handle");
        return false;
    }
    
    FARPROC target = GetProcAddress(user32, "MessageBoxA");
    if (!target) {
        LOG_ERROR("Failed to get MessageBoxA address");
        return false;
    }
    
    return VISION_INSTALL_HOOK("MessageBoxA", target, hooked_message_box_a, original_message_box_a);
}

bool remove_message_box_hook() {
    return VISION_REMOVE_HOOK("MessageBoxA");
}

} // namespace winapi
} // namespace hooks
} // namespace vision
