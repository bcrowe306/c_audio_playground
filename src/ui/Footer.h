#pragma once

#ifndef FOOTER_H
#define FOOTER_H

#include "core/Daw.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/UIState.h"
#include <iostream>
#include <string>

using namespace ImGui;
using namespace std;



inline void Footer(AppPrt App, UIState &ui_state){
    SetNextWindowPos(ui_state.footer_position);
    SetNextWindowSize(ui_state.footer_size);

    // Set window padding to 0
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    PushStyleColor(ImGuiCol_WindowBg, ui_colors.toolbar_background);
    Begin("Footer", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration);
    std::string buttons[4] = {"Track", "Session", "Mixer", "Song"};

    // Draw Four Buttons at the on this footer equal width and equal spaced in the center
    float button_width = 100.0;
    float button_spacing = 1.0;

    // Calculate the starting x position for the buttons to be centered

    ImDrawList *  draw_list = GetWindowDrawList();
    PushFont(ui_state.fonts.font_24);
    float start_x = (ui_state.footer_size.x - (button_width * 4 + button_spacing * 3)) / 2;
    int selected_button = 0;
    for(int i = 0; i < 4; i++){
        if(i > 0){
            SameLine();
        }
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(button_spacing, 0));
        PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5, 0.5));
        PushStyleColor(ImGuiCol_Text, ui_colors.text_default);
        SetCursorPosX(start_x + i * (button_width + button_spacing));
        bool tab_selected = ui_state.is_tab_selected(i);
        auto button_bg_color = tab_selected ? ui_colors.window_light_background : ui_colors.toolbar_background;
        auto text_color = tab_selected ? ui_colors.text_dark : ui_colors.text_default;
        // set the x position based on the button number i
        SetCursorPosX(start_x + i * (button_width + button_spacing));
        if(InvisibleButton(buttons[i].c_str(), ImVec2(button_width, ui_state.footer_height))){
            ui_state.main_window_tab = (UIState::MainWindowTab)i;
        }
        if(IsItemHovered()){
            text_color = ui_colors.text_selected;
        }
        draw_list->AddRectFilled(GetItemRectMin(), GetItemRectMax(), button_bg_color);
        // Draw centered text in button
        auto text_size = CalcTextSize(buttons[i].c_str());
        auto text_pos = GetItemRectMin();
        text_pos.x = text_pos.x + (button_width - text_size.x) / 2;
        text_pos.y = text_pos.y + (ui_state.footer_height - text_size.y) / 2;
        draw_list->AddText(text_pos, text_color, buttons[i].c_str());
        
        
        PopStyleVar(6);
        PopStyleColor(1);
    }
    PopFont();
    PopStyleVar();
    PopStyleColor();
    End();

}

#endif // !FOOTER_H