#pragma once
#include "imgui.h"
#include <algorithm>
#ifndef SIDEBAR_H
#define SIDEBAR_H


#include <iostream>
#include <filesystem>
#include <string>
#include "core/Daw.h"
#include "hello_imgui/hello_imgui.h"
#include "ui/UIState.h"
#include <algorithm>
#include "imgui.h"

#include "ui/Settings.h"

using namespace ImGui;
using namespace std;
using namespace std::filesystem;

void Sidebar(AppPrt App, ImVec2 &position, ImVec2 &size, UIState &ui_state){
    static string selected_file = "";
    float resize_handle_width = 7.0f;
    SetNextWindowSize( size);
    SetNextWindowPos(position);
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 0));
    
    auto space = ImGui::GetContentRegionAvail();
    ImVec2 selectable_size = ImVec2(size.x - resize_handle_width - 10, 0);
    bool selected = false;


    Begin("Sidebar", NULL,  
        ImGuiWindowFlags_NoMove 
                | ImGuiWindowFlags_NoCollapse 
                | ImGuiWindowFlags_NoDecoration
                );

    BeginTabBar("SidebarTabs");

        if (BeginTabItem("Browser"))
        {
            path sample_path = "/Users/bcrowe/Documents/Audio Samples/BVKER - Elevate Beamaker Kit/Tonal Shots";

            for (const auto & entry : filesystem::directory_iterator(sample_path))
            {
                if (entry.is_directory())
                {
                    // Text(entry.path().filename().string().c_str());
                }
                else
                {
                    if( Selectable( entry.path().filename().string().c_str(), &selected, 0, selectable_size) ){
                      // get full path of selected file
                        selected_file = entry.path().filename().string();
                        // daw.audio_sampler->audio_buffer->load_sample(entry.path().string());
                    }
                }
            }
            EndTabItem();
        }
        if (BeginTabItem("Devices"))
        {
            Text("Devices");
            EndTabItem();
        }
        if (BeginTabItem("Settings"))
        {
            Settings(App, position, size, ui_state);
            EndTabItem();
        }
    EndTabBar();

    // Resize handle
    SameLine();
    SetCursorPosX(size.x - resize_handle_width-2);
    SetCursorPosY(0);
    ImGui::InvisibleButton("##resize_handle",
                           ImVec2(5.0f, ImGui::GetContentRegionAvail().y),
                           ImGuiButtonFlags_MouseButtonLeft);
                           
    // Check if mouse is hovering resize handle
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(
          ImGuiMouseCursor_ResizeEW); // East-West resize cursor
    // Check if dragging
    if (ImGui::IsItemActive()) {
        float sidebarWidth = size.x + ImGui::GetIO().MouseDelta.x; // Adjust width based on mouse movement
        ui_state.sidebar_size.x = std::clamp(sidebarWidth, ui_state.min_sidebar_width, ui_state.max_sidebar_width); // Clamp to min/max width
    }
    // Draw a visual separator
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 pMin = ImGui::GetItemRectMin();
    ImVec2 pMax = ImGui::GetItemRectMax();
    drawList->AddLine(ImVec2(pMin.x + 2.5f, pMin.y),
                      ImVec2(pMin.x + 2.5f, pMax.y),
                      IM_COL32(200, 200, 200, 128), resize_handle_width);

    ImGui::SameLine();
    PopStyleVar();
    End();
}

#endif // !SIDEBAR_H