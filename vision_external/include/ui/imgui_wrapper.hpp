#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <vector>

namespace vision {
namespace ui {

// Forward declarations
class c_gui;
class c_widget;
class c_draw;

// Animation container for smooth transitions
template<typename T>
T* anim_container(T** state, ImGuiID id) {
    static std::unordered_map<ImGuiID, std::unique_ptr<T>> animation_states;
    
    auto it = animation_states.find(id);
    if (it == animation_states.end()) {
        animation_states[id] = std::make_unique<T>();
        *state = animation_states[id].get();
        memset(*state, 0, sizeof(T));
    } else {
        *state = it->second.get();
    }
    
    return *state;
}

// Animation states for different UI elements
struct button_state { 
    float hover_t = 0.0f; 
    float press_t = 0.0f; 
    float glow_t = 0.0f;
};

struct toggle_state { 
    float hover_t = 0.0f; 
    float active_t = 0.0f; 
    float check_t = 0.0f;
};

struct slider_state { 
    float hover_t = 0.0f; 
    float drag_t = 0.0f; 
    float handle_t = 0.0f;
};

struct tab_state { 
    float hover_t = 0.0f; 
    float active_t = 0.0f; 
    float slide_t = 0.0f;
};

struct input_state { 
    float hover_t = 0.0f; 
    float focus_t = 0.0f; 
    float cursor_t = 0.0f;
};

struct combo_state { 
    float hover_t = 0.0f; 
    float open_t = 0.0f; 
    float arrow_t = 0.0f;
};

// Color and style management
struct color_scheme {
    ImVec4 accent_color = ImVec4(0.11f, 0.64f, 0.92f, 1.0f);
    ImVec4 accent_hover = ImVec4(0.15f, 0.68f, 0.96f, 1.0f);
    ImVec4 accent_active = ImVec4(0.08f, 0.50f, 0.72f, 1.0f);
    
    ImVec4 background = ImVec4(0.13f, 0.14f, 0.15f, 1.0f);
    ImVec4 child_bg = ImVec4(0.16f, 0.17f, 0.18f, 1.0f);
    ImVec4 popup_bg = ImVec4(0.12f, 0.13f, 0.14f, 0.95f);
    
    ImVec4 text_active = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
    ImVec4 text_inactive = ImVec4(0.70f, 0.70f, 0.70f, 1.0f);
    ImVec4 text_hovered = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    ImVec4 text_disabled = ImVec4(0.50f, 0.50f, 0.50f, 1.0f);
    
    ImVec4 element_bg = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    ImVec4 element_bg_hover = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    ImVec4 element_bg_active = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    
    ImVec4 element_stroke = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    ImVec4 element_stroke_hover = ImVec4(0.53f, 0.53f, 0.60f, 0.70f);
    ImVec4 element_stroke_active = ImVec4(0.63f, 0.63f, 0.70f, 0.90f);
    
    ImVec4 separator = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    ImVec4 white_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 success_color = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
    ImVec4 warning_color = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
    ImVec4 error_color = ImVec4(0.9f, 0.2f, 0.2f, 1.0f);
};

struct style_settings {
    float rounding = 6.0f;
    float border_size = 1.0f;
    float tab_rounding = 4.0f;
    float button_rounding = 4.0f;
    float input_rounding = 3.0f;
    
    ImVec2 padding = ImVec2(8.0f, 6.0f);
    ImVec2 item_spacing = ImVec2(8.0f, 6.0f);
    ImVec2 button_padding = ImVec2(12.0f, 8.0f);
    ImVec2 tab_padding = ImVec2(16.0f, 10.0f);
    
    float animation_speed = 8.0f;
    float fast_animation_speed = 12.0f;
    float slow_animation_speed = 4.0f;
};

// Animation easing functions
namespace easing {
    inline float ease_out_cubic(float t) { return 1.0f - powf(1.0f - t, 3.0f); }
    inline float ease_in_out_cubic(float t) { return t < 0.5f ? 4.0f * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f; }
    inline float ease_out_back(float t) { const float c1 = 1.70158f; const float c3 = c1 + 1.0f; return 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f); }
    inline float bounce_out(float t) { 
        const float n1 = 7.5625f; const float d1 = 2.75f;
        if (t < 1.0f / d1) return n1 * t * t;
        else if (t < 2.0f / d1) return n1 * (t -= 1.5f / d1) * t + 0.75f;
        else if (t < 2.5f / d1) return n1 * (t -= 2.25f / d1) * t + 0.9375f;
        else return n1 * (t -= 2.625f / d1) * t + 0.984375f;
    }
}

// Fade direction enum
enum class fade_direction {
    horizontally,
    vertically,
    diagonally,
    diagonally_reversed
};

// Tab alignment
enum class tab_alignment {
    left,
    center,
    right
};

// Main GUI wrapper class
class c_gui {
public:
    static c_gui& instance() {
        static c_gui instance;
        return instance;
    }

