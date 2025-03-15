#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "ui/UIState.h"
#include "ui/widgets/Fader.h"
#include "ui/widgets/ToggleButton.h"
#ifndef AUDIOSAMPLERWIDGET_H
#define AUDIOSAMPLERWIDGET_H

#include <iostream>
#include <string>
#include "hello_imgui/hello_imgui.h"
#include "core/Daw.h"
#include "library/utility.h"
#include <algorithm>
#include "ui/widgets/include.h"

using namespace ImGui;
static bool audition_button_state = false;
static float get_unit_position(float position, float min, float max){
    return (position - min) / (max - min);
}
void AudioSamplerWidget(Daw &daw, ImVec2 position, ImVec2 size){
        auto audio_sampler = daw.audio_sampler;
        //!! You might want to use these ^^ values in the OS window instead, and add the ImGuiWindowFlags_NoTitleBar flag in the ImGui window !!


        ImGui::BeginChild("AudioSampler", size, ImGuiChildFlags_None);


        // Waveform size drag widget starting x = 400, y = 150
        static int waveform_size_x = 600;
        static int waveform_size_y = 150;
        static float y_scale = 0.5f;
        static int _start = 0;
        static int _end = 44100;

        // Waveform size drag widget
        DragInt("_start", &_start, 100, 0, 44100);
        DragInt("_end", &_end, 100, 0, audio_sampler->audio_buffer->get_end());


        PushStyleColor(ImGuiCol_ChildBg, ui_colors.windows_background);
        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        ImVec2 waveform_size = ImVec2(waveform_size_x, waveform_size_y);
        ImVec2 waveform_header_size = ImVec2(waveform_size.x, 20);
        float line_height = waveform_size.y - waveform_header_size.y;
        ImVec2 line_handle_size = ImVec2(8, 8);
        
        
        BeginChild("AudioSamplerHeader", waveform_size);
        auto plots = audio_sampler->audio_buffer->get_audio_peaks(_start, _end, waveform_size_x);

        auto averages = audio_sampler->audio_buffer->get_audio_averages(_start, _end, waveform_size_x);

        auto beginning_position = GetCursorScreenPos();
        static float handle1_x = beginning_position.x;
        static float handle2_x = beginning_position.x + waveform_size.x - line_handle_size.x;

        // Draw waveform header
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImVec2(ImGui::GetCursorScreenPos().x + waveform_header_size.x,
                   ImGui::GetCursorScreenPos().y + waveform_header_size.y),
            ui_colors.window_heading);

        // Header Text
        auto start_unit_position = get_unit_position(handle1_x, beginning_position.x, beginning_position.x + waveform_size.x);
        auto end_unit_position = get_unit_position(handle2_x, beginning_position.x, beginning_position.x + waveform_size.x);
        Text("%s", audio_sampler->audio_buffer->get_file_name().c_str());
        
        auto plot_0_y = beginning_position.y + waveform_header_size.y + (line_height/2);
        // Manually draw waveform
        // for(int i = 0; i < waveform_size_x; i++){
        //     auto plot = (*plots)[i];
        //     auto x = beginning_position.x + i;
        //     auto max_y = plot_0_y - (plot.max * y_scale * line_height);
        //     auto min_y = plot_0_y - (plot.min * y_scale * line_height);
        //     draw_list->AddLine(ImVec2(x, max_y), ImVec2(x, min_y), ui_colors.wave_color);
        // }

        for (int i = 0; i < waveform_size_x; i++)
        {
          // Normalize values (assuming samples are in [-1, 1])
          float meanHeight = (*averages)[i].meanAbs * line_height * y_scale;
          float rmsHeight = (*averages)[i].rms * line_height * y_scale;
          float meanY = plot_0_y - meanHeight;
          float rmsY = plot_0_y - rmsHeight;
          auto x = beginning_position.x + i;
          // Draw the bars (Mean in white, RMS in red)
        draw_list->AddRectFilled(ImVec2(x, meanY), ImVec2(x + 1, plot_0_y), ui_colors.text_selected);
        draw_list->AddRectFilled(ImVec2(x, plot_0_y), ImVec2(x + 1, plot_0_y + meanHeight), ui_colors.text_selected);

        // Draw the bars (Mean in white, RMS in red)
        }



        // Draw first vertical line for start position.
        SetCursorScreenPos(ImVec2(handle1_x, beginning_position.y + waveform_header_size.y));
        InvisibleButton("start_position_handle", line_handle_size);
        auto handle_min = GetItemRectMin();
        draw_list->AddRectFilled(handle_min, GetItemRectMax(), ui_colors.icons.play);
        draw_list->AddLine(handle_min, ImVec2(handle_min.x, handle_min.y + line_height),
                           ui_colors.icons.play);
        // Hover changes mouse cursor to hand
        if (IsItemHovered()) {
          SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        // Dragging the line
        if (IsItemActive())
        {
            auto mouse_delta = ImGui::GetIO().MouseDelta.x;
            handle1_x = std::clamp(handle1_x + mouse_delta, beginning_position.x, beginning_position.x + waveform_header_size.x - line_handle_size.x);
        }

        // Draw second vertical line for end position.
        SetCursorScreenPos(ImVec2(handle2_x, beginning_position.y + waveform_header_size.y));
        InvisibleButton("end_position_handle", line_handle_size);
        auto handle2_min = GetItemRectMin();
        draw_list->AddRectFilled(handle2_min, GetItemRectMax(), ui_colors.icons.record);
        draw_list->AddLine(
            ImVec2(handle2_min.x + line_handle_size.x - 1, handle2_min.y),
            ImVec2(handle2_min.x + line_handle_size.x - 1, handle2_min.y + line_height),
            ui_colors.icons.record);
        // Hover changes mouse cursor to hand
        if (IsItemHovered()) {
          SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        // Dragging the line
        if (IsItemActive())
        {
            auto mouse_delta = ImGui::GetIO().MouseDelta.x;
            handle2_x = std::clamp(handle2_x + mouse_delta, beginning_position.x, beginning_position.x + waveform_size.x - line_handle_size.x);
        }
        float zoom = 1.0f;
        EndChild();
        PopStyleColor();
        PopStyleVar();

        ImGui::EndChild();
}
#endif // !AUDIOSAMPLERWIDGET_H