#include <ui/imgui_manager.hpp>
#include <ui/imgui_demo.hpp>
#include <memory/c_memory.hpp>
#include <unordered_map>

namespace vision {
namespace ui {

bool imgui_manager::initialize(HWND hwnd, IDirect3DDevice9* device) {
    if (initialized_) {
        LOG_WARNING("ImGui manager already initialized");
        return true;
    }
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    
    // Setup fonts before initializing backends
    setup_fonts();
    
    // Setup Platform/Renderer backends
    if (!ImGui_ImplWin32_Init(hwnd)) {
        LOG_ERROR("Failed to initialize ImGui Win32 implementation");
        return false;
    }
    
    if (!ImGui_ImplDX9_Init(device)) {
        LOG_ERROR("Failed to initialize ImGui DX9 implementation");
        ImGui_ImplWin32_Shutdown();
        return false;
    }
    
    // Apply modern theme
    apply_modern_dark_theme();
    
    initialized_ = true;
    LOG_INFO("ImGui manager initialized successfully");
    return true;
}

void imgui_manager::shutdown() {
    if (!initialized_) return;
    
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    initialized_ = false;
    LOG_INFO("ImGui manager shutdown");
}

void imgui_manager::begin_frame() {
    if (!initialized_) return;
    
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void imgui_manager::end_frame() {
    if (!initialized_) return;
    
    ImGui::EndFrame();
}

void imgui_manager::render() {
    if (!initialized_) return;
    
    render_main_menu();
    
    // Demo window disabled to prevent errors
    // imgui_demo::instance().render();
    
    // Actually render ImGui
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void imgui_manager::setup_fonts() {
    ImGuiIO& io = ImGui::GetIO();
    
    // Load Verdana as default font with compact size for our small UI
    font_default_ = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 14.0f);
    if (!font_default_) {
        // Fallback to default font if Verdana is not found
        font_default_ = io.Fonts->AddFontDefault();
        LOG_WARNING("Could not load Verdana font, using default font");
    } else {
        LOG_INFO("Loaded Verdana font successfully");
    }
    
    // Load Font Awesome from memory with matching size
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;
    config.GlyphMinAdvanceX = 14.0f; // Match default font size for consistency
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    
    font_icons_ = io.Fonts->AddFontFromMemoryTTF((void*)fa_regular_400, sizeof(fa_regular_400), 
                                                 16.0f, &config, icon_ranges);
    if (!font_icons_) {
        // Create a fallback font for icons if Font Awesome loading fails
        font_icons_ = font_default_;
        LOG_WARNING("Could not load Font Awesome, using default font for icons");
    } else {
        LOG_INFO("Loaded Font Awesome icons successfully");
    }
    
    // Bold font variant for headers (slightly larger for emphasis)
    font_bold_ = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdanab.ttf", 16.0f);
    if (!font_bold_) {
        font_bold_ = font_default_;
        LOG_WARNING("Could not load Verdana Bold font, using default font");
    } else {
        LOG_INFO("Loaded Verdana Bold font successfully");
    }
    
    // Build font atlas
    io.Fonts->Build();
    LOG_INFO("Font atlas built successfully");
}

void imgui_manager::apply_modern_dark_theme() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Modern dark theme with vibrant accents
    style.Colors[ImGuiCol_Text] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.08f, 0.95f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.10f, 0.90f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.12f, 0.98f);
    
    // Borders with subtle glow
    style.Colors[ImGuiCol_Border] = ImVec4(0.20f, 0.25f, 0.35f, 0.70f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
    
    // Frame backgrounds with gradient feel
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.25f, 0.32f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.32f, 0.42f, 1.00f);
    
    // Title bar
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.06f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.08f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.02f, 0.02f, 0.04f, 0.80f);
    
