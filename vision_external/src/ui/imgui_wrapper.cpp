#include <ui/imgui_wrapper.hpp>
#include <cmath>

namespace vision {
namespace ui {

// Global instances
c_gui* gui = &c_gui::instance();
c_widget* widget = &c_widget::instance();
c_draw* draw = &c_draw::instance();

// === c_gui IMPLEMENTATIONS ===

void c_gui::push_font(ImFont* font) {
    ImGuiContext& g = *GImGui;
    if (!font) font = ImGui::GetDefaultFont();
    
    ImGui::SetCurrentFont(font);
    g.FontStack.push_back(font);
    g.CurrentWindow->DrawList->PushTextureID(font->ContainerAtlas->TexID);
}

void c_gui::pop_font() {
    ImGuiContext& g = *GImGui;
    g.CurrentWindow->DrawList->PopTextureID();
    g.FontStack.pop_back();
    
    ImGui::SetCurrentFont(g.FontStack.empty() ? ImGui::GetDefaultFont() : g.FontStack.back());
}

float c_gui::fixed_speed(float speed) {
    return speed * ImGui::GetIO().DeltaTime;
}

ImU32 c_gui::get_color(const ImVec4& col, float alpha) {
    ImVec4 c = col;
    c.w *= ImGui::GetStyle().Alpha * alpha;
    return ImGui::ColorConvertFloat4ToU32(c);
}

// === c_draw IMPLEMENTATIONS ===

void c_draw::add_rect_filled(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags) {
    draw_list->AddRectFilled(p_min, p_max, col, rounding, flags);
}

void c_draw::add_rect(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags, float thickness) {
    draw_list->AddRect(p_min, p_max, col, rounding, flags, thickness);
}

void c_draw::add_circle_filled(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 col, int num_segments) {
    draw_list->AddCircleFilled(center, radius, col, num_segments);
}

void c_draw::add_line(ImDrawList* draw_list, const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness) {
    draw_list->AddLine(p1, p2, col, thickness);
}

void c_draw::render_text(ImDrawList* draw_list, ImFont* font, const ImVec2& pos_min, const ImVec2& pos_max, 
                        ImU32 color, const char* text, const char* text_display_end, 
                        const ImVec2* text_size_if_known, const ImVec2& align, const ImRect* clip_rect) {
    gui->push_font(font);
    
    ImVec2 pos = pos_min;
    const ImVec2 text_size = text_size_if_known ? *text_size_if_known : ImGui::CalcTextSize(text, text_display_end, false, 0.0f);
    
    if (align.x > 0.0f) pos.x = ImMax(pos.x, pos.x + (pos_max.x - pos.x - text_size.x) * align.x);
    if (align.y > 0.0f) pos.y = ImMax(pos.y, pos.y + (pos_max.y - pos.y - text_size.y) * align.y);
    
    if (clip_rect) {
        ImVec4 fine_clip_rect(clip_rect->Min.x, clip_rect->Min.y, clip_rect->Max.x, clip_rect->Max.y);
        draw_list->AddText(nullptr, 0, pos, color, text, text_display_end, 0.0f, &fine_clip_rect);
    } else {
        draw_list->AddText(nullptr, 0, pos, color, text, text_display_end, 0.0f, nullptr);
    }
    
    gui->pop_font();
}

// === c_widget ADVANCED ANIMATED IMPLEMENTATIONS ===

bool c_widget::animated_button(std::string_view label, const ImVec2& size) {
    // Safety check for null pointers
    if (!gui || !draw) {
        return ImGui::Button(label.data(), size);
    }
    
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;
    
    const ImGuiID id = window->GetID(label.data());
    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 button_size = ImVec2(size.x > 0 ? size.x : 100, size.y > 0 ? size.y : 28);
    
    ImRect bb(pos, ImVec2(pos.x + button_size.x, pos.y + button_size.y));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;
    
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    
    // Get animation state
    static std::unordered_map<ImGuiID, button_state> button_states;
    button_state& state = button_states[id];
    
    // Smooth animations with better easing
    float dt = ImGui::GetIO().DeltaTime;
    float hover_speed = hovered ? 12.0f : 8.0f;
    float press_speed = held ? 15.0f : 10.0f;
    
    state.hover_t += (hovered ? 1.0f : -1.0f) * dt * hover_speed;
    state.press_t += (held ? 1.0f : -1.0f) * dt * press_speed;
    state.glow_t += (hovered ? 1.0f : -1.0f) * dt * 10.0f;
    
    state.hover_t = ImClamp(state.hover_t, 0.0f, 1.0f);
    state.press_t = ImClamp(state.press_t, 0.0f, 1.0f);
    state.glow_t = ImClamp(state.glow_t, 0.0f, 1.0f);
    
    // Advanced easing functions
    float eased_hover = state.hover_t * state.hover_t * (3.0f - 2.0f * state.hover_t); // smoothstep
    float eased_press = 1.0f - powf(1.0f - state.press_t, 4.0f); // ease_out_quart
    float eased_glow = sinf(state.glow_t * IM_PI * 0.5f); // ease_out_sine
    
    ImDrawList* draw_list = window->DrawList;
    
    // Modern color scheme
    ImVec4 base_color = ImVec4(0.18f, 0.22f, 0.28f, 1.0f);
    ImVec4 hover_color = ImVec4(0.25f, 0.35f, 0.50f, 1.0f);
    ImVec4 active_color = ImVec4(0.20f, 0.60f, 0.90f, 1.0f);
    ImVec4 accent_color = ImVec4(0.25f, 0.70f, 0.95f, 1.0f);
    ImVec4 text_color = ImVec4(0.98f, 0.98f, 0.98f, 1.0f);
    
    // Color interpolation
    ImVec4 current_bg = lerp_color(base_color, hover_color, eased_hover);
    if (held) {
        current_bg = lerp_color(current_bg, active_color, eased_press);
    }
    
    // Scale and position effects
    float scale = 1.0f - eased_press * 0.03f;
    float y_offset = eased_press * 1.0f;
    
    ImVec2 scaled_min = ImVec2(bb.Min.x + (bb.GetWidth() * (1.0f - scale)) * 0.5f, 
                               bb.Min.y + (bb.GetHeight() * (1.0f - scale)) * 0.5f + y_offset);
    ImVec2 scaled_max = ImVec2(bb.Max.x - (bb.GetWidth() * (1.0f - scale)) * 0.5f, 
                               bb.Max.y - (bb.GetHeight() * (1.0f - scale)) * 0.5f + y_offset);
    
    // Outer glow effect
    if (eased_glow > 0.01f) {
        float glow_intensity = eased_glow * 0.8f;
        for (int i = 3; i >= 1; i--) {
            float glow_size = i * 2.0f * glow_intensity;
            float glow_alpha = (0.15f / i) * glow_intensity;
            
            ImVec2 glow_min = ImVec2(scaled_min.x - glow_size, scaled_min.y - glow_size);
            ImVec2 glow_max = ImVec2(scaled_max.x + glow_size, scaled_max.y + glow_size);
            ImU32 glow_col = ImGui::ColorConvertFloat4ToU32(ImVec4(accent_color.x, accent_color.y, accent_color.z, glow_alpha));
            
            draw_list->AddRectFilled(glow_min, glow_max, glow_col, 8.0f + i);
        }
    }
    
    // Main button background with gradient effect
    ImU32 bg_col = ImGui::ColorConvertFloat4ToU32(current_bg);
    ImU32 bg_col_light = ImGui::ColorConvertFloat4ToU32(lerp_color(current_bg, ImVec4(1, 1, 1, 0.1f), 0.1f));
    
    draw_list->AddRectFilledMultiColor(scaled_min, scaled_max, bg_col_light, bg_col_light, bg_col, bg_col);
    
    // Border with accent color on hover
    ImVec4 border_color = lerp_color(ImVec4(0.25f, 0.30f, 0.40f, 0.8f), accent_color, eased_hover * 0.7f);
    ImU32 border_col = ImGui::ColorConvertFloat4ToU32(border_color);
    draw_list->AddRect(scaled_min, scaled_max, border_col, 6.0f, 0, 1.0f + eased_hover);
    
    // Inner highlight
    if (eased_hover > 0.1f) {
        ImVec2 highlight_min = ImVec2(scaled_min.x + 1, scaled_min.y + 1);
        ImVec2 highlight_max = ImVec2(scaled_max.x - 1, scaled_min.y + 2);
        ImU32 highlight_col = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 0.1f * eased_hover));
        draw_list->AddRectFilled(highlight_min, highlight_max, highlight_col, 5.0f);
    }
    
