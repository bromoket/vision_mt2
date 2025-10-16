#include <ui/imgui_demo.hpp>
#include <imgui.h>

namespace vision {
namespace ui {

void imgui_demo::render() {
    if (!show_demo_) return;
    
    ImGui::SetNextWindowSize(ImVec2(900, 700), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    
    if (!ImGui::Begin("Vision UI Demo - Animated Components", &show_demo_, ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        return;
    }
    
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Demo")) {
            if (ImGui::MenuItem("Reset All Values")) {
                // Reset all demo values
                demo_checkbox = false;
                demo_toggle = false;
                demo_slider_float = 0.5f;
                demo_slider_int = 50;
                demo_angle = 0.0f;
                strcpy_s(demo_text_buffer, "Hello World!");
                demo_input_int = 42;
                demo_input_float = 3.14159f;
                demo_combo_current = 0;
                demo_listbox_current = 0;
                demo_tab_selected = 0;
                demo_custom_tab_selected = 0;
                demo_progress = 0.0f;
                demo_status_active = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    // Update progress animation
    demo_progress += ImGui::GetIO().DeltaTime * 0.3f;
    if (demo_progress > 1.0f) demo_progress = 0.0f;
    
    // Create scrollable area
    ImGui::BeginChild("DemoContent", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    
    // Header
    widget->text_centered("🎨 Vision UI Animated Components Demo", gui->colors.accent_color);
    widget->animated_separator(2.0f);
    widget->animated_spacing(10.0f);
    
    // Render all demo sections
    render_button_demo();
    render_toggle_demo();
    render_slider_demo();
    render_input_demo();
    render_combo_demo();
    render_tab_demo();
    render_progress_demo();
    render_text_demo();
    render_color_demo();
    render_layout_demo();
    
    ImGui::EndChild();
    ImGui::End();
}

void imgui_demo::render_button_demo() {
    widget->text_with_background("🔘 ANIMATED BUTTONS", gui->colors.accent_color, gui->colors.white_color);
    widget->animated_spacing(5.0f);
    
    // Regular animated buttons
    if (widget->animated_button("Standard Button", ImVec2(150, 35))) {
        // Button clicked
    }
    
    widget->same_line_animated();
    if (widget->animated_button_colored("Success Button", gui->colors.success_color, ImVec2(150, 35))) {
        // Success button clicked
    }
    
    widget->same_line_animated();
    if (widget->animated_button_colored("Warning Button", gui->colors.warning_color, ImVec2(150, 35))) {
        // Warning button clicked
    }
    
    widget->same_line_animated();
    if (widget->animated_button_colored("Error Button", gui->colors.error_color, ImVec2(150, 35))) {
        // Error button clicked
    }
    
    // Icon buttons
    widget->animated_spacing(10.0f);
    ImGui::Text("Icon Buttons:");
    
    if (widget->icon_button("⚙", "Settings", ImVec2(40, 40))) {
        // Settings clicked
    }
    
    widget->same_line_animated();
    if (widget->icon_button("🔍", "Search", ImVec2(40, 40))) {
        // Search clicked
    }
    
    widget->same_line_animated();
    if (widget->icon_button("💾", "Save", ImVec2(40, 40))) {
        // Save clicked
    }
    
    widget->same_line_animated();
    if (widget->icon_button("📁", "Open", ImVec2(40, 40))) {
        // Open clicked
    }
    
    // Gradient button
    widget->animated_spacing(10.0f);
    if (widget->gradient_button("Gradient Button", gui->colors.accent_color, gui->colors.accent_hover, ImVec2(200, 40))) {
        // Gradient button clicked
    }
    
    widget->animated_separator();
    widget->animated_spacing(15.0f);
}

void imgui_demo::render_toggle_demo() {
    widget->text_with_background("☑ ANIMATED TOGGLES & CHECKBOXES", gui->colors.accent_color, gui->colors.white_color);
    widget->animated_spacing(5.0f);
    
    // Animated checkbox
    widget->animated_checkbox("Animated Checkbox", &demo_checkbox);
    
    // Animated toggle switch
    widget->animated_spacing(10.0f);
    ImGui::Text("Toggle Switch:");
    widget->animated_toggle("Enable Feature", &demo_toggle, ImVec2(60, 30));
    
    // Radio buttons
    widget->animated_spacing(10.0f);
    ImGui::Text("Radio Buttons:");
    static int radio_selection = 0;
    widget->radio_button("Option A", &radio_selection, 0);
    widget->same_line_animated();
    widget->radio_button("Option B", &radio_selection, 1);
    widget->same_line_animated();
    widget->radio_button("Option C", &radio_selection, 2);
    
    widget->animated_separator();
    widget->animated_spacing(15.0f);
}

void imgui_demo::render_slider_demo() {
    widget->text_with_background("🎚 ANIMATED SLIDERS", gui->colors.accent_color, gui->colors.white_color);
    widget->animated_spacing(5.0f);
    
    // Float slider
    widget->animated_slider_float("Float Slider", &demo_slider_float, 0.0f, 1.0f, "%.3f");
    
    // Integer slider
    widget->animated_slider_int("Integer Slider", &demo_slider_int, 0, 100, "%d");
    
    // Angle slider
    widget->animated_slider_angle("Angle Slider", &demo_angle, -180.0f, 180.0f);
    
    widget->animated_separator();
    widget->animated_spacing(15.0f);
}

void imgui_demo::render_input_demo() {
    widget->text_with_background("📝 ANIMATED INPUTS", gui->colors.accent_color, gui->colors.white_color);
    widget->animated_spacing(5.0f);
    
    // Text input
    widget->animated_input_text("Text Input", demo_text_buffer, sizeof(demo_text_buffer));
    
    // Text input with hint
    static char hint_buffer[256] = "";
    widget->animated_input_text_hint("Input with Hint", "Enter your name...", hint_buffer, sizeof(hint_buffer));
    
    // Integer input
    widget->animated_input_int("Integer Input", &demo_input_int);
    
    // Float input
    widget->animated_input_float("Float Input", &demo_input_float, 0.01f, 1.0f, "%.5f");
    
    widget->animated_separator();
    widget->animated_spacing(15.0f);
}

void imgui_demo::render_combo_demo() {
    widget->text_with_background("📋 ANIMATED COMBOS & LISTS", gui->colors.accent_color, gui->colors.white_color);
    widget->animated_spacing(5.0f);
    
    // Animated combo
    widget->animated_combo("Combo Box", &demo_combo_current, demo_combo_items, 4);
    
    // Animated listbox
    widget->animated_spacing(10.0f);
    widget->animated_listbox("List Box", &demo_listbox_current, demo_listbox_items, 6, 4);
    
    widget->animated_separator();
    widget->animated_spacing(15.0f);
}

void imgui_demo::render_tab_demo() {
    widget->text_with_background("📑 ANIMATED TAB SYSTEMS", gui->colors.accent_color, gui->colors.white_color);
    widget->animated_spacing(5.0f);
    
    // Standard animated tab bar
    if (widget->begin_animated_tab_bar("DemoTabs")) {
        if (widget->animated_tab_item("Tab 1")) {
            ImGui::Text("Content of Tab 1");
            ImGui::Text("This is the first tab with some content.");
        }
        
        if (widget->animated_tab_item("Tab 2")) {
            ImGui::Text("Content of Tab 2");
            ImGui::Text("This is the second tab with different content.");
        }
        
        if (widget->animated_tab_item("Tab 3")) {
            ImGui::Text("Content of Tab 3");
            ImGui::Text("This is the third tab with more content.");
        }
        
        widget->end_animated_tab_bar();
    }
    
    widget->animated_spacing(20.0f);
    
    // Custom tab system
    ImGui::Text("Custom Tab System:");
    if (widget->begin_custom_tabs("CustomTabs", demo_tab_names, &demo_custom_tab_selected, tab_alignment::center)) {
        switch (demo_custom_tab_selected) {
        case 0:
            ImGui::Text("General Settings");
            ImGui::Text("Configure general application settings here.");
            break;
        case 1:
            ImGui::Text("Advanced Options");
            ImGui::Text("Advanced configuration options for power users.");
            break;
        case 2:
            ImGui::Text("Application Settings");
            ImGui::Text("Customize the application behavior.");
            break;
        case 3:
            ImGui::Text("About Information");
            ImGui::Text("Version information and credits.");
            break;
        }
        widget->end_custom_tabs();
    }
    
    widget->animated_separator();
    widget->animated_spacing(15.0f);
}

void imgui_demo::render_progress_demo() {
    widget->text_with_background("📊 ANIMATED PROGRESS & STATUS", gui->colors.accent_color, gui->colors.white_color);
    widget->animated_spacing(5.0f);
    
    // Animated progress bar
    widget->animated_progress_bar(demo_progress, ImVec2(-1, 25), "Loading...");
    
    widget->animated_spacing(10.0f);
    
    // Loading spinner
    ImGui::Text("Loading Spinner:");
    widget->same_line_animated();
    widget->animated_loading_spinner(20.0f, 4.0f);
    
    widget->animated_spacing(10.0f);
    
    // Status dots
    ImGui::Text("Status Indicators:");
    widget->same_line_animated();
    widget->animated_status_dot(true, gui->colors.success_color, gui->colors.text_inactive, 6.0f);
    widget->same_line_animated();
    ImGui::Text("Online");
    
    widget->same_line_animated(0.0f, 20.0f);
    widget->animated_status_dot(false, gui->colors.error_color, gui->colors.text_inactive, 6.0f);
    widget->same_line_animated();
    ImGui::Text("Offline");
    
    widget->same_line_animated(0.0f, 20.0f);
    widget->animated_status_dot(demo_status_active, gui->colors.warning_color, gui->colors.text_inactive, 6.0f);
    widget->same_line_animated();
    ImGui::Text("Processing");
    
    // Toggle status for demo
    demo_status_active = (fmod(ImGui::GetTime(), 2.0f) < 1.0f);
    
    widget->animated_separator();
    widget->animated_spacing(15.0f);
}

void imgui_demo::render_text_demo() {
    widget->text_with_background("📝 TEXT & STYLING", gui->colors.accent_color, gui->colors.white_color);
    widget->animated_spacing(5.0f);
    
    // Colored text
    widget->text_colored(nullptr, gui->get_color(gui->colors.success_color), "Success colored text");
    widget->text_colored(nullptr, gui->get_color(gui->colors.warning_color), "Warning colored text");
    widget->text_colored(nullptr, gui->get_color(gui->colors.error_color), "Error colored text");
    
    widget->animated_spacing(10.0f);
    
    // Centered text
    widget->text_centered("This text is centered", gui->colors.accent_color);
    
    widget->animated_spacing(10.0f);
    
    // Text with background
    widget->text_with_background("Text with background", gui->colors.element_bg, gui->colors.text_active);
    
    widget->animated_spacing(10.0f);
    
    // Different separators
    ImGui::Text("Different Separators:");
    widget->animated_separator(1.0f);
    widget->animated_spacing(5.0f);
    widget->gradient_separator(gui->colors.accent_color, gui->colors.accent_hover, 2.0f);
    widget->animated_spacing(5.0f);
    widget->animated_separator(3.0f);
    
    widget->animated_separator();
    widget->animated_spacing(15.0f);
}

void imgui_demo::render_color_demo() {
    widget->text_with_background("🎨 ANIMATED COLOR PICKERS", gui->colors.accent_color, gui->colors.white_color);
    widget->animated_spacing(5.0f);
    
    // Color edit 3
    widget->animated_color_edit3("Color RGB", demo_color3);
    
    // Color edit 4
    widget->animated_color_edit4("Color RGBA", demo_color4);
    
    widget->animated_spacing(10.0f);
    
    // Color picker 3
    widget->animated_color_picker3("Color Picker RGB", demo_color3);
    
    widget->animated_separator();
    widget->animated_spacing(15.0f);
}

void imgui_demo::render_layout_demo() {
    widget->text_with_background("📐 LAYOUT HELPERS", gui->colors.accent_color, gui->colors.white_color);
    widget->animated_spacing(5.0f);
    
    // Animated group
    widget->begin_animated_group();
    {
        ImGui::Text("This is inside an animated group");
        widget->animated_button("Group Button 1", ImVec2(120, 30));
        widget->same_line_animated();
        widget->animated_button("Group Button 2", ImVec2(120, 30));
    }
    widget->end_animated_group();
    
    widget->animated_spacing(10.0f);
    
    // Help marker
    ImGui::Text("Help Marker Example:");
    widget->same_line_animated();
    widget->help_marker("This is a help marker that shows additional information when hovered.");
    
    widget->animated_spacing(10.0f);
    
    // Tooltip
    ImGui::Text("Hover for tooltip:");
    if (ImGui::IsItemHovered()) {
        widget->animated_tooltip("This is an animated tooltip with smooth transitions!");
    }
    
    widget->animated_spacing(20.0f);
    
    // Footer
    widget->gradient_separator(gui->colors.accent_color, gui->colors.accent_hover, 2.0f);
    widget->animated_spacing(10.0f);
    widget->text_centered("🎉 End of Demo - All components are fully animated! 🎉", gui->colors.accent_color);
}

} // namespace ui
} // namespace vision
