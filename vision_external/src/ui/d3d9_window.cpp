#include <ui/d3d9_window.hpp>
#include <ui/imgui_manager.hpp>

namespace vision {
namespace ui {

bool d3d9_window::initialize(const std::string& window_title, int width, int height) {
    if (running_) {
        LOG_WARNING("D3D9 window already initialized");
        return true;
    }
    
    if (!create_window(window_title, width, height)) {
        LOG_ERROR("Failed to create window");
        return false;
    }
    
    if (!create_d3d9_device()) {
        LOG_ERROR("Failed to create D3D9 device");
        return false;
    }
    
    // Initialize ImGui
    if (!imgui_manager::instance().initialize(hwnd_, device_)) {
        LOG_ERROR("Failed to initialize ImGui");
        cleanup_d3d9();
        return false;
    }
    
    running_ = true;
    LOG_INFO("D3D9 window initialized successfully");
    return true;
}

void d3d9_window::shutdown() {
    if (!running_) return;
    
    imgui_manager::instance().shutdown();
    cleanup_d3d9();
    
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
    
    UnregisterClassA(window_class_name_.c_str(), GetModuleHandle(nullptr));
    
    running_ = false;
    LOG_INFO("D3D9 window shutdown");
}

void d3d9_window::process_messages() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        if (msg.message == WM_QUIT) {
            running_ = false;
        }
    }
}

void d3d9_window::present() {
    if (!device_ || device_lost_) return;
    
    // Clear the back buffer
    device_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
    
    if (device_->BeginScene() >= 0) {
        // Render ImGui
        imgui_manager::instance().begin_frame();
        imgui_manager::instance().render();
        imgui_manager::instance().end_frame();
        
        device_->EndScene();
    }
    
    // Present the frame
    HRESULT result = device_->Present(nullptr, nullptr, nullptr, nullptr);
    
    // Handle device lost
    if (result == D3DERR_DEVICELOST && device_->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
        imgui_manager::instance().shutdown();
        
        if (device_->Reset(&present_params_) == D3D_OK) {
            imgui_manager::instance().initialize(hwnd_, device_);
            device_lost_ = false;
        }
    }
}

bool d3d9_window::create_window(const std::string& title, int width, int height) {
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = window_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = window_class_name_.c_str();
    wc.hIconSm = nullptr;
    
    if (!RegisterClassExA(&wc)) {
        LOG_ERROR("Failed to register window class");
        return false;
    }
    
    // Calculate window size including borders
    RECT rect = {0, 0, width, height};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    
    hwnd_ = CreateWindowExA(
        WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
        window_class_name_.c_str(),
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (!hwnd_) {
        LOG_ERROR("Failed to create window");
        return false;
    }
    
    ShowWindow(hwnd_, SW_SHOWDEFAULT);
    UpdateWindow(hwnd_);
    
    return true;
}

bool d3d9_window::create_d3d9_device() {
    d3d9_ = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d9_) {
        LOG_ERROR("Failed to create Direct3D9 object - DirectX 9 may not be installed");
        return false;
    }
    
    // Check adapter capabilities
    D3DCAPS9 caps;
    HRESULT caps_hr = d3d9_->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    if (FAILED(caps_hr)) {
        LOG_ERROR("Failed to get device capabilities: 0x{:X}", caps_hr);
        d3d9_->Release();
        d3d9_ = nullptr;
        return false;
    }
    
    ZeroMemory(&present_params_, sizeof(present_params_));
    present_params_.Windowed = TRUE;
    present_params_.SwapEffect = D3DSWAPEFFECT_DISCARD;
    present_params_.BackBufferFormat = D3DFMT_UNKNOWN;
    present_params_.EnableAutoDepthStencil = TRUE;
    present_params_.AutoDepthStencilFormat = D3DFMT_D16;
    present_params_.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    
    // Try hardware vertex processing first
    DWORD behavior_flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    if (!(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)) {
        LOG_WARNING("Hardware T&L not supported, falling back to software vertex processing");
        behavior_flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
    
    HRESULT hr = d3d9_->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hwnd_,
        behavior_flags,
        &present_params_,
        &device_
    );
    
    if (FAILED(hr)) {
        LOG_ERROR("Failed to create D3D9 device with hardware processing: 0x{:X}", hr);
        
        // Try software vertex processing as fallback
        if (behavior_flags == D3DCREATE_HARDWARE_VERTEXPROCESSING) {
            LOG_INFO("Retrying with software vertex processing...");
            hr = d3d9_->CreateDevice(
                D3DADAPTER_DEFAULT,
                D3DDEVTYPE_HAL,
                hwnd_,
                D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                &present_params_,
                &device_
            );
        }
        
        if (FAILED(hr)) {
            LOG_ERROR("Failed to create D3D9 device with software processing: 0x{:X}", hr);
            
            // Try reference device as last resort
            LOG_INFO("Retrying with reference device...");
            hr = d3d9_->CreateDevice(
                D3DADAPTER_DEFAULT,
                D3DDEVTYPE_REF,
                hwnd_,
                D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                &present_params_,
                &device_
            );
            
            if (FAILED(hr)) {
                LOG_ERROR("Failed to create D3D9 reference device: 0x{:X}", hr);
                d3d9_->Release();
                d3d9_ = nullptr;
                return false;
            } else {
                LOG_WARNING("Using D3D9 reference device (software rendering)");
            }
        } else {
            LOG_INFO("Successfully created D3D9 device with software vertex processing");
        }
    } else {
        LOG_INFO("Successfully created D3D9 device with hardware vertex processing");
    }
    
    return true;
}