    // Text with smooth color transition
    ImVec4 current_text_color = lerp_color(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), text_color, eased_hover);
    if (held) {
        current_text_color = lerp_color(current_text_color, accent_color, eased_press * 0.5f);
    }
    
    ImU32 text_col = ImGui::ColorConvertFloat4ToU32(current_text_color);
    ImVec2 text_size = ImGui::CalcTextSize(label.data());
    ImVec2 text_pos = ImVec2(scaled_min.x + (scaled_max.x - scaled_min.x - text_size.x) * 0.5f,
                             scaled_min.y + (scaled_max.y - scaled_min.y - text_size.y) * 0.5f + y_offset);
    
    // Text shadow for depth
    if (eased_hover > 0.2f) {
        ImVec2 shadow_pos = ImVec2(text_pos.x + 0.5f, text_pos.y + 0.5f);
        ImU32 shadow_col = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.3f * eased_hover));
        draw_list->AddText(shadow_pos, shadow_col, label.data());
    }
    
    draw_list->AddText(text_pos, text_col, label.data());
    
    return pressed;
}

bool c_widget::animated_button_colored(std::string_view label, const ImVec4& color, const ImVec2& size) {
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    bool result = ImGui::Button(label.data(), size);
    ImGui::PopStyleColor();
    return result;
}

