#pragma once

#ifndef SONG_WINDOW_H
#define SONG_WINDOW_H

#include "core/Daw.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/UIState.h"
#include <iostream>
#include <string>

using namespace ImGui;

void SongWindow(AppPrt App, UIState &ui_state)
{
    SetNextWindowPos(ui_state.main_position);
    SetNextWindowSize(ui_state.main_size);
    PushStyleColor(ImGuiCol_WindowBg, ui_colors.window_light_background);
    Begin("Song Window", NULL,
          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
              ImGuiWindowFlags_NoDecoration);
    Text("Song Window");
    PopStyleColor();
    End();
}

#endif // !SONG_WINDOW_H