void d3d9_window::cleanup_d3d9() {
    if (device_) {
        device_->Release();
        device_ = nullptr;
    }
    
    if (d3d9_) {
        d3d9_->Release();
        d3d9_ = nullptr;
    }
}

void d3d9_window::set_topmost(bool topmost) {
    if (!hwnd_) return;
    
    SetWindowPos(hwnd_, topmost ? HWND_TOPMOST : HWND_NOTOPMOST,
                0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void d3d9_window::set_transparent(bool transparent) {
    if (!hwnd_) return;
    
    if (transparent) {
        SetWindowLongA(hwnd_, GWL_EXSTYLE, GetWindowLongA(hwnd_, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hwnd_, RGB(0, 0, 0), 200, LWA_ALPHA);
    } else {
        SetWindowLongA(hwnd_, GWL_EXSTYLE, GetWindowLongA(hwnd_, GWL_EXSTYLE) & ~WS_EX_LAYERED);
    }
}

void d3d9_window::set_clickthrough(bool clickthrough) {
    if (!hwnd_) return;
    
    if (clickthrough) {
        SetWindowLongA(hwnd_, GWL_EXSTYLE, GetWindowLongA(hwnd_, GWL_EXSTYLE) | WS_EX_TRANSPARENT);
    } else {
        SetWindowLongA(hwnd_, GWL_EXSTYLE, GetWindowLongA(hwnd_, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);
    }
}

LRESULT WINAPI d3d9_window::window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // Forward to ImGui first
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return true;
    }
    
    switch (msg) {
    case WM_SIZE:
        if (instance().device_ && wparam != SIZE_MINIMIZED) {
            instance().present_params_.BackBufferWidth = LOWORD(lparam);
            instance().present_params_.BackBufferHeight = HIWORD(lparam);
            
            imgui_manager::instance().shutdown();
            instance().device_->Reset(&instance().present_params_);
            imgui_manager::instance().initialize(hwnd, instance().device_);
        }
        return 0;
        
    case WM_SYSCOMMAND:
        if ((wparam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
        
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        
    case WM_KEYDOWN:
        if (wparam == VK_INSERT) {
            imgui_manager::instance().toggle_visibility();
        }
        break;
    }
    
    return DefWindowProcA(hwnd, msg, wparam, lparam);
}

} // namespace ui
} // namespace vision
