#pragma once

#include <ui/imgui_wrapper.hpp>
#include <string>
#include <vector>

namespace vision {
namespace ui {

class imgui_demo {
public:
    static imgui_demo& instance() {
        static imgui_demo instance;
        return instance;
    }

    void render();
    void set_visible(bool visible) { show_demo_ = visible; }
    bool is_visible() const { return show_demo_; }
    void toggle_visibility() { show_demo_ = !show_demo_; }

private:
    bool show_demo_ = true;
    
    // Demo state variables
    bool demo_checkbox = false;
    bool demo_toggle = false;
    float demo_slider_float = 0.5f;
    int demo_slider_int = 50;
    float demo_angle = 0.0f;
    
    char demo_text_buffer[256] = "Hello World!";
    int demo_input_int = 42;
    float demo_input_float = 3.14159f;
    
    int demo_combo_current = 0;
    int demo_listbox_current = 0;
    
    int demo_tab_selected = 0;
    int demo_custom_tab_selected = 0;
    
    float demo_progress = 0.0f;
    bool demo_status_active = false;
    
    float demo_color3[3] = {1.0f, 0.0f, 0.5f};
    float demo_color4[4] = {0.2f, 0.8f, 0.2f, 1.0f};
    
    // Demo data
    const char* demo_combo_items[4] = {"Option 1", "Option 2", "Option 3", "Option 4"};
    const char* demo_listbox_items[6] = {"Item A", "Item B", "Item C", "Item D", "Item E", "Item F"};
    std::vector<std::string> demo_tab_names = {"General", "Advanced", "Settings", "About"};
    
    // Demo sections
    void render_button_demo();
    void render_toggle_demo();
    void render_slider_demo();
    void render_input_demo();
    void render_combo_demo();
    void render_tab_demo();
    void render_progress_demo();
    void render_text_demo();
    void render_color_demo();
    void render_layout_demo();
};

} // namespace ui
} // namespace vision
