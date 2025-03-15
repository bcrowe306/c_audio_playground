#pragma once

#ifndef SESSION_WINDOW_H
#define SESSION_WINDOW_H

#include "core/Daw.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/UIState.h"
#include <iostream>
#include <string>

using namespace ImGui;

void SessionWindow(AppPrt App, UIState &ui_state) {
  SetNextWindowPos(ui_state.main_position);
  SetNextWindowSize(ui_state.main_size);
  PushStyleColor(ImGuiCol_WindowBg, ui_colors.window_light_background);
  Begin("Session Window", NULL,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoDecoration);
  Text("Session Window");
  PopStyleColor();
  End();
}

#endif // !SESSION_WINDOW_H