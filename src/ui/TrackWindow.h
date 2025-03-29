#pragma once

#include "AudioSampler.h"
#include <memory>
#ifndef TRACK_WINDOW_H
#define TRACK_WINDOW_H

#include "core/Daw.h"
#include "ui/TrackHeader.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "core/TrackNode.h"
#include "ui/UIState.h"
#include "ui/AudioSamplerWidget.h"
#include <iostream>
#include <string>

using namespace ImGui;

inline void TrackWindow(AppPrt App, UIState &ui_state)
{
    auto session_height = ui_state.main_size.y - ui_state.track_view_section_height;
    auto track_view_details_width = ui_state.main_size.x - ui_state.track_view_main_width;
    SetNextWindowPos(ui_state.main_position);
    SetNextWindowSize(ImVec2(ui_state.main_size.x, ui_state.main_size.y));
    PushStyleColor(ImGuiCol_WindowBg, ui_colors.window_light_background);
    Begin("Track Window", NULL,
          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
              ImGuiWindowFlags_NoDecoration);

    SetNextWindowPos( ImVec2(ui_state.main_position.x, ui_state.main_position.y));
    SetNextWindowSize(ImVec2(ui_state.main_size.x, session_height));
    BeginChild("Tracks");
        auto i = 0;
        for(auto &track : *App->Tracks.get_tracks()){
            auto start_x = i * ui_state.track_header_size.x;
            bool isSelected = i == App->Tracks.get_selected_track_index();
            if(track == nullptr){
                continue;
            }
            TrackHeader(App, track, isSelected, ui_state, ImVec2(start_x, 0));
            // Text(track->name.get_value().c_str());
            SameLine();
            i++;
        }
    EndChild();
    PopStyleColor();

    // Track View Section
    SetNextWindowPos(ImVec2(ui_state.main_position.x,session_height + ui_state.toolbar_height));
    SetNextWindowSize(ImVec2(ui_state.main_size.x, ui_state.track_view_section_height));
    PushStyleColor(ImGuiCol_ChildBg, ui_colors.windows_background);
    BeginChild("Track View");
    auto selected_track = App->Tracks.get_selected_track();
    if(selected_track != nullptr){

        // Track View Main Section. This has the track name as a header, volume fader, pan, and mute/solo buttons.
        PushStyleColor(ImGuiCol_ChildBg, ui_colors.windows_dark_background);
        SetNextWindowSize(ImVec2(ui_state.track_view_main_width, ui_state.track_view_section_height));
        BeginChild("Track Main");
        auto track_name = selected_track->name.get_value();
        auto track_name_size = CalcTextSize(track_name.c_str());
        auto track_name_pos = ImVec2(ui_state.track_view_main_width / 2 - track_name_size.x / 2, 0);
        // center track name
        SetCursorPosX(ui_state.track_view_main_width / 2 - track_name_size.x / 2);
        Text(track_name.c_str(), track_name_pos);
        EndChild();
        PopStyleColor();

        // Track View Details Section. This has the Clip, Device, Effects.
        PushStyleColor(ImGuiCol_ChildBg, ui_colors.windows_background);
        SetNextWindowSize(ImVec2(track_view_details_width, ui_state.track_view_section_height));
        SetNextWindowPos(ImVec2(ui_state.main_position.x + ui_state.track_view_main_width, session_height + ui_state.toolbar_height));
        BeginChild("Track Details");
        
        BeginTabBar("Track Details View");
        if (BeginTabItem("Clip")) {
            Text("Clip Details");
            EndTabItem();
        }
        if (BeginTabItem("Device")) {
            Text("Device Details");
            EndTabItem();
        }
        if (BeginTabItem("Effects")) {
            Text("Effects Details");
            EndTabItem();
        }
        if (BeginTabItem("Automation")) {
            Text("Automation Details");
            EndTabItem();
        }
        EndTabBar();
        EndChild();
        PopStyleColor();
    }

    EndChild();
    PopStyleColor();


    
    End();
}

#endif // !TRACK_WINDOW_H