    // Menu bar
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.10f, 0.14f, 1.00f);
    
    // Scrollbar with modern look
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.04f, 0.04f, 0.06f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.30f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.42f, 0.55f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.55f, 0.70f, 1.00f);
    
    // Check mark with accent color
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.20f, 0.75f, 1.00f, 1.00f);
    
    // Slider with gradient
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.70f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.35f, 0.80f, 1.00f, 1.00f);
    
    // Modern buttons with better contrast
    style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.22f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.35f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.60f, 0.90f, 1.00f);
    
    // Header with modern styling
    style.Colors[ImGuiCol_Header] = ImVec4(0.15f, 0.20f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.30f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.40f, 0.60f, 1.00f);
    
    // Separator with accent
    style.Colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.30f, 0.40f, 0.80f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.35f, 0.50f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.20f, 0.70f, 1.00f, 1.00f);
    
    // Resize grip
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.25f, 0.35f, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.30f, 0.40f, 0.55f, 0.80f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.25f, 0.70f, 0.95f, 1.00f);
    
    // Tab with modern look
    style.Colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.15f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.30f, 0.45f, 1.00f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.25f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.10f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.20f, 0.28f, 1.00f);
    
    // Plot lines with vibrant colors
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.70f, 0.80f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.70f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.30f, 0.80f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.40f, 0.90f, 0.70f, 1.00f);
    
    // Table with better contrast
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.15f, 0.20f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.25f, 0.30f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.20f, 0.25f, 0.35f, 0.80f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.20f, 0.25f, 0.35f, 0.15f);
    
    // Text selection with accent
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.60f, 0.90f, 0.40f);
    
    // Drag drop
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.25f, 0.75f, 1.00f, 1.00f);
    
    // Nav with modern colors
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.25f, 0.70f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.80f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.60f);
    
    // Modal
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.70f);
    
    // Ultra-compact style settings
    style.WindowPadding = ImVec2(4.00f, 4.00f);
    style.FramePadding = ImVec2(6.00f, 3.00f);
    style.CellPadding = ImVec2(3.00f, 2.00f);
    style.ItemSpacing = ImVec2(3.00f, 2.00f);
    style.ItemInnerSpacing = ImVec2(3.00f, 2.00f);
    style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style.IndentSpacing = 20;
    style.ScrollbarSize = 12;
    style.GrabMinSize = 8;
    style.WindowBorderSize = 0;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 0;
    style.TabBorderSize = 0;
    style.WindowRounding = 8;
    style.ChildRounding = 6;
    style.FrameRounding = 6;
    style.PopupRounding = 6;
    style.ScrollbarRounding = 6;
    style.GrabRounding = 4;
    style.LogSliderDeadzone = 4;
    style.TabRounding = 6;
}

