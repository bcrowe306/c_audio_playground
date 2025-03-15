#pragma once

#include <algorithm>
#ifndef RECORD_BUTTON_H
#define RECORD_BUTTON_H

#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/UIState.h"

using namespace ImGui;

bool RecordButton(ImVec2 size, bool &is_active,
                ImU32 default_color = ui_colors.icons.default_color,
                ImU32 active_color = ui_colors.icons.record) {
    // Play Button
    bool clicked = false;
    auto triangleSize = size.x;
    auto record_button_color = is_active ? active_color : default_color;
    auto record_button = InvisibleButton("record_buttonFunctionality", size);
    ImDrawList *draw_list = GetWindowDrawList();

    if (IsItemHovered()) {
        if (!is_active) {
            record_button_color = ui_colors.icons.hover_color;
        }
    }
    auto p = GetItemRectMin();
    auto p_max = GetItemRectMax();
    p.y = p.y + 5;
    p_max.y = p_max.y + 5;
    auto diameter = std::min(size.x, size.y);
    auto center = ImVec2(p.x + size.x / 2, p.y + size.y / 2);
    auto radius = diameter / 2;
    draw_list->AddCircleFilled(center, radius, record_button_color);
    if (record_button) {
        clicked = true;
    }
    return clicked;
}

#endif // !RECORD_BUTTON_H