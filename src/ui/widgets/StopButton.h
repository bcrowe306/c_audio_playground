#pragma once

#ifndef STOP_BUTTON_H
#define STOP_BUTTON_H

#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/UIState.h"

using namespace ImGui;

bool StopButton(ImVec2 size,
                ImU32 default_color = ui_colors.icons.default_color,
                ImU32 active_color = ui_colors.icons.hover_color) {
    // Play Button
    bool clicked = false;
    auto triangleSize = size.x;
    auto stop_button_color = default_color;
    auto stop_button = InvisibleButton("stop_buttonFunctionality", size);
    ImDrawList *draw_list = GetWindowDrawList();

    if (IsItemHovered()) {
        stop_button_color = ui_colors.icons.hover_color;
    }
    auto p = GetItemRectMin();
    auto p_max = GetItemRectMax();
    p.y = p.y + 5;
    p_max.y = p_max.y + 5;
    draw_list->AddRectFilled(p, p_max, stop_button_color);
    if (stop_button) {
        clicked = true;
    }
    return clicked;
}

#endif // !STOP_BUTTON_H