void imgui_manager::render_main_menu() {
    if (!show_menu_) return;
    
    ImGui::SetNextWindowSize(ImVec2(480, 270), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(80, 45), ImGuiCond_FirstUseEver);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    
    if (!ImGui::Begin("Vision - Modern Enhancement Tool", &show_menu_, 
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
        ImGui::PopStyleVar(2);
        ImGui::End();
        return;
    }
    
    render_modern_layout();
    
    ImGui::PopStyleVar(2);
    ImGui::End();
}

void imgui_manager::render_modern_layout() {
    ImVec2 window_size = ImGui::GetWindowSize();
    
    // Custom title bar (even smaller for ultra-compact UI)
    render_custom_titlebar();
    
    // Ultra-compact sidebar for smaller window
    ImGui::BeginChild("Sidebar", ImVec2(110, window_size.y - 25), true, ImGuiWindowFlags_NoScrollbar);
    render_sidebar();
    ImGui::EndChild();
    
    // Main content area (adjusted for ultra-compact sidebar)
    ImGui::SameLine();
    ImGui::BeginChild("MainContent", ImVec2(0, window_size.y - 25), true);
    render_main_content();
    ImGui::EndChild();
}

void imgui_manager::render_custom_titlebar() {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 window_size = ImGui::GetWindowSize();
    
    // Ultra-compact title bar background with modern gradient
    ImVec2 titlebar_min = window_pos;
    ImVec2 titlebar_max = ImVec2(window_pos.x + window_size.x, window_pos.y + 25);
    
    ImU32 gradient_start = gui->get_color(ImVec4(0.06f, 0.08f, 0.12f, 1.0f));
    ImU32 gradient_end = gui->get_color(ImVec4(0.10f, 0.15f, 0.22f, 1.0f));
    
    draw_list->AddRectFilledMultiColor(titlebar_min, titlebar_max, 
                                      gradient_start, gradient_start, gradient_end, gradient_end);
    
    // Subtle border at bottom
    draw_list->AddLine(ImVec2(titlebar_min.x, titlebar_max.y), titlebar_max, 
                      gui->get_color(ImVec4(0.25f, 0.35f, 0.50f, 0.8f)), 1.0f);
    
    // Ultra-compact title text with bold font
    ImGui::SetCursorPos(ImVec2(6, 4));
    ImGui::PushFont(font_bold_);
    
    // Subtle glow effect
    ImGui::SetCursorPos(ImVec2(6.5f, 4.5f));
    ImGui::TextColored(ImVec4(0.25f, 0.70f, 0.95f, 0.4f), "VISION");
    
    // Main title
    ImGui::SetCursorPos(ImVec2(6, 4));
    ImGui::TextColored(ImVec4(0.98f, 0.98f, 0.98f, 1.0f), "VISION");
    
    ImGui::PopFont();
    
    // Subtitle with regular font
    ImGui::SameLine();
    ImGui::PushFont(font_default_);
    ImGui::TextColored(ImVec4(0.70f, 0.75f, 0.80f, 1.0f), " | MT2");
    ImGui::PopFont();
    
    // Ultra-compact close button
    ImGui::SetCursorPos(ImVec2(window_size.x - 23, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
    if (widget->animated_button("×", ImVec2(20, 20))) {
        show_menu_ = false;
    }
    ImGui::PopStyleVar();
    
    ImGui::SetCursorPosY(25); // Move cursor below ultra-compact title bar
}

void imgui_manager::render_sidebar() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
    
    const char* tab_icons[] = {ICON_FA_SYRINGE, ICON_FA_SWORD, ICON_FA_PERSON_RUNNING, ICON_FA_USER, ICON_FA_WRENCH, ICON_FA_GEAR};
    const char* tab_names[] = {"Inject", "Combat", "Movement", "Player", "Other", "Config"};
    
    static float tab_animations[6] = {0};
    
    for (int i = 0; i < 6; i++) {
        // Animation for hover effect
        bool is_selected = (selected_main_tab_ == i);
        bool is_hovered = false;
        
        ImGui::PushID(i);
        
        // Custom button with full width
        ImVec2 button_size = ImVec2(ImGui::GetContentRegionAvail().x, 45);
        ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
        
        // Animation
        float dt = ImGui::GetIO().DeltaTime;
        tab_animations[i] += (is_selected ? 1.0f : -1.0f) * dt * 6.0f;
        tab_animations[i] = ImClamp(tab_animations[i], 0.0f, 1.0f);
        
        // Check if hovered
        if (ImGui::IsMouseHoveringRect(cursor_pos, ImVec2(cursor_pos.x + button_size.x, cursor_pos.y + button_size.y))) {
            is_hovered = true;
        }
        
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        // Background with smooth animation
        ImVec4 bg_color = is_selected ? gui->colors.accent_color : 
                         is_hovered ? gui->colors.element_bg_hover : gui->colors.element_bg;
        
        if (is_selected) {
            // Selected tab with gradient and glow
            ImU32 selected_color = gui->get_color(ImVec4(gui->colors.accent_color.x, gui->colors.accent_color.y, 
                                                        gui->colors.accent_color.z, 0.8f));
            draw_list->AddRectFilled(cursor_pos, ImVec2(cursor_pos.x + button_size.x, cursor_pos.y + button_size.y), 
                                   selected_color, 6.0f);
            
            // Left accent line
            draw_list->AddRectFilled(cursor_pos, ImVec2(cursor_pos.x + 4, cursor_pos.y + button_size.y), 
                                   gui->get_color(gui->colors.accent_hover), 0.0f);
        } else if (is_hovered) {
            draw_list->AddRectFilled(cursor_pos, ImVec2(cursor_pos.x + button_size.x, cursor_pos.y + button_size.y), 
                                   gui->get_color(bg_color), 6.0f);
        }
        
        // Button behavior
        if (ImGui::InvisibleButton("##tab", button_size)) {
            selected_main_tab_ = i;
        }
        
        // Properly aligned icon and text
        float icon_y_center = cursor_pos.y + (button_size.y * 0.5f) - 8.0f; // Center vertically
        float text_y_center = cursor_pos.y + (button_size.y * 0.5f) - (ImGui::GetTextLineHeight() * 0.5f);
        
        // Icon with Font Awesome and animation
        ImGui::SetCursorScreenPos(ImVec2(cursor_pos.x + 15, icon_y_center));
        ImGui::PushFont(font_icons_);
        float icon_scale = 1.0f + (is_selected ? 0.15f : 0.0f);
        ImGui::SetWindowFontScale(icon_scale);
        ImGui::TextColored(is_selected ? gui->colors.white_color : gui->colors.text_inactive, "%s", tab_icons[i]);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopFont();
        
        // Tab name with default font - properly aligned
        ImGui::SetCursorScreenPos(ImVec2(cursor_pos.x + 45, text_y_center));
        ImGui::PushFont(font_default_);
        ImGui::TextColored(is_selected ? gui->colors.white_color : gui->colors.text_inactive, "%s", tab_names[i]);
        ImGui::PopFont();
        
        ImGui::PopID();
        ImGui::SetCursorScreenPos(ImVec2(cursor_pos.x, cursor_pos.y + button_size.y + 2));
    }
    
    ImGui::PopStyleVar(2);
}

void imgui_manager::render_main_content() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 8));
    
    switch (selected_main_tab_) {
        case 0: render_inject_tab(); break;
        case 1: render_combat_tab(); break;
        case 2: render_movement_tab(); break;
        case 3: render_player_tab(); break;
        case 4: render_other_tab(); break;
        case 5: render_configs_tab(); break;
    }
    
    ImGui::PopStyleVar();
}

