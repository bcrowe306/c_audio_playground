#pragma once
#ifndef ICON_WIDGET_H
#define ICON_WIDGET_H

#include "hello_imgui/hello_imgui.h"
#include "imgui.h"

#include "imgui.h"

// Custom toggle button with centered icon
bool IconToggleButton(const char *id, bool *v, ImVec2 size, ImU32 bgColor,
                      ImU32 iconColorOn, ImU32 iconColorOff, const char *icon) {
  ImGui::PushID(id);

  // Determine icon color based on toggle state
  ImU32 iconColor = *v ? iconColorOn : iconColorOff;

  // Draw button with background color
  ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bgColor);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, bgColor);

  bool clicked = ImGui::Button("##toggle", size);

  ImGui::PopStyleColor(3);

  // Get button position and center icon
  ImDrawList *drawList = ImGui::GetWindowDrawList();
  ImVec2 pos = ImGui::GetItemRectMin();
  ImVec2 textSize = ImGui::CalcTextSize(icon);
  ImVec2 textPos =
      ImVec2(pos.x + (size.x - textSize.x) * 0.5f, // Center horizontally
             pos.y + (size.y - textSize.y) * 0.5f  // Center vertically
      );

  // Draw the icon
  drawList->AddText(nullptr, 0.0f, textPos, iconColor, icon);

  if (clicked)
    *v = !(*v); // Toggle the state

  ImGui::PopID();
  return clicked;
};

#endif // !ICON_WIDGET_H