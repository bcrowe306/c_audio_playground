#pragma once
#include "Application.h"
#ifndef MAIN_H
#define MAIN_H

#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/Toolbar.h"
#include "ui/Sidebar.h"
#include "ui/MainWindow.h"
#include "ui/Footer.h"
#include "ui/MasterChannel.h"

#include "ui/UIState.h"

void main_ui(AppPrt App){
    // Get the window size
    auto space = ImGui::GetContentRegionAvail();
    // auto screenSize = runnerParams->appWindowParams.windowGeometry.size;
    float window_width = space.x;
    float window_height = space.y;
    // Set up panel sizes and positions
        
    ImGui::GetStyle().WindowRounding = 0.0f;
    ImGui::GetStyle().WindowBorderSize = 0.1f;
    ImGui::GetStyle().FrameRounding = 0.0f;
    ImGui::GetStyle().GrabRounding = 0.0f;
    ImGui::GetStyle().PopupRounding = 0.0f;
    ImGui::GetStyle().ScrollbarRounding = 0.0f;

    // Set the sidebar position and size
    ui_state.sidebar_position.y = ui_state.toolbar_height;
    ui_state.sidebar_size.y = window_height - ui_state.toolbar_height - ui_state.footer_height;

    // set the master channel position and size
    ui_state.master_channel_position.y = ui_state.toolbar_height;
    ui_state.master_channel_size.y = window_height - ui_state.toolbar_height - ui_state.footer_height;
    ui_state.master_channel_position.x = window_width - ui_state.master_channel_width;
    ui_state.master_channel_size.x = ui_state.master_channel_width;

    // Set the main window position and size
    if(ui_state.sidebar){
        ui_state.main_position.x = ui_state.sidebar_size.x;
        ui_state.main_position.y = ui_state.toolbar_height;
        ui_state.main_size.x = window_width - ui_state.sidebar_size.x - ui_state.master_channel_width;
        ui_state.main_size.y = window_height - ui_state.footer_height - ui_state.toolbar_height;

    }
    else {
        ui_state.main_position.x = 0;
        ui_state.main_position.y = ui_state.toolbar_height;
        ui_state.main_size.x = window_width - ui_state.master_channel_width;
        ui_state.main_size.y = window_height - ui_state.footer_height - ui_state.toolbar_height;
    }
    
    // Set the footer position and size
    ui_state.footer_position.y = window_height - ui_state.footer_height;
    ui_state.footer_size.x = window_width;
    ui_state.footer_size.y = ui_state.footer_height;

    // Toolbar
    Toolbar(App, {0, 0}, {window_width, ui_state.toolbar_height}, ui_state);

    // Sidebar
    if (ui_state.sidebar) {
      Sidebar(App, ui_state.sidebar_position,
              ui_state.sidebar_size,
              ui_state);
    }

    // Main Window
    MainWindow(App, ui_state);

    MasterChannel(App, ui_state);

    // Footer
    Footer(App, ui_state);
    // AudioSamplerWidget(daw, {sidebar_width, toolbar_height}, {window_width- sidebar_width, main_height + footer_height});
};

#endif // !MAIN_H