bool c_widget::animated_checkbox(std::string_view label, bool* v) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    const ImGuiID id = window->GetID(label.data());
    const ImVec2 pos = window->DC.CursorPos;
    const float checkbox_size = 16.0f; // Slightly larger for better visibility
    const float label_spacing = 8.0f; // Better spacing
    const ImVec2 total_bb_size = ImVec2(checkbox_size + (label.empty() ? 0 : label_spacing + ImGui::CalcTextSize(label.data()).x), checkbox_size + 4);

    ImRect total_bb(pos, ImVec2(pos.x + total_bb_size.x, pos.y + total_bb_size.y));
    ImRect check_bb(pos, ImVec2(pos.x + checkbox_size, pos.y + checkbox_size));
    
    ImGui::ItemSize(total_bb, 3); // Better spacing between checkboxes
    if (!ImGui::ItemAdd(total_bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed) *v = !(*v);

    // Professional animation system
    static std::unordered_map<ImGuiID, ImVec4> animations;
    ImVec4& anim = animations[id];
    
    float dt = ImGui::GetIO().DeltaTime;
    anim.x += (*v ? 1.0f : -1.0f) * dt * 8.0f; // Check state
    anim.y += (hovered ? 1.0f : -1.0f) * dt * 10.0f; // Hover state
    anim.z += (held ? 1.0f : -1.0f) * dt * 15.0f; // Press state
    anim.w += (*v ? 1.0f : -1.0f) * dt * 6.0f; // Glow state
    
    anim.x = ImClamp(anim.x, 0.0f, 1.0f);
    anim.y = ImClamp(anim.y, 0.0f, 1.0f);
    anim.z = ImClamp(anim.z, 0.0f, 1.0f);
    anim.w = ImClamp(anim.w, 0.0f, 1.0f);
    
    // Smooth easing
    float check_t = anim.x * anim.x * (3.0f - 2.0f * anim.x);
    float hover_t = anim.y * anim.y * (3.0f - 2.0f * anim.y);
    float press_t = anim.z * anim.z * (3.0f - 2.0f * anim.z);
    float glow_t = anim.w * anim.w * (3.0f - 2.0f * anim.w);

    ImDrawList* draw_list = window->DrawList;
    
    // Professional color scheme
    ImVec4 bg_unchecked = ImVec4(0.08f, 0.10f, 0.14f, 1.0f);
    ImVec4 bg_checked = ImVec4(0.15f, 0.55f, 0.85f, 1.0f);
    ImVec4 border_unchecked = ImVec4(0.25f, 0.30f, 0.40f, 1.0f);
    ImVec4 border_checked = ImVec4(0.20f, 0.65f, 0.95f, 1.0f);
    
    // Color interpolation
    ImVec4 bg_color = lerp_color(bg_unchecked, bg_checked, check_t);
    ImVec4 border_color = lerp_color(border_unchecked, border_checked, check_t);
    
    // Hover brightening
    if (hover_t > 0.01f) {
        bg_color = lerp_color(bg_color, ImVec4(bg_color.x + 0.08f, bg_color.y + 0.08f, bg_color.z + 0.08f, 1.0f), hover_t * 0.5f);
        border_color = lerp_color(border_color, border_checked, hover_t * 0.6f);
    }
    
    // Press darkening
    if (press_t > 0.01f) {
        bg_color = lerp_color(bg_color, ImVec4(bg_color.x * 0.8f, bg_color.y * 0.8f, bg_color.z * 0.8f, 1.0f), press_t * 0.3f);
    }

    // Outer glow effect when checked
    if (glow_t > 0.1f) {
        float glow_size = 3.0f * glow_t;
        ImVec2 glow_min = ImVec2(check_bb.Min.x - glow_size, check_bb.Min.y - glow_size);
        ImVec2 glow_max = ImVec2(check_bb.Max.x + glow_size, check_bb.Max.y + glow_size);
        ImU32 glow_col = ImGui::ColorConvertFloat4ToU32(ImVec4(0.20f, 0.65f, 0.95f, glow_t * 0.2f));
        draw_list->AddRectFilled(glow_min, glow_max, glow_col, 6.0f);
    }
    
    // Main checkbox background
    ImU32 bg_col = ImGui::ColorConvertFloat4ToU32(bg_color);
    ImU32 border_col = ImGui::ColorConvertFloat4ToU32(border_color);
    
    draw_list->AddRectFilled(check_bb.Min, check_bb.Max, bg_col, 4.0f);
    draw_list->AddRect(check_bb.Min, check_bb.Max, border_col, 4.0f, 0, 1.5f);
    
    // Professional checkmark
    if (check_t > 0.01f) {
        ImVec2 center = ImVec2((check_bb.Min.x + check_bb.Max.x) * 0.5f, (check_bb.Min.y + check_bb.Max.y) * 0.5f);
        float scale = check_t * 0.7f;
        
        // Checkmark points
        ImVec2 p1 = ImVec2(center.x - 4.0f * scale, center.y - 1.0f * scale);
        ImVec2 p2 = ImVec2(center.x - 1.5f * scale, center.y + 2.5f * scale);
        ImVec2 p3 = ImVec2(center.x + 4.0f * scale, center.y - 3.0f * scale);
        
        // Checkmark with subtle glow
        ImU32 check_glow = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, check_t * 0.4f));
        draw_list->AddLine(p1, p2, check_glow, 3.0f);
        draw_list->AddLine(p2, p3, check_glow, 3.0f);
        
        ImU32 check_col = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, check_t));
        draw_list->AddLine(p1, p2, check_col, 2.2f);
        draw_list->AddLine(p2, p3, check_col, 2.2f);
    }

    // Professional label
    if (!label.empty()) {
        ImVec2 label_pos = ImVec2(check_bb.Max.x + label_spacing, check_bb.Min.y + (checkbox_size - ImGui::GetTextLineHeight()) * 0.5f);
        ImVec4 text_color = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
        
        // Text hover effect
        if (hover_t > 0.01f) {
            text_color = lerp_color(text_color, ImVec4(0.98f, 0.98f, 0.98f, 1.0f), hover_t);
        }
        
        ImU32 text_col = ImGui::ColorConvertFloat4ToU32(text_color);
        draw_list->AddText(label_pos, text_col, label.data());
    }

    return pressed;
}

