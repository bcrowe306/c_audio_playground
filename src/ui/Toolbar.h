#pragma once
#include "Playhead.h"
#include "imgui.h"
#include "ui/widgets/IconWidget.h"
#include "ui/widgets/MenuButton.h"
#include "ui/widgets/RecordButton.h"
#include "ui/widgets/StopButton.h"
#ifndef TOOLBAR_H
#define TOOLBAR_H


#include <iostream>
#include "hello_imgui/hello_imgui.h"
#include "core/Daw.h"
#include "ui/UIState.h"
#include "ui/widgets/include.h"

using namespace ImGui;
void Toolbar(AppPrt App, ImVec2 position, ImVec2 size, UIState &ui_state)
{
    SetNextWindowPos(position);
    SetNextWindowSize(size);
    
    ImGui::Begin("Toolbar", NULL ,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoDecoration);

    PushFont(ui_state.fonts.font_24);
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(25, 10));
    SetCursorPosY(15);
    auto song_position = App->Transport.get_song_position_string();

    // Sidebar Button
    if(MenuButton(ui_state.toolbar_icon_size, ui_state.sidebar)){
        ui_state.sidebar = !ui_state.sidebar;
    }
    SameLine();

    // Space filler
    Dummy(ImVec2(280, 0));
    SameLine();

    // Laumch Quantization Combo
    auto launch_quantization = App->Transport.get_launch_quantization();
    SetNextItemWidth(120);

    if (BeginCombo("LQ",
                   App->Transport.get_launch_quantization_string().c_str())) {
      for (int n = 0; n < launch_quantization_values.size(); n++) {
        bool is_selected = (launch_quantization == n);
        if (Selectable(launch_quantization_strings[n].c_str(), is_selected)) {
          App->Transport.set_launch_quantization(n);
        }
        if (is_selected) {
          SetItemDefaultFocus();
        }
      }
      EndCombo();
    }
    SameLine();

    // Metronome Button
    int metronome_type = App->Transport.get_metronome_type();
    bool metronome_enabled = App->Transport.get_metronome_enabled();
    if(MetronomeButton(ui_state.toolbar_icon_size, metronome_enabled, metronome_type)){
        App->Transport.toggle_metronome();
    }
    SameLine();


    // Tempo Slider
    static auto tempo = App->Transport.get_tempo();
    SetNextItemWidth(120);
    if(DragFloat("##Tempo", &tempo, 1.0f, 20.0f, 300.0f, "%.2f BPM")){
        App->Transport.set_tempo(tempo);
    };

    SameLine();
    SetNextItemWidth(120);
    Text("%s", song_position.c_str());

    SameLine();

    // Record Button
    bool recording = App->Transport.is_recording();
    if (RecordButton(ui_state.toolbar_icon_size, recording))
    {
        App->Transport.toggle_record();
    }
    ImGui::SameLine();

    // Play Button
    bool playing = App->Transport.is_playing();
    SetNextItemShortcut(ImGuiKey_Space);
    if (PlayButton(ui_state.toolbar_icon_size, playing)) {
      App->Transport.toggle_play();
    }
    ImGui::SameLine();

    // Stop Button
    if (StopButton(ui_state.toolbar_icon_size)) {
      App->Transport.stop();
    }
    ImGui::SameLine();
    
    PopFont();
    PopStyleVar();
    ImGui::End();
};


#endif // !TOOLBAR_H
