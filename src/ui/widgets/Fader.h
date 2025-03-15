#pragma once
#ifndef UI_WIDGETS_FADER_H
#define UI_WIDGETS_FADER_H

#include "imgui.h"
#include <algorithm> // For std::clamp
#include <cmath>
#include "ui/widgets/StereoMeter.h"

bool FaderWidget(const char *label, float *dB, float peakLevel, float rms_left, float rms_right, float peak_left, float peak_right, ImVec2 pos,
                 ImVec2 size,
                 ImU32 faderColorTop = IM_COL32(180, 180, 230, 255),
                 ImU32 faderColorBottom = IM_COL32(30, 30, 50, 255)) {
  ImGui::PushID(label);

  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  bool isActive = false;
  float min_dB = -60.0f; // Represents -∞
  float max_dB = 6.0f;

  float textFieldHeight = 19.0f; // Height for both text fields
  float spacing = 16.0f;          // Space between elements

  float markingOffset = 8.0f;        // Space for dB tick lines
  float trackWidth = 8; // Narrower track
  float faderWidth = 16;  // Wider fader handle
  float trackX = pos.x + (size.x - trackWidth) * 0.5f;

  // Adjust position to account for text fields
  ImVec2 faderAreaPos = ImVec2(pos.x, pos.y + textFieldHeight + spacing);
  ImVec2 faderAreaSize =
      ImVec2(size.x, size.y - 2 * (textFieldHeight + spacing));

  // Convert dB to a normalized range (0.0 to 1.0)
  float normValue = (*dB - min_dB) / (max_dB - min_dB);
  normValue = std::clamp(normValue, 0.0f, 1.0f);

  // Compute fader position
  float faderHeight = 30; // 12% of total height
  float faderY = faderAreaPos.y + faderAreaSize.y -
                 (normValue * faderAreaSize.y) - (faderHeight * 0.5f);

  // Darker track and markings
  ImU32 trackColor = IM_COL32(10, 10, 10, 255);
  ImU32 markingColor = IM_COL32(100, 100, 100, 255);

  // Draw fader track
  ImVec2 trackMin = ImVec2(trackX, faderAreaPos.y);
  ImVec2 trackMax =
      ImVec2(trackX + trackWidth, faderAreaPos.y + faderAreaSize.y);
  draw_list->AddRectFilled(trackMin, trackMax, trackColor, 5.0f);

  // Draw dB markings as short darker lines
  for (float mark = max_dB; mark >= min_dB; mark -= 6.0f) {
    float markNorm = (mark - min_dB) / (max_dB - min_dB);
    float markY =
        faderAreaPos.y + faderAreaSize.y - (markNorm * faderAreaSize.y);
    ImVec2 lineStart = ImVec2(trackX - markingOffset, markY);
    ImVec2 lineEnd = ImVec2(trackX-2, markY);
    draw_list->AddLine(lineStart, lineEnd, markingColor, 2.0f);
  }

  // Draw fader handle with gradient using AddRectFilledMultiColor
  ImVec2 faderMin = ImVec2(trackX - (faderWidth - trackWidth) * 0.5f, faderY);
  ImVec2 faderMax =
      ImVec2(trackX + trackWidth + (faderWidth - trackWidth) * 0.5f,
             faderY + faderHeight);
  draw_list->AddRectFilledMultiColor(faderMin, faderMax, faderColorTop,
                                     faderColorTop, faderColorBottom,
                                     faderColorBottom);

  // Draw center horizontal white line on fader
  float centerY = (faderMin.y + faderMax.y) * 0.5f;
  draw_list->AddLine(ImVec2(faderMin.x, centerY), ImVec2(faderMax.x, centerY),
                     IM_COL32(255, 255, 255, 50), 2.0f);

  // Handle dragging
  ImGui::SetCursorScreenPos(faderMin);
  ImGui::InvisibleButton("##fader",
                         ImVec2(faderMax.x - faderMin.x, faderHeight));

  if (ImGui::IsItemActive()) {
    isActive = true;
    float mouseY = ImGui::GetMousePos().y;
    float newNorm = 1.0f - ((mouseY - faderAreaPos.y) / faderAreaSize.y);
    *dB = min_dB + newNorm * (max_dB - min_dB);
    *dB = std::clamp(*dB, min_dB, max_dB);
  }

  // Display peak level in a disabled text box at the top
  ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y));
  ImGui::SetNextItemWidth(size.x);
  char peakLabel[25];
  if (peakLevel <= min_dB)
    snprintf(peakLabel, sizeof(peakLabel), "-∞");
  else
    snprintf(peakLabel, sizeof(peakLabel), "%.1fdB", peakLevel);

  ImGui::BeginDisabled();
  ImGui::InputText("##peakLevel", peakLabel, sizeof(peakLabel),
                   ImGuiInputTextFlags_ReadOnly);
  ImGui::EndDisabled();

  // Display editable text field for dB value at the bottom
  ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + size.y - textFieldHeight));
  ImGui::SetNextItemWidth(size.x);
  char dBLabel[25];
  if (*dB <= min_dB)
    snprintf(dBLabel, sizeof(dBLabel), "-∞");
  else
    snprintf(dBLabel, sizeof(dBLabel), "%.1f dB", *dB);

  if (ImGui::InputText("##dBInput", dBLabel, sizeof(dBLabel),
                       ImGuiInputTextFlags_EnterReturnsTrue)) {
    float newVal;
    if (sscanf(dBLabel, "%f", &newVal) == 1) {
      *dB = std::clamp(newVal, min_dB, max_dB);
    }
  }

    StereoMeter(draw_list, ImVec2(pos.x + 40, pos.y), ImVec2(size.x/3, size.y), peak_left, peak_right, rms_left, rms_right);

    ImGui::PopID();

  return isActive; // Returns true if the fader is being dragged
}

#endif // !UI_WIDGETS_FADER_H