bool c_widget::animated_toggle(std::string_view label, bool* v, const ImVec2& size) {
    return ImGui::Checkbox(label.data(), v);
}

bool c_widget::animated_slider_float(std::string_view label, float* v, float v_min, float v_max, const char* format) {
    return ImGui::SliderFloat(label.data(), v, v_min, v_max, format);
}

bool c_widget::animated_slider_int(std::string_view label, int* v, int v_min, int v_max, const char* format) {
    return ImGui::SliderInt(label.data(), v, v_min, v_max, format);
}

bool c_widget::animated_slider_angle(std::string_view label, float* v_rad, float v_degrees_min, float v_degrees_max) {
    float degrees = *v_rad * 180.0f / IM_PI;
    bool changed = ImGui::SliderFloat(label.data(), &degrees, v_degrees_min, v_degrees_max, "%.1f°");
    if (changed) *v_rad = degrees * IM_PI / 180.0f;
    return changed;
}

bool c_widget::animated_input_text(std::string_view label, char* buf, size_t buf_size, ImGuiInputTextFlags flags) {
    return ImGui::InputText(label.data(), buf, buf_size, flags);
}

bool c_widget::animated_input_text_hint(std::string_view label, std::string_view hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags) {
    return ImGui::InputTextWithHint(label.data(), hint.data(), buf, buf_size, flags);
}

