#pragma once

#ifndef MENU_BUTTON_H
#define MENU_BUTTON_H

#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/UIState.h"

using namespace ImGui;

bool MenuButton(ImVec2 size,
                bool &is_active,
                ImU32 default_color = ui_colors.icons.default_color,
                ImU32 active_color = ui_colors.icons.hover_color) {
    // Play Button
    bool clicked = false;
    auto triangleSize = size.x;
    auto menu_button_color = is_active ? active_color : default_color;
    auto menu_button = InvisibleButton("menu_buttonFunctionality", size);
    ImDrawList *draw_list = GetWindowDrawList();

    if (IsItemHovered()) {
        if(!is_active){
            menu_button_color = ui_colors.icons.hover_color;
        }
    }
    auto p = GetItemRectMin();
    auto p_max = GetItemRectMax();
    p.y = p.y + 5;
    p_max.y = p_max.y + 5;
    
    // Draw three line menu button
    auto line1 = ImVec2(p.x + 5, p.y + 5);
    auto line2 = ImVec2(p.x + size.x - 5, p.y + 5);
    auto line3 = ImVec2(p.x + size.x / 2, p.y + size.y - 5);
    draw_list->AddLine(line1, line2, menu_button_color, 2);
    draw_list->AddLine(ImVec2(line1.x, line1.y + 5), ImVec2(line2.x, line2.y + 5), menu_button_color, 2);
    draw_list->AddLine(ImVec2(line1.x, line1.y + 10), ImVec2(line2.x, line2.y + 10), menu_button_color, 2);
    if (menu_button) {
        clicked = true;
    }
    return clicked;
}

#endif // !MENU_BUTTON_H