#include <vision/config.hpp>
#include <vision/logger.hpp>
#include <vision/result.hpp>
#include <vision/utils/memory/process.hpp>
#include <vision/utils/xorstr.hpp>
#include <vision/metin2_sdk/sdk.hpp>

#ifdef IMGUI_AVAILABLE
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <tchar.h>
#endif

#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace vision;
using namespace vision::utils::memory;

// Global variables
static bool g_running = true;
static std::unique_ptr<c_process> g_target_process;

#ifdef IMGUI_AVAILABLE
// DirectX 11 globals
static ID3D11Device* g_pd3d_device = nullptr;
static ID3D11DeviceContext* g_pd3d_device_context = nullptr;
static IDXGISwapChain* g_p_swap_chain = nullptr;
static ID3D11RenderTargetView* g_p_main_render_target_view = nullptr;

// Forward declarations for UI
bool create_device_d3d(HWND hWnd);
void cleanup_device_d3d();
void create_render_target();
void cleanup_render_target();
LRESULT WINAPI wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void render_ui();
#endif

// Forward declarations
result<void> initialize_console();
result<void> initialize_vision();
result<void> attach_to_process();
result<void> inject_dll();
void main_loop();
void shutdown_vision();

int main() {
    // Initialize console for debugging
    auto console_result = initialize_console();
    if (!console_result) {
        ::MessageBoxA(nullptr, console_result.message().c_str(), "Console Error", MB_ICONERROR);
        return -1;
    }
    
    // Initialize vision system
    auto init_result = initialize_vision();
    if (!init_result) {
        std::cerr << "Failed to initialize vision: " << init_result.message() << std::endl;
        return -1;
    }
    
    logger::log_info("Vision external started");
    logger::log_info("Version: {}", config::version);
    logger::log_info("Build date: {}", config::build_date);
    
    // Enable debug privileges
    auto priv_result = c_process::enable_debug_privileges();
    if (!priv_result) {
        logger::log_warning("Failed to enable debug privileges: {}", priv_result.message());
    } else {
        logger::log_info("Debug privileges enabled");
    }
    
    // Attach to target process
    auto attach_result = attach_to_process();
    if (!attach_result) {
        logger::log_error("Failed to attach to process: {}", attach_result.message());
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }
    
    // Inject DLL
    auto inject_result = inject_dll();
    if (!inject_result) {
        logger::log_error("Failed to inject DLL: {}", inject_result.message());
    } else {
        logger::log_info("DLL injection successful");
    }
    
    // Start main loop
    main_loop();
    
    // Cleanup
    shutdown_vision();
    return 0;
}

result<void> initialize_console() {
    if (!::AllocConsole()) {
        return result<void>::error("Failed to allocate console");
    }
    
    // Redirect stdout, stdin, stderr to console
    FILE* pCout;
    FILE* pCin;
    FILE* pCerr;
    
    freopen_s(&pCout, "CONOUT$", "w", stdout);
    freopen_s(&pCin, "CONIN$", "r", stdin);
    freopen_s(&pCerr, "CONOUT$", "w", stderr);
    
    // Set console title
    ::SetConsoleTitleA("Vision External - Debug Console");
    
    return result<void>::ok();
}

result<void> initialize_vision() {
    try {
        // Initialize logger
        logger::initialize(xorstr("vision_external"), logger::log_level::debug, config::enable_file_logging);
        
        return result<void>::ok();
    } catch (const std::exception& e) {
        return result<void>::error("Initialization failed: " + std::string(e.what()));
    }
}

result<void> attach_to_process() {
    logger::log_info("Searching for target process: {}", config::target_process_name);
    
    // Try to find the process
    auto process_result = c_process::get(config::target_process_name);
    if (!process_result) {
        return result<void>::error("Process not found: " + process_result.message());
    }
    
    g_target_process = std::make_unique<c_process>(std::move(process_result.value()));
    
    logger::log_info("Attached to process PID: {}", g_target_process->id());
    
    // Example: Find a pattern in the target process
    auto pattern_result = g_target_process->find_pattern(metin2_sdk::signatures::get_player_pos);
    if (pattern_result) {
        logger::log_info("Found get_player_pos at: 0x{:X}", pattern_result->raw());
    } else {
        logger::log_warning("get_player_pos pattern not found: {}", pattern_result.message());
    }
    
    return result<void>::ok();
}