void imgui_manager::render_inject_tab() {
    static vision::memory::c_memory memory;
    static bool is_attached = false;
    static std::string status_text = "Not Connected";
    static DWORD process_id = 0;
    static uintptr_t base_address = 0;
    
    // Section header with bold font and spacing
    ImGui::Spacing();
    ImGui::PushFont(font_bold_);
    ImGui::TextColored(ImVec4(0.98f, 0.98f, 0.98f, 1.0f), "Process Injection & Management");
    ImGui::PopFont();
    ImGui::Separator();
    ImGui::Spacing();
    
    static char process_name[256] = "metin2client.exe";
    ImGui::InputText("Process Name", process_name, sizeof(process_name));
    ImGui::Spacing();
    
    if (widget->animated_button("Attach to Process", ImVec2(120, 24))) {
        LOG_INFO("Attempting to attach to process: {}", process_name);
        
        if (memory.attach_to_process(process_name)) {
            is_attached = true;
            process_id = memory.get_process_id();
            base_address = memory.get_module_base("");
            status_text = "Connected";
            LOG_INFO("Successfully attached to {} (PID: {})", process_name, process_id);
        } else {
            is_attached = false;
            status_text = "Connection Failed";
            process_id = 0;
            base_address = 0;
        }
    }
    
    ImGui::SameLine();
    if (widget->animated_button_colored("Inject DLL", gui->colors.success_color, ImVec2(100, 24))) {
        if (is_attached) {
            std::string dll_path = "build\\bin\\Release\\vision_internal.dll";
            LOG_INFO("Attempting DLL injection: {}", dll_path);
            
            if (memory.inject_dll(dll_path)) {
                LOG_INFO("DLL injection successful!");
            } else {
                LOG_ERROR("DLL injection failed!");
            }
        } else {
            LOG_WARNING("Not attached to any process");
        }
    }
    
    ImGui::SameLine();
    if (widget->animated_button_colored("Disconnect", gui->colors.error_color, ImVec2(80, 24))) {
        memory.close_process();
        is_attached = false;
        status_text = "Disconnected";
        process_id = 0;
        base_address = 0;
        LOG_INFO("Disconnected from process");
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::PushFont(font_bold_);
    ImGui::Text("Process Status:");
    ImGui::PopFont();
    ImGui::BulletText("Status: %s", status_text.c_str());
    ImGui::BulletText("Process ID: %s", process_id ? std::to_string(process_id).c_str() : "N/A");
    ImGui::BulletText("Base Address: %s", base_address ? fmt::format("0x{:X}", base_address).c_str() : "N/A");
    
    if (is_attached) {
        auto modules = memory.get_loaded_modules();
        ImGui::BulletText("Loaded Modules: %zu", modules.size());
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::PushFont(font_bold_);
    ImGui::Text("Auto-Injection Settings");
    ImGui::PopFont();
    
    render_feature_toggle("auto_inject", "Auto-inject on process start");
    render_feature_toggle("auto_reconnect", "Auto-reconnect on disconnect");
    render_feature_slider("inject_delay", "Injection delay (seconds)", 0.0f, 10.0f, "%.1f");
}

void imgui_manager::render_combat_tab() {
    // Combat subtabs
    if (ImGui::BeginTabBar("CombatSubTabs")) {
        if (ImGui::BeginTabItem("General")) {
            render_combat_general();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Skills")) {
            render_combat_skills();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Targeting")) {
            render_combat_targeting();
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
}

void imgui_manager::render_combat_general() {
    ImGui::Spacing();
    ImGui::PushFont(font_bold_);
    ImGui::Text("General Combat Settings");
    ImGui::PopFont();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Basic Combat Features
    ImGui::PushFont(font_default_);
    ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.9f, 1.0f), "Basic Features:");
    ImGui::PopFont();
    ImGui::Spacing();
    
    render_feature_toggle("auto_attack", "Auto Attack");
    render_feature_toggle("auto_pickup", "Auto Pickup Items");
    render_feature_toggle("auto_potion", "Auto Use Potions");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Damage Modifications
    ImGui::PushFont(font_default_);
    ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.9f, 1.0f), "Damage Modifications:");
    ImGui::PopFont();
    ImGui::Spacing();
    
    render_feature_toggle("damage_hack", "Damage Multiplier");
    render_feature_slider("damage_multiplier", "Damage Multiplier", 1.0f, 10.0f, "%.1fx");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Advanced Features
    ImGui::PushFont(font_default_);
    ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.9f, 1.0f), "Advanced Features:");
    ImGui::PopFont();
    ImGui::Spacing();
    
    render_feature_toggle("god_mode", "God Mode (Invincibility)");
    render_feature_toggle("infinite_mana", "Infinite Mana/SP");
    render_feature_toggle("no_cooldown", "No Skill Cooldowns");
    render_feature_toggle("critical_hit", "Always Critical Hit");
    
    ImGui::Spacing();
}

