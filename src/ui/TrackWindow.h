#pragma once

#include "AudioSampler.h"
#ifndef TRACK_WINDOW_H
#define TRACK_WINDOW_H

#include "core/Daw.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/AudioSamplerWidget.h"
#include "ui/UIState.h"
#include <iostream>
#include <string>
using namespace ImGui;

void TrackWindow(AppPrt App, UIState &ui_state)
{
    SetNextWindowPos(ui_state.main_position);
    SetNextWindowSize(ui_state.main_size);
    PushStyleColor(ImGuiCol_WindowBg, ui_colors.window_light_background);
    Begin("Track Window", NULL,
          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
              ImGuiWindowFlags_NoDecoration);
    
    PopStyleColor();
    End();
}

#endif // !TRACK_WINDOW_H