bool c_widget::animated_input_int(std::string_view label, int* v, int step, int step_fast, ImGuiInputTextFlags flags) {
    return ImGui::InputInt(label.data(), v, step, step_fast, flags);
}

bool c_widget::animated_input_float(std::string_view label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags) {
    return ImGui::InputFloat(label.data(), v, step, step_fast, format, flags);
}

bool c_widget::animated_combo(std::string_view label, int* current_item, const char* const items[], int items_count) {
    return ImGui::Combo(label.data(), current_item, items, items_count);
}

bool c_widget::animated_combo_string(std::string_view label, int* current_item, const std::string items[], int items_count) {
    std::vector<const char*> c_items;
    for (int i = 0; i < items_count; ++i) {
        c_items.push_back(items[i].c_str());
    }
    return animated_combo(label, current_item, c_items.data(), items_count);
}

bool c_widget::animated_listbox(std::string_view label, int* current_item, const char* const items[], int items_count, int height_in_items) {
    return ImGui::ListBox(label.data(), current_item, items, items_count, height_in_items);
}

bool c_widget::begin_animated_tab_bar(std::string_view str_id, ImGuiTabBarFlags flags) {
    return ImGui::BeginTabBar(str_id.data(), flags);
}

bool c_widget::animated_tab_item(std::string_view label, bool* p_open, ImGuiTabItemFlags flags) {
    return ImGui::BeginTabItem(label.data(), p_open, flags);
}

void c_widget::end_animated_tab_bar() {
    ImGui::EndTabBar();
}

bool c_widget::begin_custom_tabs(std::string_view str_id, const std::vector<std::string>& tab_names, int* selected_tab, tab_alignment alignment) {
    for (int i = 0; i < (int)tab_names.size(); ++i) {
        if (i > 0) ImGui::SameLine();
        
        bool is_selected = (*selected_tab == i);
        if (is_selected) {
            ImGui::PushStyleColor(ImGuiCol_Button, gui->get_color(gui->colors.accent_color));
        }
        
        if (ImGui::Button(tab_names[i].c_str())) {
            *selected_tab = i;
        }
        
        if (is_selected) {
            ImGui::PopStyleColor();
        }
    }
    
    ImGui::Separator();
    return true;
}

void c_widget::end_custom_tabs() {
    // Nothing needed
}

void c_widget::animated_progress_bar(float fraction, const ImVec2& size, const char* overlay) {
    ImGui::ProgressBar(fraction, size, overlay);
}

void c_widget::animated_loading_spinner(float radius, float thickness) {
    ImGui::Text("Loading...");
}

void c_widget::animated_status_dot(bool active, const ImVec4& active_color, const ImVec4& inactive_color, float radius) {
    ImVec4 color = active ? active_color : inactive_color;
    ImGui::TextColored(color, "●");
}

