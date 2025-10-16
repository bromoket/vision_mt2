#pragma once

#include <d3d9.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <ui/imgui_wrapper.hpp>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <vision/common/logger.hpp>

// Font Awesome icons
#include "../../fonts/IconsFontAwesome6Pro.h"
#include "../../fonts/fa-regular-400.h"

namespace vision {
namespace ui {

struct feature_config {
    bool enabled = false;
    float value = 0.0f;
    int int_value = 0;
    std::string string_value = "";
};

class imgui_manager {
public:
    static imgui_manager& instance() {
        static imgui_manager instance;
        return instance;
    }

    bool initialize(HWND hwnd, IDirect3DDevice9* device);
    void shutdown();
    
    void begin_frame();
    void end_frame();
    void render();
    
    bool is_visible() const { return show_menu_; }
    void toggle_visibility() { show_menu_ = !show_menu_; }
    void set_visibility(bool visible) { show_menu_ = visible; }
    
    // Style management
    void apply_modern_dark_theme();
    void apply_moonlight_theme();
    void apply_custom_colors();
    
    // Font management
    void setup_fonts();
    ImFont* get_default_font() const { return font_default_; }
    ImFont* get_icon_font() const { return font_icons_; }
    ImFont* get_bold_font() const { return font_bold_; }
    
    // Feature management
    feature_config& get_feature(const std::string& name);
    void set_feature(const std::string& name, const feature_config& config);

private:
    imgui_manager() = default;
    ~imgui_manager() = default;
    
    bool initialized_ = false;
    bool show_menu_ = true;
    
    // UI State
    int selected_main_tab_ = 0;
    int selected_combat_subtab_ = 0;
    int selected_movement_subtab_ = 0;
    int selected_player_subtab_ = 0;
    int selected_other_subtab_ = 0;
    
    // Feature storage
    std::unordered_map<std::string, feature_config> features_;
    
    // Font storage
    ImFont* font_default_ = nullptr;
    ImFont* font_icons_ = nullptr;
    ImFont* font_bold_ = nullptr;
    
    // Rendering functions
    void render_main_menu();
    void render_inject_tab();
    void render_combat_tab();
    void render_movement_tab();
    void render_player_tab();
    void render_other_tab();
    void render_configs_tab();
    
    // Combat subtabs
    void render_combat_general();
    void render_combat_skills();
    void render_combat_targeting();
    
    // Movement subtabs
    void render_movement_speed();
    void render_movement_teleport();
    void render_movement_pathfinding();
    
    // Player subtabs
    void render_player_stats();
    void render_player_inventory();
    void render_player_character();
    
    // Other subtabs
    void render_other_visual();
    void render_other_misc();
    void render_other_debug();
    void render_modern_layout();
    void render_sidebar();
    void render_main_content();
    void render_custom_titlebar();
    
    // Helper functions
    void render_feature_toggle(const std::string& name, const std::string& description);
    void render_feature_slider(const std::string& name, const std::string& description, 
                              float min_val, float max_val, const std::string& format = "%.1f");
    void render_feature_input_int(const std::string& name, const std::string& description);
    void render_feature_input_text(const std::string& name, const std::string& description);
    
    // Animated UI elements
    bool render_animated_button(const std::string& text, const ImVec2& size = ImVec2(0, 0));
    void render_animated_tab_bar();
    
    // Style helpers
    void push_style_colors();
    void pop_style_colors();
};

} // namespace ui
} // namespace vision

// Global ImGui message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