result<void> inject_dll() {
    if (!g_target_process || !g_target_process->is_valid()) {
        return result<void>::error("No valid target process");
    }
    
    logger::log_info("Injecting DLL: {}", config::internal_dll_name);
    
    // Get full path to DLL (assuming it's in the same directory)
    char dll_path[MAX_PATH];
    ::GetModuleFileNameA(nullptr, dll_path, MAX_PATH);
    
    // Replace exe name with dll name
    std::string dll_full_path = dll_path;
    const auto last_slash = dll_full_path.find_last_of("\\/");
    if (last_slash != std::string::npos) {
        dll_full_path = dll_full_path.substr(0, last_slash + 1) + std::string(config::internal_dll_name);
    }
    
    // Check if DLL exists
    if (::GetFileAttributesA(dll_full_path.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return result<void>::error("DLL not found: " + dll_full_path);
    }
    
    // Allocate memory in target process for DLL path
    const auto path_size = dll_full_path.length() + 1;
    const auto remote_memory = ::VirtualAllocEx(g_target_process->handle(), nullptr, path_size, 
                                               MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remote_memory) {
        return result<void>::error("Failed to allocate memory in target process");
    }
    
    // Write DLL path to target process
    SIZE_T bytes_written = 0;
    if (!::WriteProcessMemory(g_target_process->handle(), remote_memory, dll_full_path.c_str(), 
                             path_size, &bytes_written)) {
        ::VirtualFreeEx(g_target_process->handle(), remote_memory, 0, MEM_RELEASE);
        return result<void>::error("Failed to write DLL path to target process");
    }
    
    // Get LoadLibraryA address
    const auto kernel32 = ::GetModuleHandleA("kernel32.dll");
    if (!kernel32) {
        ::VirtualFreeEx(g_target_process->handle(), remote_memory, 0, MEM_RELEASE);
        return result<void>::error("Failed to get kernel32.dll handle");
    }
    
    const auto load_library_addr = ::GetProcAddress(kernel32, "LoadLibraryA");
    if (!load_library_addr) {
        ::VirtualFreeEx(g_target_process->handle(), remote_memory, 0, MEM_RELEASE);
        return result<void>::error("Failed to get LoadLibraryA address");
    }
    
    // Create remote thread
    const auto thread_handle = ::CreateRemoteThread(g_target_process->handle(), nullptr, 0,
                                                   reinterpret_cast<LPTHREAD_START_ROUTINE>(load_library_addr),
                                                   remote_memory, 0, nullptr);
    if (!thread_handle) {
        ::VirtualFreeEx(g_target_process->handle(), remote_memory, 0, MEM_RELEASE);
        return result<void>::error("Failed to create remote thread");
    }
    
    // Wait for injection to complete
    ::WaitForSingleObject(thread_handle, INFINITE);
    
    // Cleanup
    ::CloseHandle(thread_handle);
    ::VirtualFreeEx(g_target_process->handle(), remote_memory, 0, MEM_RELEASE);
    
    return result<void>::ok();
}

void main_loop() {
#ifdef IMGUI_AVAILABLE
    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, wnd_proc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Vision External", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Vision External", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);
    
    // Initialize Direct3D
    if (!create_device_d3d(hwnd)) {
        cleanup_device_d3d();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        logger::log_error("Failed to create D3D device");
        return;
    }
    
    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3d_device, g_pd3d_device_context);
    
    // Main loop
    bool done = false;
    while (!done && g_running) {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;
        
        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        // Render UI
        render_ui();
        
        // Rendering
        ImGui::Render();
        const float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
        g_pd3d_device_context->OMSetRenderTargets(1, &g_p_main_render_target_view, nullptr);
        g_pd3d_device_context->ClearRenderTargetView(g_p_main_render_target_view, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        
        g_p_swap_chain->Present(1, 0); // Present with vsync
    }
    
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    cleanup_device_d3d();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
#else
    // Simple console-based main loop
    logger::log_info("ImGui not available, running in console mode");
    logger::log_info("Press 'q' and Enter to quit");
    
    std::string input;
    while (g_running) {
        std::getline(std::cin, input);
        
        if (input == "q" || input == "quit") {
            g_running = false;
        } else if (input == "status") {
            if (g_target_process && g_target_process->is_valid()) {
                logger::log_info("Process status: Connected (PID: {})", g_target_process->id());
            } else {
                logger::log_info("Process status: Disconnected");
            }
        } else if (!input.empty()) {
            logger::log_info("Unknown command: {}", input);
            logger::log_info("Available commands: status, quit");
        }
    }
#endif
}

void shutdown_vision() {
    g_running = false;
    logger::log_info("Vision external shutting down");
    logger::shutdown();
    
    // Free console
    ::FreeConsole();
}

#ifdef IMGUI_AVAILABLE
// Helper functions for D3D11
bool create_device_d3d(HWND hWnd) {
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    
    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_p_swap_chain, &g_pd3d_device, &featureLevel, &g_pd3d_device_context);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_p_swap_chain, &g_pd3d_device, &featureLevel, &g_pd3d_device_context);
    if (res != S_OK)
        return false;
    
    create_render_target();
    return true;
}

void cleanup_device_d3d() {
    cleanup_render_target();
    if (g_p_swap_chain) { g_p_swap_chain->Release(); g_p_swap_chain = nullptr; }
    if (g_pd3d_device_context) { g_pd3d_device_context->Release(); g_pd3d_device_context = nullptr; }
    if (g_pd3d_device) { g_pd3d_device->Release(); g_pd3d_device = nullptr; }
}

void create_render_target() {
    ID3D11Texture2D* pBackBuffer;
    g_p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3d_device->CreateRenderTargetView(pBackBuffer, nullptr, &g_p_main_render_target_view);
    pBackBuffer->Release();
}

void cleanup_render_target() {
    if (g_p_main_render_target_view) { g_p_main_render_target_view->Release(); g_p_main_render_target_view = nullptr; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    
    switch (msg) {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        if (g_pd3d_device != nullptr) {
            cleanup_render_target();
            g_p_swap_chain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            create_render_target();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void render_ui() {
    // Main window
    ImGui::Begin("Vision External");
    
    ImGui::Text("Vision Game Helper v%s", std::string(config::version).c_str());
    ImGui::Text("Build: %s", std::string(config::build_date).c_str());
    ImGui::Separator();
    
    // Process status
    if (g_target_process && g_target_process->is_valid()) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Process: Connected (PID: %d)", g_target_process->id());
    } else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Process: Disconnected");
    }
    
    ImGui::Separator();
    
    // Controls
    if (ImGui::Button("Reconnect to Process")) {
        auto result = attach_to_process();
        if (!result) {
            logger::log_error("Reconnection failed: {}", result.message());
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Inject DLL")) {
        auto result = inject_dll();
        if (!result) {
            logger::log_error("Injection failed: {}", result.message());
        }
    }
    
    ImGui::Separator();
    
    if (ImGui::Button("Exit")) {
        g_running = false;
    }
    
    ImGui::End();
    
    // Demo window (if enabled)
    if (config::features::enable_imgui_demo) {
        ImGui::ShowDemoWindow();
    }
}
#endif
