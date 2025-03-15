#pragma once

#include <algorithm>
#ifndef METRONOME_BUTTON_H
#define METRONOME_BUTTON_H

#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/UIState.h"

using namespace ImGui;

bool MetronomeButton(ImVec2 size, bool &is_active,
                int metronome_type = -1,
                ImU32 default_color = ui_colors.icons.default_color,
                ImU32 active_color = ui_colors.icons.mid_color) {
    // Play Button
    bool clicked = false;
    auto triangleSize = size.x;
    auto metronome_button_color = is_active ? active_color : default_color;
    auto metronome_button = InvisibleButton("metronome_buttonFunctionality", size);
    ImDrawList *draw_list = GetWindowDrawList();

    if (IsItemHovered()) {
        if (!is_active) {
            metronome_button_color = ui_colors.icons.hover_color;
        }
    }
    auto p = GetItemRectMin();
    auto p_max = GetItemRectMax();
    p.y = p.y + 5;
    p_max.y = p_max.y + 5;
    auto diameter = std::min(size.x, size.y);
    auto center = ImVec2(p.x + size.x / 2, p.y + size.y / 2);
    auto radius = diameter / 2;
    draw_list->AddCircle(center, radius, metronome_button_color, 0, 4);


    // Draw inner circle
    if(metronome_type != 0){
      auto inner_radius = radius - 7;
      draw_list->AddCircleFilled(center, inner_radius, metronome_button_color,
                                 0);
    }
    
    if (metronome_button) {
        clicked = true;
    }
    return clicked;
}

#endif // !METRONOME_BUTTON_H