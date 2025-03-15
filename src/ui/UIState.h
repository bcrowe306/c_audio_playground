#pragma once
#ifndef UI_STATE_H
#define UI_STATE_H

#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "core/Application.h"
#include <memory>

using std::shared_ptr;
using AppPrt = shared_ptr<Application>;

struct UIState {
    bool sidebar = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    // Fonts
    struct Fonts {
        ImFont *font_16 = nullptr;
        ImFont *font_24 = nullptr;
    } fonts;
    
    // Toolbar state
    float toolbar_height = 60.0f;
    ImVec2 toolbar_icon_size = ImVec2(24, 24);

    // Sidebar state
    float min_sidebar_width = 200.0f;
    float max_sidebar_width = 500.0f;
    ImVec2 sidebar_position = ImVec2(0, 50);
    ImVec2 sidebar_size = ImVec2(350, 600);

    // Master Channel State
    float master_channel_width = 100.0f;
    ImVec2 master_channel_position = ImVec2(0, 0);
    ImVec2 master_channel_size = ImVec2(0, 0);

    // Main Window state
    enum class MainWindowTab {
        TRACK,
        SESSION,
        MIXER,
        SONG,
     };
    MainWindowTab main_window_tab = MainWindowTab::TRACK;
    bool is_tab_selected(MainWindowTab tab){
        return main_window_tab == tab;
    }
    bool is_tab_selected(int tab){
        return main_window_tab == (MainWindowTab)tab;
    }
    ImVec2 main_position = ImVec2(0, 0);
    ImVec2 main_size = ImVec2(0, 0);
    ImColor main_color = IM_COL32(127, 127, 127, 255);

    // Footer state
    float footer_height = 40.0f;
    ImVec2 footer_position = ImVec2(0, 0);
    ImVec2 footer_size = ImVec2(0, 0);
};
struct icons {
  ImU32 play = IM_COL32(45, 184, 42, 255);
  ImU32 pause = IM_COL32(255, 0, 0, 255);
  ImU32 record = IM_COL32(255, 0, 0, 255);
  ImU32 stop = IM_COL32(255, 0, 0, 255);
  ImU32 default_color = IM_COL32(128, 128, 128, 255);
  ImU32 hover_color = IM_COL32(200, 200, 200, 255);
  ImU32 active_color = IM_COL32(0, 204, 255, 255);
  ImU32 mid_color = IM_COL32(255, 200, 0, 255);
};
struct ui_colors {
    icons icons;
    ImU32 window_heading = IM_COL32(36, 36, 36, 255);
    ImU32 window_light_background = IM_COL32(127, 127, 127, 255);
    ImU32 windows_background = IM_COL32(56, 56, 56, 255);
    ImU32 windows_dark_background = IM_COL32(29, 29, 29, 255);
    ImU32 toolbar_background = IM_COL32(56, 56, 56, 255);

    ImU32 text_default = IM_COL32(190, 190, 190, 255);
    ImU32 text_light = IM_COL32(245, 245, 245, 255);
    ImU32 text_dark = IM_COL32(48, 48, 48, 255);
    ImU32 text_selected = IM_COL32(0, 204, 255, 255);
    ImU32 wave_color = IM_COL32(0, 178, 234, 200);
};





static UIState ui_state;
static ui_colors ui_colors;
#endif // !UI_STATE_H