void imgui_manager::render_combat_skills() {
    ImGui::Text("Skill Management");
    ImGui::Separator();
    
    render_feature_toggle("skill_spam", "Skill Spam");
    render_feature_input_int("skill_id", "Skill ID to spam");
    render_feature_slider("skill_delay", "Skill delay (ms)", 0.0f, 5000.0f, "%.0f ms");
    
    ImGui::Spacing();
    render_feature_toggle("auto_buff", "Auto Buff");
    render_feature_toggle("combo_attack", "Combo Attack");
    render_feature_toggle("range_hack", "Unlimited Attack Range");
}

void imgui_manager::render_combat_targeting() {
    ImGui::Text("Targeting & AI");
    ImGui::Separator();
    
    render_feature_toggle("auto_target", "Auto Target Enemies");
    render_feature_toggle("target_nearest", "Always Target Nearest");
    render_feature_toggle("ignore_players", "Ignore Other Players");
    render_feature_slider("target_range", "Target Range", 1.0f, 50.0f, "%.1f");
    
    ImGui::Spacing();
    render_feature_toggle("smart_targeting", "Smart Targeting (Weak enemies first)");
    render_feature_toggle("boss_priority", "Prioritize Boss Monsters");
}

void imgui_manager::render_movement_tab() {
    if (ImGui::BeginTabBar("MovementSubTabs")) {
        
        if (ImGui::BeginTabItem("Speed")) {
            render_movement_speed();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Teleport")) {
            render_movement_teleport();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Pathfinding")) {
            render_movement_pathfinding();
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
}

void imgui_manager::render_movement_speed() {
    ImGui::Spacing();
    ImGui::PushFont(font_bold_);
    ImGui::Text("Movement Speed Modifications");
    ImGui::PopFont();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Speed Modifications
    ImGui::PushFont(font_default_);
    ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.9f, 1.0f), "Speed & Movement:");
    ImGui::PopFont();
    ImGui::Spacing();
    
    render_feature_toggle("speed_hack", "Speed Hack");
    render_feature_slider("movement_speed", "Movement Speed Multiplier", 1.0f, 10.0f, "%.1fx");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Advanced Movement
    ImGui::PushFont(font_default_);
    ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.9f, 1.0f), "Advanced Movement:");
    ImGui::PopFont();
    ImGui::Spacing();
    
    render_feature_toggle("fly_hack", "Fly Hack (No Clip)");
    render_feature_toggle("wall_hack", "Wall Hack (Walk through walls)");
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Jump Modifications
    ImGui::PushFont(font_default_);
    ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.9f, 1.0f), "Jump Modifications:");
    ImGui::PopFont();
    ImGui::Spacing();
    
    render_feature_toggle("jump_hack", "Super Jump");
    render_feature_slider("jump_height", "Jump Height Multiplier", 1.0f, 5.0f, "%.1fx");
    
    ImGui::Spacing();
}

