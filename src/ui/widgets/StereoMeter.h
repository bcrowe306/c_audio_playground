#ifndef STEREOMETER_H
#define STEREOMETER_H

#include "imgui.h"

using namespace ImGui;

static void StereoMeter(
    ImDrawList *draw_list,
    ImVec2 position,
    ImVec2 size,
    float peakLeftValue,
    float peakRightValue,
    float rmsLeftValue,
    float rmsRightValue,
    float peakMin = -60.f,
    float peakMax = 0.f,
    float rmsMin = -60.f,
    float rmsMax = 0.f)
{

    // Define measurements;
    float meterSpacing = 2.f;
    float meterWidth = (size.x - meterSpacing) / 2.f;
    auto meterLeftStart_x = position.x;
    auto meterLeftEnd_x = position.x + meterWidth;
    auto meterRightStart_x = meterLeftEnd_x + meterSpacing;
    auto meterRightEnd_x = meterRightStart_x + meterWidth;
    auto meterStart_y = position.y;
    auto meterSize_y = size.y;
    auto meterEnd_y = meterStart_y + meterSize_y;

    // Calculate rms and peak
    auto peakL = (1 - peakLeftValue / (peakMin - peakMax)) * meterSize_y;
    auto peakR = (1 - peakRightValue / (peakMin - peakMax)) * meterSize_y;
    auto rmsL = (1 - rmsLeftValue / (rmsMin - rmsMax)) * meterSize_y;
    auto rmsR = (1 - rmsRightValue / (rmsMin - rmsMax)) * meterSize_y;

    //  Define Colors
    auto dark_gray = IM_COL32(10, 10, 10, 255);
    auto rdMtrClr = IM_COL32(255, 0, 0, 255);
    auto tpMtrClr = IM_COL32(255, 255, 0, 255);
    // Light Green
    auto midMtrClr = IM_COL32(0, 255, 0, 255);
    // Dark Green
    auto btmMrtClr = IM_COL32(0, 128, 0, 255);

    //  Draw Background Left
    draw_list->AddRectFilled(ImVec2(meterLeftStart_x, meterStart_y), ImVec2(meterLeftEnd_x + meterWidth, meterEnd_y), dark_gray);

    //  Draw Background Right
    draw_list->AddRectFilled(ImVec2(meterRightStart_x, meterStart_y), ImVec2(meterRightEnd_x, meterEnd_y), dark_gray);

    // Left Meter bars
    draw_list->PushClipRect(ImVec2(meterLeftStart_x, meterEnd_y - rmsL), ImVec2(meterLeftEnd_x, meterEnd_y));
    draw_list->AddRectFilledMultiColor(ImVec2(meterLeftStart_x, meterStart_y), ImVec2(meterLeftEnd_x, meterStart_y + (meterSize_y * .25f)), rdMtrClr, rdMtrClr, tpMtrClr, tpMtrClr);
    draw_list->AddRectFilledMultiColor(ImVec2(meterLeftStart_x, meterStart_y + (meterSize_y * .25f)), ImVec2(meterLeftEnd_x, meterStart_y + (meterSize_y * .5f)), tpMtrClr, tpMtrClr, midMtrClr, midMtrClr);
    draw_list->AddRectFilledMultiColor(ImVec2(meterLeftStart_x, meterStart_y + (meterSize_y * .5f)), ImVec2(meterLeftEnd_x, meterEnd_y), midMtrClr, midMtrClr, btmMrtClr, btmMrtClr);
    draw_list->PopClipRect();

    // Right Meter bars
    draw_list->PushClipRect(ImVec2(meterRightStart_x, meterEnd_y - rmsR), ImVec2(meterRightEnd_x, meterEnd_y));
    draw_list->AddRectFilledMultiColor(ImVec2(meterRightStart_x, meterStart_y), ImVec2(meterRightEnd_x, meterStart_y + (meterSize_y * .25f)), rdMtrClr, rdMtrClr, tpMtrClr, tpMtrClr);
    draw_list->AddRectFilledMultiColor(ImVec2(meterRightStart_x, meterStart_y + (meterSize_y * .25f)), ImVec2(meterRightEnd_x, meterStart_y + (meterSize_y * .5f)), tpMtrClr, tpMtrClr, midMtrClr, midMtrClr);
    draw_list->AddRectFilledMultiColor(ImVec2(meterRightStart_x, meterStart_y + (meterSize_y * .5f)), ImVec2(meterRightEnd_x, meterEnd_y), midMtrClr, midMtrClr, btmMrtClr, btmMrtClr);
    draw_list->PopClipRect();

    draw_list->PushClipRect(ImVec2(meterLeftStart_x, meterStart_y), ImVec2(meterRightEnd_x, meterEnd_y));
    draw_list->AddLine(ImVec2(meterLeftStart_x, meterEnd_y - peakL), ImVec2(meterLeftEnd_x, meterEnd_y - peakL), IM_COL32_WHITE);
    draw_list->AddLine(ImVec2(meterRightStart_x, meterEnd_y - peakR), ImVec2(meterRightEnd_x, meterEnd_y - peakR), IM_COL32_WHITE);
    draw_list->PopClipRect();
}

#endif // !STEREOMETER_H