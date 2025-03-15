#pragma once

#ifndef PLAY_BUTTON_H
#define PLAY_BUTTON_H

#include "imgui.h"
#include "hello_imgui/hello_imgui.h"
#include "ui/UIState.h"

using namespace ImGui;



bool PlayButton(ImVec2 size, bool &is_active, ImU32 default_color = ui_colors.icons.default_color, ImU32 active_color = ui_colors.icons.play)
{
    // Play Button
    bool clicked = false;
    auto triangleSize = size.x;
    auto playbuttonColor = is_active ? active_color : default_color;
    auto playButton = InvisibleButton("playbuttonFunctionality",
                                            size);
    ImDrawList *draw_list = GetWindowDrawList();

    if(IsItemHovered()){
        if(!is_active){
            playbuttonColor = ui_colors.icons.hover_color;
        }
    }
    auto p = GetItemRectMin();
    auto left_padding = 0.f;
    auto start_y = p.y + 5;
    auto trigger_start = ImVec2(p.x + left_padding, start_y);
    auto trigger_middle = ImVec2(trigger_start.x + triangleSize,
                                trigger_start.y + triangleSize / 2);
    auto trigger_end = ImVec2(trigger_start.x, trigger_start.y + triangleSize);
    draw_list->AddTriangleFilled(trigger_start, trigger_middle, trigger_end,
                                playbuttonColor);
    if (playButton) {
        clicked = true;
    }
    return clicked;
}

#endif // !PLAY_BUTTON_H