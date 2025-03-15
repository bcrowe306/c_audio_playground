#pragma once

#ifndef MASTER_CHANNEL_H
#define MASTER_CHANNEL_H

#include "core/Daw.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/UIState.h"
#include <iostream>
#include <string>

using namespace ImGui;
using namespace std;

void MasterChannel(AppPrt App, UIState &ui_state)
{
    SetNextWindowPos(ui_state.master_channel_position);
    SetNextWindowSize(ui_state.master_channel_size);

    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    PushStyleColor(ImGuiCol_WindowBg, ui_colors.toolbar_background);
    Begin("Master Channel", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration);

    // Draw the master channel
    Text("Master Channel");

    PopStyleColor();
    PopStyleVar();
    End();
}

#endif // !MASTER_CHANNEL_H