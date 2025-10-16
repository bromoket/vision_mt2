#pragma once

#include <windows.h>
#include <d3d9.h>
#include <memory>
#include <vision/common/logger.hpp>

namespace vision {
namespace ui {

class d3d9_window {
public:
    static d3d9_window& instance() {
        static d3d9_window instance;
        return instance;
    }

    bool initialize(const std::string& window_title = "Vision Overlay", 
                   int width = 800, int height = 600);
    void shutdown();
    
    bool is_running() const { return running_; }
    void process_messages();
    void present();
    
    HWND get_hwnd() const { return hwnd_; }
    IDirect3DDevice9* get_device() const { return device_; }
    
    // Window state
    void set_topmost(bool topmost);
    void set_transparent(bool transparent);
    void set_clickthrough(bool clickthrough);

private:
    d3d9_window() = default;
    ~d3d9_window() = default;
    
    bool create_window(const std::string& title, int width, int height);
    bool create_d3d9_device();
    void cleanup_d3d9();
    
    static LRESULT WINAPI window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    
    HWND hwnd_ = nullptr;
    IDirect3D9* d3d9_ = nullptr;
    IDirect3DDevice9* device_ = nullptr;
    D3DPRESENT_PARAMETERS present_params_ = {};
    
    bool running_ = false;
    bool device_lost_ = false;
    
    std::string window_class_name_ = "VisionD3D9Window";
};

} // namespace ui
} // namespace vision