void imgui_manager::render_movement_teleport() {
    ImGui::Spacing();
    ImGui::PushFont(font_bold_);
    ImGui::Text("Teleportation Features");
    ImGui::PopFont();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Basic Teleport
    ImGui::PushFont(font_default_);
    ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.9f, 1.0f), "Basic Teleportation:");
    ImGui::PopFont();
    ImGui::Spacing();
    
    render_feature_toggle("teleport_hack", "Teleport Hack");
    
    ImGui::Spacing();
    
    // Coordinate Teleport
    ImGui::PushFont(font_default_);
    ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.9f, 1.0f), "Coordinate Teleport:");
    ImGui::PopFont();
    ImGui::Spacing();
    
    static float teleport_coords[3] = {0.0f, 0.0f, 0.0f};
    ImGui::InputFloat3("Coordinates (X, Y, Z)", teleport_coords);
    ImGui::Spacing();
    
    if (widget->animated_button("Teleport to Coordinates", ImVec2(180, 24))) {
        LOG_INFO("Teleporting to: {}, {}, {}", teleport_coords[0], teleport_coords[1], teleport_coords[2]);
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Advanced Teleport
    ImGui::PushFont(font_default_);
    ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.9f, 1.0f), "Advanced Options:");
    ImGui::PopFont();
    ImGui::Spacing();
    
    render_feature_toggle("teleport_to_mob", "Teleport to Nearest Mob");
    render_feature_toggle("safe_teleport", "Safe Teleport (Avoid obstacles)");
    
    ImGui::Spacing();
}

void imgui_manager::render_movement_pathfinding() {
    ImGui::Text("Automated Movement");
    ImGui::Separator();
    
    render_feature_toggle("auto_walk", "Auto Walk");
    render_feature_toggle("follow_player", "Follow Player");
    render_feature_input_text("follow_player_name", "Player name to follow");
    
    ImGui::Spacing();
    render_feature_toggle("waypoint_system", "Waypoint System");
    render_feature_toggle("avoid_obstacles", "Avoid Obstacles");
    render_feature_toggle("return_to_spot", "Return to Original Spot");
}

