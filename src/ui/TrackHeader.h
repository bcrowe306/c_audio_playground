#pragma once

#ifndef TRACK_HEADER_H
#define TRACK_HEADER_H

#include "core/Daw.h"
#include "core/TrackNode.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/UIState.h"

using namespace ImGui;
inline void TrackHeader(AppPrt App, shared_ptr<TrackNode> track, bool selected, UIState ui_state, ImVec2 position) {
    if(track == nullptr){
        return;
    }
    SetCursorPos(position);
    InvisibleButton(track->name.get_value().c_str(), ui_state.track_header_size);
    ImVec2 pMin = ImGui::GetItemRectMin();
    ImVec2 pMax = ImGui::GetItemRectMax();
    ImDrawList *draw_list = GetWindowDrawList();
    
    auto text_color = ui_colors.text_default;
    auto bg_color = ui_colors.window_heading;
    if(IsItemHovered()){
        text_color = ui_colors.text_light;
        bg_color = ui_colors.windows_background;

    }
    
    if(IsItemClicked()){
        auto track_index = App->Tracks.get_track_index(track);
        App->Tracks.select_track(track_index);
    }
    if(IsItemClicked(1)){
        auto track_index = App->Tracks.get_track_index(track);
        App->Tracks.remove_track(track_index);
    }
    text_color = selected ? ui_colors.text_light : text_color;
    bg_color = selected ? ui_colors.windows_background : bg_color;
    draw_list->AddRectFilled(pMin, pMax, bg_color);
    auto label = track->name.get_value();
    auto text_size = CalcTextSize(label.c_str());
    auto text_pos = ImVec2(pMin.x + (ui_state.track_header_size.x - text_size.x) / 2, pMin.y + (ui_state.track_header_size.y - text_size.y) / 2);
    draw_list->AddText(text_pos, text_color, label.c_str());
}

#endif // !TRACK_HEADER_H