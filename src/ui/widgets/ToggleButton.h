#pragma once
#ifndef UI_WIDGETS_TOGGLEBUTTON_H
#define UI_WIDGETS_TOGGLEBUTTON_H

#include "imgui.h"

bool ToggleButton(const char *label, bool *value, ImVec2 pos, ImVec2 size = ImVec2(40, 20),
                  ImU32 onColor = IM_COL32(200, 0, 0, 255),
                  ImU32 offColor = IM_COL32(50, 50, 50, 255),
                  float rounding = 5.0f) // Default rounded corners
{
  ImGui::SetCursorScreenPos(pos);

  ImGui::PushID(label);

  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  ImVec2 min = pos;
  ImVec2 max = ImVec2(pos.x + size.x, pos.y + size.y);

  ImU32 fillColor = *value ? onColor : offColor;
  ImU32 textColor = *value ? offColor : onColor; // Opposite of fill color

  draw_list->AddRectFilled(min, max, fillColor, rounding);

  if (ImGui::InvisibleButton(label, size)) {
    *value = !(*value);
  }

  // Draw label centered in the button
  ImVec2 textSize = ImGui::CalcTextSize(label);
  ImVec2 textPos = ImVec2(min.x + (size.x - textSize.x) * 0.5f,
                          min.y + (size.y - textSize.y) * 0.5f);

  draw_list->AddText(textPos, textColor, label);

  ImGui::PopID();

  return *value;
}
#endif // !UI_WIDGETS_TOGGLEBUTTON_H