void imgui_manager::render_player_tab() {
    if (ImGui::BeginTabBar("PlayerSubTabs")) {
        if (ImGui::BeginTabItem("Stats")) {
            render_player_stats();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Inventory")) {
            render_player_inventory();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Character")) {
            render_player_character();
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
}

void imgui_manager::render_player_stats() {
    ImGui::Text("Player Statistics Modification");
    ImGui::Separator();
    
    render_feature_toggle("stat_hack", "Enable Stat Modifications");
    render_feature_input_int("health_value", "Health Points");
    render_feature_input_int("mana_value", "Mana Points");
    render_feature_input_int("level_value", "Character Level");
    
    ImGui::Spacing();
    render_feature_input_int("strength_value", "Strength");
    render_feature_input_int("intelligence_value", "Intelligence");
    render_feature_input_int("dexterity_value", "Dexterity");
    render_feature_input_int("vitality_value", "Vitality");
}

void imgui_manager::render_player_inventory() {
    ImGui::Text("Inventory Management");
    ImGui::Separator();
    
    render_feature_toggle("item_hack", "Item Modifications");
    render_feature_toggle("infinite_items", "Infinite Item Usage");
    render_feature_toggle("auto_sell", "Auto Sell Junk Items");
    render_feature_toggle("item_esp", "Item ESP (Show items on ground)");
    
    ImGui::Spacing();
    render_feature_input_int("item_id", "Item ID to spawn");
    render_feature_input_int("item_count", "Item count");
    
    if (ImGui::Button("Spawn Item", ImVec2(120, 30))) {
        LOG_INFO("Spawning item");
    }
}

void imgui_manager::render_player_character() {
    ImGui::Text("Character Modifications");
    ImGui::Separator();
    
    render_feature_toggle("name_change", "Change Character Name");
    render_feature_input_text("new_name", "New character name");
    render_feature_toggle("gender_change", "Change Gender");
    render_feature_toggle("class_change", "Change Class");
    
    ImGui::Spacing();
    render_feature_toggle("appearance_hack", "Appearance Modifications");
    render_feature_toggle("size_hack", "Character Size Hack");
    render_feature_slider("character_size", "Character Size", 0.1f, 5.0f, "%.1fx");
}

void imgui_manager::render_other_tab() {
    if (ImGui::BeginTabBar("OtherSubTabs")) {
        if (ImGui::BeginTabItem("Visual")) {
            render_other_visual();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Misc")) {
            render_other_misc();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Debug")) {
            render_other_debug();
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
}

void imgui_manager::render_other_visual() {
    ImGui::Text("Visual Enhancements");
    ImGui::Separator();
    
    render_feature_toggle("esp_players", "Player ESP");
    render_feature_toggle("esp_mobs", "Monster ESP");
    render_feature_toggle("esp_items", "Item ESP");
    render_feature_toggle("esp_npcs", "NPC ESP");
    
    ImGui::Spacing();
    render_feature_toggle("fullbright", "Fullbright (No darkness)");
    render_feature_toggle("no_fog", "Remove Fog");
    render_feature_toggle("wireframe", "Wireframe Mode");
    render_feature_toggle("crosshair", "Custom Crosshair");
}

void imgui_manager::render_other_misc() {
    ImGui::Text("Miscellaneous Features");
    ImGui::Separator();
    
    render_feature_toggle("chat_spam", "Chat Spam");
    render_feature_input_text("spam_message", "Spam message");
    render_feature_slider("spam_delay", "Spam delay (seconds)", 0.1f, 10.0f, "%.1f");
    
    ImGui::Spacing();
    render_feature_toggle("auto_login", "Auto Login");
    render_feature_toggle("anti_afk", "Anti-AFK");
    render_feature_toggle("packet_logger", "Log Network Packets");
}

void imgui_manager::render_other_debug() {
    ImGui::Text("Debug & Development");
    ImGui::Separator();
    
    render_feature_toggle("console_output", "Show Console Output");
    render_feature_toggle("memory_scanner", "Memory Scanner");
    render_feature_toggle("function_hooker", "Function Hook Manager");
    
    ImGui::Spacing();
    if (ImGui::Button("Dump Memory", ImVec2(120, 30))) {
        LOG_INFO("Dumping memory");
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Scan Patterns", ImVec2(120, 30))) {
        LOG_INFO("Scanning patterns");
    }
}

void imgui_manager::render_configs_tab() {
    ImGui::Text("Configuration Management");
    ImGui::Separator();
    
    ImGui::Text("Save/Load Configurations:");
    
    static char config_name[256] = "default";
    ImGui::InputText("Config Name", config_name, sizeof(config_name));
    
    if (ImGui::Button("Save Config", ImVec2(100, 30))) {
        LOG_INFO("Saving configuration: {}", config_name);
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Load Config", ImVec2(100, 30))) {
        LOG_INFO("Loading configuration: {}", config_name);
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Delete Config", ImVec2(100, 30))) {
        LOG_INFO("Deleting configuration: {}", config_name);
    }
    
    ImGui::Spacing();
    ImGui::Text("Available Configurations:");
    
    // List available configs
    const char* configs[] = {"default", "combat_focused", "stealth_mode", "farming_setup"};
    static int selected_config = 0;
    
    for (int i = 0; i < 4; i++) {
        if (ImGui::Selectable(configs[i], selected_config == i)) {
            selected_config = i;
            strcpy_s(config_name, configs[i]);
        }
    }
}



feature_config& imgui_manager::get_feature(const std::string& name) {
    return features_[name];
}

void imgui_manager::set_feature(const std::string& name, const feature_config& config) {
    features_[name] = config;
}

void imgui_manager::render_feature_toggle(const std::string& name, const std::string& description) {
    auto& feature = get_feature(name);
    
    // Use our new compact animated checkbox
    bool enabled = feature.enabled;
    if (widget->animated_checkbox(description, &enabled)) {
        feature.enabled = enabled;
        LOG_DEBUG("Feature '{}' {}", name, feature.enabled ? "enabled" : "disabled");
    }
}

void imgui_manager::render_feature_slider(const std::string& name, const std::string& description, 
                                        float min_val, float max_val, const std::string& format) {
    auto& feature = get_feature(name);
    
    // Use a simple, properly constrained slider
    ImGui::PushFont(font_default_);
    ImGui::Text("%s", description.c_str());
    ImGui::PopFont();
    
    // Create a properly sized slider that respects window bounds
    ImGui::PushItemWidth(-80.0f); // Leave 80px for the value display
    bool changed = ImGui::SliderFloat(("##" + name).c_str(), &feature.value, min_val, max_val, format.c_str());
    ImGui::PopItemWidth();
    
    if (changed) {
        LOG_DEBUG("Feature '{}' value changed to: {}", name, feature.value);
    }
    
    ImGui::Spacing();
}

void imgui_manager::render_feature_input_int(const std::string& name, const std::string& description) {
    auto& feature = get_feature(name);
    if (ImGui::InputInt(description.c_str(), &feature.int_value)) {
        LOG_DEBUG("Feature '{}' int value changed to: {}", name, feature.int_value);
    }
}

void imgui_manager::render_feature_input_text(const std::string& name, const std::string& description) {
    auto& feature = get_feature(name);
    char buffer[256];
    strncpy_s(buffer, feature.string_value.c_str(), sizeof(buffer) - 1);
    
    if (ImGui::InputText(description.c_str(), buffer, sizeof(buffer))) {
        feature.string_value = buffer;
        LOG_DEBUG("Feature '{}' string value changed to: {}", name, feature.string_value);
    }
}

bool imgui_manager::render_animated_button(const std::string& text, const ImVec2& size) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;
    
    const ImGuiID id = window->GetID(text.c_str());
    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 button_size = ImVec2(size.x > 0 ? size.x : 120, size.y > 0 ? size.y : 30);
    
    ImRect bb(pos, ImVec2(pos.x + button_size.x, pos.y + button_size.y));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;
    
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    
    // Animation state
    struct button_state { float hover_t; float press_t; };
    button_state* state = gui->anim_container(&state, id);
    
    state->hover_t = ImClamp(state->hover_t + (gui->fixed_speed(12.0f) * (hovered ? 1.0f : -1.0f)), 0.0f, 1.0f);
    state->press_t = ImClamp(state->press_t + (gui->fixed_speed(15.0f) * (held ? 1.0f : -1.0f)), 0.0f, 1.0f);
    
    ImDrawList* draw_list = window->DrawList;
    
    // Colors with animation
    ImVec4 base_color = gui->colors.element_bg;
    ImVec4 hover_color = gui->colors.accent_color;
    
    // Interpolate colors
    ImVec4 bg_color = ImVec4(
        base_color.x + (hover_color.x - base_color.x) * state->hover_t * 0.3f,
        base_color.y + (hover_color.y - base_color.y) * state->hover_t * 0.3f,
        base_color.z + (hover_color.z - base_color.z) * state->hover_t * 0.3f,
        base_color.w
    );
    
    ImU32 bg_col = gui->get_color(bg_color);
    ImU32 border_col = gui->get_color(gui->colors.accent_color, 0.4f + 0.4f * state->hover_t);
    ImU32 text_col = gui->get_color(gui->colors.text_active, 0.8f + 0.2f * state->hover_t);
    
    // Scale effect when pressed
    float scale = 1.0f - 0.05f * state->press_t;
    ImVec2 scaled_min = ImVec2(bb.Min.x + (bb.GetWidth() * (1.0f - scale)) * 0.5f, 
                               bb.Min.y + (bb.GetHeight() * (1.0f - scale)) * 0.5f);
    ImVec2 scaled_max = ImVec2(bb.Max.x - (bb.GetWidth() * (1.0f - scale)) * 0.5f, 
                               bb.Max.y - (bb.GetHeight() * (1.0f - scale)) * 0.5f);
    
    // Background with glow effect
    if (state->hover_t > 0.1f) {
        // Outer glow
        ImVec2 glow_min = ImVec2(scaled_min.x - 2, scaled_min.y - 2);
        ImVec2 glow_max = ImVec2(scaled_max.x + 2, scaled_max.y + 2);
        ImU32 glow_col = gui->get_color(gui->colors.accent_color, 0.2f * state->hover_t);
        draw->add_rect_filled(draw_list, glow_min, glow_max, glow_col, gui->settings.rounding + 2);
    }
    
    // Main button
    draw->add_rect_filled(draw_list, scaled_min, scaled_max, bg_col, gui->settings.rounding);
    draw->add_rect(draw_list, scaled_min, scaled_max, border_col, gui->settings.rounding, 0, 1.5f);
    
    // Text with slight movement when pressed
    ImVec2 text_offset = ImVec2(0, state->press_t * 1.0f);
    ImVec2 text_min = ImVec2(scaled_min.x + text_offset.x, scaled_min.y + text_offset.y);
    ImVec2 text_max = ImVec2(scaled_max.x + text_offset.x, scaled_max.y + text_offset.y);
    draw->render_text(draw_list, nullptr, text_min, text_max, text_col, 
                     text.c_str(), nullptr, nullptr, ImVec2(0.5f, 0.5f));
    
    return pressed;
}

} // namespace ui
} // namespace vision