    // Font management
    void push_font(ImFont* font);
    void pop_font();

    // Window management
    void set_next_window_pos(const ImVec2& pos, ImGuiCond cond = 0, const ImVec2& pivot = ImVec2(0, 0));
    void set_next_window_size(const ImVec2& size, ImGuiCond cond = 0);
    bool begin(const char* name, bool* p_open = nullptr, ImGuiWindowFlags flags = 0);
    void end();

    // Cursor management
    ImVec2 get_cursor_pos();
    float get_cursor_pos_x();
    float get_cursor_pos_y();
    void set_cursor_pos(const ImVec2& local_pos);
    void set_cursor_pos_x(float x);
    void set_cursor_pos_y(float y);

    // Frame management
    void new_frame();
    void end_frame();

    // Color utilities
    ImU32 get_color(const ImVec4& col, float alpha = 1.0f);
    
    // Style management
    void push_style_color(ImGuiCol idx, ImU32 col);
    void push_style_color(ImGuiCol idx, const ImVec4& col);
    void pop_style_color(int count = 1);
    void push_style_var(ImGuiStyleVar idx, float val);
    void push_style_var(ImGuiStyleVar idx, const ImVec2& val);
    void pop_style_var(int count = 1);

    // Layout
    void spacing();
    void sameline(float offset_from_start_x = 0.0f, float spacing_w = -1.0f);
    void begin_group();
    void end_group();

    // Animation helpers
    float fixed_speed(float speed);
    template<typename T>
    T* anim_container(T** state, ImGuiID id) {
        return vision::ui::anim_container(state, id);
    }

    // Rotation helpers
    void rotate_start();
    void rotate_end(float rad, ImVec2 center = ImVec2(0, 0));
    float deg_to_rad(float deg);

    // Utility
    std::string get_current_date();

    // Color scheme and settings
    color_scheme colors;
    style_settings settings;

private:
    int rotation_start_index_ = 0;
    ImVec2 rotate_center();
};

// Widget wrapper class
class c_widget {
public:
    static c_widget& instance() {
        static c_widget instance;
        return instance;
    }

    // === ANIMATED BUTTONS ===
    bool animated_button(std::string_view label, const ImVec2& size = ImVec2(0, 0));
    bool animated_button_colored(std::string_view label, const ImVec4& color, const ImVec2& size = ImVec2(0, 0));
    bool icon_button(std::string_view icon, std::string_view tooltip = "", const ImVec2& size = ImVec2(0, 0));
    bool gradient_button(std::string_view label, const ImVec4& color1, const ImVec4& color2, const ImVec2& size = ImVec2(0, 0));

    // === ANIMATED TOGGLES & CHECKBOXES ===
    bool animated_checkbox(std::string_view label, bool* v);
    bool animated_toggle(std::string_view label, bool* v, const ImVec2& size = ImVec2(50, 25));
    bool radio_button(std::string_view label, int* v, int button_value);

    // === ANIMATED SLIDERS ===
    bool animated_slider_float(std::string_view label, float* v, float v_min, float v_max, const char* format = "%.2f");
    bool animated_slider_int(std::string_view label, int* v, int v_min, int v_max, const char* format = "%d");
    bool animated_slider_angle(std::string_view label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f);

