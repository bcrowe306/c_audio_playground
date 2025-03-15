#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <string>
#include "core/Daw.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "UIState.h"
#include "TrackWindow.h"
#include "SessionWindow.h"
#include "MixerWindow.h"
#include "SongWindow.h"

using namespace ImGui;
using namespace std;

void MainWindow(AppPrt App, UIState &ui_state) {

    

    SetNextWindowPos(ui_state.main_position);
    SetNextWindowSize(ui_state.main_size);
    PushStyleColor(ImGuiCol_WindowBg, (ImU32)ui_state.main_color);
    Begin("Main Window", NULL,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoDecoration);
    
    switch(ui_state.main_window_tab){
        case UIState::MainWindowTab::TRACK:
            TrackWindow(App, ui_state);
            break;
        case UIState::MainWindowTab::SESSION:
            SessionWindow(App, ui_state);
            break;
        case UIState::MainWindowTab::MIXER:
            MixerWindow(App, ui_state);
            break;
        case UIState::MainWindowTab::SONG:
            SongWindow(App, ui_state);
            break;
    }
    
    PopStyleColor();
    End();
}

#endif // !MAINWINDOW_H
