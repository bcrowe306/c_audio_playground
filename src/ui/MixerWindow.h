#pragma once

#ifndef MIXER_WINDOW_H
#define MIXER_WINDOW_H

#include "core/Daw.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/UIState.h"
#include <iostream>
#include <string>

using namespace ImGui;

void MixerWindow(AppPrt App, UIState &ui_state) {
  SetNextWindowPos(ui_state.main_position);
  SetNextWindowSize(ui_state.main_size);
  PushStyleColor(ImGuiCol_WindowBg, ui_colors.window_light_background);
  Begin("Mixer Window", NULL,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoDecoration);
  Text("Mixer Window");
  PopStyleColor();
  End();
}

#endif // !MIXER_WINDOW_H