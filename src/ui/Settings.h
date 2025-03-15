#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include "core/Daw.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "ui/UIState.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>

using namespace ImGui;
using std::string;

inline void Settings(AppPrt App, ImVec2 &position, ImVec2 &size, UIState &ui_state){
    BeginTabBar("SettingsTabs");
    if (BeginTabItem("General"))
    {
        Text("General Settings");
        EndTabItem();
    }
    if (BeginTabItem("Midi"))
    {
        int i = 0;
        auto midi_input_devices = App->daw->midi_engine->inputDevices;
        auto midi_output_devices = App->daw->midi_engine->outputDevices;
        Text("Input Devices");
        for(auto &device : midi_input_devices){
            PushID(i);
            Text("%s", device.first.c_str()); SameLine();
            Checkbox("Track##xx", &device.second->enabled); SameLine();
            Checkbox("Sync##xy", &device.second->sync); SameLine();
            Checkbox("Remote##xz", &device.second->remote);
            PopID();
            NewLine();
            i++;
        }
        Separator();
        Text("Output Devices");
        for(auto &device : midi_output_devices){
            PushID(i);
            Text("%s", device.first.c_str()); SameLine();
            Checkbox("Track##xx", &device.second->enabled); SameLine();
            Checkbox("Sync##xy", &device.second->sync); SameLine();
            PopID();
            NewLine();
            i++;
        }
        
        Separator();
        Text("Control Scripts");
        // Three select boxes in same line, first box is the script, second is the input device, thrid is the output device...
        auto control_scripts = App->daw->midi_script_handler->control_scripts;
        if(BeginCombo("Control Script", "Select Script")){
            for(auto &script : control_scripts){
                bool is_selected = false;
                if(Selectable(script.name.c_str(), is_selected)){
                    // Set the selected script
                }
            }
            EndCombo();
        }
        
        

        EndTabItem();
    }
    if (BeginTabItem("Audio"))
    {
        Text("Audio Settings");
        EndTabItem();
    }
    EndTabBar();
}

#endif // !SETTINGS_H