    // === ANIMATED INPUTS ===
    bool animated_input_text(std::string_view label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0);
    bool animated_input_text_hint(std::string_view label, std::string_view hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0);
    bool animated_input_int(std::string_view label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
    bool animated_input_float(std::string_view label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);

    // === ANIMATED COMBOS & LISTS ===
    bool animated_combo(std::string_view label, int* current_item, const char* const items[], int items_count);
    bool animated_combo_string(std::string_view label, int* current_item, const std::string items[], int items_count);
    bool animated_listbox(std::string_view label, int* current_item, const char* const items[], int items_count, int height_in_items = -1);

    // === ANIMATED TAB SYSTEM ===
    bool begin_animated_tab_bar(std::string_view str_id, ImGuiTabBarFlags flags = 0);
    bool animated_tab_item(std::string_view label, bool* p_open = nullptr, ImGuiTabItemFlags flags = 0);
    void end_animated_tab_bar();
    
    // Custom tab system with more control
    bool begin_custom_tabs(std::string_view str_id, const std::vector<std::string>& tab_names, int* selected_tab, tab_alignment alignment = tab_alignment::left);
    void end_custom_tabs();

    // === ANIMATED PROGRESS & STATUS ===
    void animated_progress_bar(float fraction, const ImVec2& size = ImVec2(-1, 0), const char* overlay = nullptr);
    void animated_loading_spinner(float radius = 16.0f, float thickness = 3.0f);
    void animated_status_dot(bool active, const ImVec4& active_color, const ImVec4& inactive_color, float radius = 4.0f);

    // === TEXT & STYLING ===
    void text_colored(ImFont* font, const ImU32 col, const std::string& text);
    void text_centered(const std::string& text, const ImVec4& color = ImVec4(1, 1, 1, 1));
    void text_with_background(const std::string& text, const ImVec4& bg_color, const ImVec4& text_color = ImVec4(1, 1, 1, 1));
    void separator();
    void animated_separator(float thickness = 1.0f);
    void gradient_separator(const ImVec4& color1, const ImVec4& color2, float thickness = 1.0f);

    // === POPUPS & TOOLTIPS ===
    bool begin_animated_popup(std::string_view name, float size_w, const ImVec2& position = ImVec2(0, 0), bool open_on_left_click = false);
    void end_animated_popup();
    bool animated_tooltip(std::string_view tooltip_text);
    void help_marker(std::string_view desc);

    // === LAYOUT HELPERS ===
    void begin_animated_group();
    void end_animated_group();
    void animated_spacing(float height = 0.0f);
    void same_line_animated(float offset_from_start_x = 0.0f, float spacing = -1.0f);

    // === COLOR PICKERS ===
    bool animated_color_edit3(std::string_view label, float col[3], ImGuiColorEditFlags flags = 0);
    bool animated_color_edit4(std::string_view label, float col[4], ImGuiColorEditFlags flags = 0);
    bool animated_color_picker3(std::string_view label, float col[3], ImGuiColorEditFlags flags = 0);
    bool animated_color_picker4(std::string_view label, float col[4], ImGuiColorEditFlags flags = 0);

    // === UTILITY FUNCTIONS ===
    ImVec2 calc_text_size(const std::string& text, ImFont* font = nullptr);
    ImVec4 lerp_color(const ImVec4& a, const ImVec4& b, float t);
    float animate_float(float current, float target, float speed);
    
    // Helper for getting current animation time
    float get_animation_time() { return ImGui::GetTime(); }

private:
    c_gui* gui = &c_gui::instance();
    c_draw* draw = nullptr; // Will be initialized
    
    // Internal helper functions
    void render_button_background(const ImRect& bb, button_state* state, const ImVec4& base_color, bool pressed);
    void render_glow_effect(const ImRect& bb, float intensity, const ImVec4& color);
    ImVec4 get_animated_color(const ImVec4& base, const ImVec4& target, float t);
};

// Drawing wrapper class
class c_draw {
public:
    static c_draw& instance() {
        static c_draw instance;
        return instance;
    }

    // Basic shapes
    void add_rect_filled(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0);
    void add_rect(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0, float thickness = 1.0f);
    void add_circle_filled(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments = 0);
    void add_circle(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments = 0, float thickness = 1.0f);
    void add_line(ImDrawList* draw_list, const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 1.0f);

    // Advanced shapes
    void rect_filled_multi_color(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, 
                                 ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left, 
                                 float rounding = 0.0f, ImDrawFlags flags = 0);
    void fade_rect_filled(ImDrawList* draw_list, const ImVec2& pos_min, const ImVec2& pos_max, 
                         ImU32 col_one, ImU32 col_two, fade_direction direction, 
                         float rounding = 0.0f, ImDrawFlags flags = 0);
    void radial_gradient(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out);

    // Text rendering
    void render_text(ImDrawList* draw_list, ImFont* font, const ImVec2& pos_min, const ImVec2& pos_max, 
                    ImU32 color, const char* text, const char* text_display_end = nullptr, 
                    const ImVec2* text_size_if_known = nullptr, const ImVec2& align = ImVec2(0, 0), 
                    const ImRect* clip_rect = nullptr);
    void add_text(ImDrawList* draw_list, const ImFont* font, float font_size, const ImVec2& pos, 
                 ImU32 col, const char* text_begin, const char* text_end = nullptr, 
                 float wrap_width = 0.0f, const ImVec4* cpu_fine_clip_rect = nullptr);

    // Image rendering
    void add_image(ImDrawList* draw_list, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, 
                  const ImVec2& uv_min = ImVec2(0, 0), const ImVec2& uv_max = ImVec2(1, 1), ImU32 col = IM_COL32_WHITE);
    void add_image_rounded(ImDrawList* draw_list, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, 
                          const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float rounding, ImDrawFlags flags = 0);

    // Clipping
    void push_clip_rect(ImDrawList* draw_list, const ImVec2& cr_min, const ImVec2& cr_max, bool intersect_with_current_clip_rect = false);
    void pop_clip_rect(ImDrawList* draw_list);

private:
    c_gui* gui = &c_gui::instance();
    void set_linear_color(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, 
                         ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1);
    void set_linear_color_alpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, 
                               ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1);
};

// Global instances
extern c_gui* gui;
extern c_widget* widget;
extern c_draw* draw;

// Utility macros
#define SCALE(x) (x)
#define SCALE_VEC2(x, y) ImVec2(x, y)

} // namespace ui
} // namespace vision