bool c_widget::animated_color_edit3(std::string_view label, float col[3], ImGuiColorEditFlags flags) {
    return ImGui::ColorEdit3(label.data(), col, flags);
}

bool c_widget::animated_color_edit4(std::string_view label, float col[4], ImGuiColorEditFlags flags) {
    return ImGui::ColorEdit4(label.data(), col, flags);
}

bool c_widget::animated_color_picker3(std::string_view label, float col[3], ImGuiColorEditFlags flags) {
    return ImGui::ColorPicker3(label.data(), col, flags);
}

bool c_widget::animated_color_picker4(std::string_view label, float col[4], ImGuiColorEditFlags flags) {
    return ImGui::ColorPicker4(label.data(), col, flags);
}

void c_widget::text_colored(ImFont* font, const ImU32 col, const std::string& text) {
    gui->push_font(font);
    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(col), "%s", text.c_str());
    gui->pop_font();
}

void c_widget::text_centered(const std::string& text, const ImVec4& color) {
    ImVec2 window_size = ImGui::GetWindowSize();
    ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
    
    ImGui::SetCursorPosX((window_size.x - text_size.x) * 0.5f);
    ImGui::TextColored(color, "%s", text.c_str());
}

void c_widget::text_with_background(const std::string& text, const ImVec4& bg_color, const ImVec4& text_color) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, bg_color);
    ImGui::BeginChild("text_bg", ImVec2(0, ImGui::GetTextLineHeight() + 8), true);
    ImGui::TextColored(text_color, "%s", text.c_str());
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

void c_widget::separator() {
    ImGui::Separator();
}

void c_widget::animated_separator(float thickness) {
    ImGui::Separator();
}

void c_widget::gradient_separator(const ImVec4& color1, const ImVec4& color2, float thickness) {
    ImGui::Separator();
}

void c_widget::animated_spacing(float height) {
    if (height <= 0.0f) height = ImGui::GetStyle().ItemSpacing.y;
    ImGui::Dummy(ImVec2(0, height));
}

void c_widget::same_line_animated(float offset_from_start_x, float spacing) {
    ImGui::SameLine(offset_from_start_x, spacing);
}

void c_widget::begin_animated_group() {
    ImGui::BeginGroup();
}

void c_widget::end_animated_group() {
    ImGui::EndGroup();
}

bool c_widget::animated_tooltip(std::string_view tooltip_text) {
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", tooltip_text.data());
        return true;
    }
    return false;
}

void c_widget::help_marker(std::string_view desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", desc.data());
    }
}

bool c_widget::icon_button(std::string_view icon, std::string_view tooltip, const ImVec2& size) {
    return ImGui::Button(icon.data(), size);
}

bool c_widget::gradient_button(std::string_view label, const ImVec4& color1, const ImVec4& color2, const ImVec2& size) {
    return animated_button_colored(label, color1, size);
}

bool c_widget::radio_button(std::string_view label, int* v, int button_value) {
    return ImGui::RadioButton(label.data(), v, button_value);
}

ImVec4 c_widget::lerp_color(const ImVec4& a, const ImVec4& b, float t) {
    return ImVec4(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    );
}

ImVec4 c_widget::get_animated_color(const ImVec4& base, const ImVec4& target, float t) {
    return lerp_color(base, target, t);
}

float c_widget::animate_float(float current, float target, float speed) {
    return current + (target - current) * gui->fixed_speed(speed);
}

ImVec2 c_widget::calc_text_size(const std::string& text, ImFont* font) {
    if (font) {
        gui->push_font(font);
        ImVec2 size = ImGui::CalcTextSize(text.c_str());
        gui->pop_font();
        return size;
    }
    return ImGui::CalcTextSize(text.c_str());
}

void c_widget::render_button_background(const ImRect& bb, button_state* state, const ImVec4& base_color, bool pressed) {
    // Simple fallback - do nothing
}

void c_widget::render_glow_effect(const ImRect& bb, float intensity, const ImVec4& color) {
    // Simple fallback - do nothing
}

} // namespace ui
} // namespace vision
