#include <iostream>
#include <memory>
#include <pybind11/eval.h>
#include <pybind11/gil.h>
#include <string>
#include <cmath>

#include "hello_imgui/hello_imgui.h"
#include "ui/main_ui.h"
#include "ui/UIState.h"
#include "pybind11/embed.h"
#include "core/Daw.h"
#include "core/Application.h"

#include "sol/sol.hpp"

using std::shared_ptr;
using std::make_shared;
using std::string;

PYBIND11_EMBEDDED_MODULE(midi_module, m) {
  py::class_<PortableMidiMessage>(m, "PortableMidiMessage")
      .def(py::init<>())
      .def_readwrite("status", &PortableMidiMessage::status)
      .def_readwrite("data1", &PortableMidiMessage::data1)
      .def_readwrite("data2", &PortableMidiMessage::data2)
      .def_readwrite("channel", &PortableMidiMessage::channel)
      .def_readwrite("isNoteOn", &PortableMidiMessage::isNoteOn)
      .def_readwrite("isNoteOff", &PortableMidiMessage::isNoteOff)
      .def_readwrite("isControlChange", &PortableMidiMessage::isControlChange)
      .def_readwrite("isPitchBend", &PortableMidiMessage::isPitchBend)
      .def_readwrite("isProgramChange", &PortableMidiMessage::isProgramChange)
      .def_readwrite("isAftertouch", &PortableMidiMessage::isAftertouch)
      .def_readwrite("isChannelPressure",
                     &PortableMidiMessage::isChannelPressure)
      .def_readwrite("isHandled", &PortableMidiMessage::isHandled)
      .def("__repr__", [](const PortableMidiMessage &msg) {
            return "<MidiMsg status=" + std::to_string(msg.status) +
                   " data1=" + std::to_string(msg.data1) +
                   " data2=" + std::to_string(msg.data2) +
                   " channel=" + std::to_string(msg.channel) + ">";
        });
}
namespace py = pybind11;

void run_gui(std::shared_ptr<Application> App){
    HelloImGui::RunnerParams params;
    params.appWindowParams.windowGeometry.fullScreenMode = HelloImGui::FullScreenMode::FullMonitorWorkArea;
    params.fpsIdling.fpsIdle = 40.f;
    params.callbacks.SetupImGuiStyle = [&]()
    {
        params.imGuiWindowParams.tweakedTheme.Theme = ImGuiTheme::ImGuiTheme_PhotoshopStyle;
        params.imGuiWindowParams.tweakedTheme.Tweaks.Rounding = 0.0f;
        params.imGuiWindowParams.tweakedTheme.Tweaks.RoundingScrollbarRatio = 0.0f;
    };
    params.callbacks.LoadAdditionalFonts = []()
    {
        ui_state.fonts.font_16 =  HelloImGui::LoadFont("fonts/OpenSans.ttf", 16.f);
        ui_state.fonts.font_24 =  HelloImGui::LoadFont("fonts/OpenSans.ttf", 24.f);
    };
    params.callbacks.ShowGui = [&]()
    {
        main_ui(App);
    };
    HelloImGui::Run(params);
}

// TODO: Going with lua for now.
// TODO: Need to build a lua script interpreter class.
// This class will handle the excecution of the lua scripts.
// It will also handle the loading of the scripts.
// It will also handle the reloading of the scripts.
// It will also handle the building of the environment.
// TODO: Also need to generate usertypes for my classes.
// TODO: also need to test loading modules from lua scripts.
// TODO: need to reduce  dependencies on my top level classes. This will keep from running into cyclic dependencies.
// MidiEngine -> MidiHandler -> MidiScriptHandler -> AudioEngine -> MidiEngine -> Application
// TODO: Need a better logging system. Maybe a library.

int main(int, char**){
    // Main loop

    // py::scoped_interpreter guard{};
    // py::module_::import("midi_module");
    // py::gil_scoped_release release;
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.script("print('bark bark bark!')");
    auto daw = make_shared<Daw>();
    auto App = make_shared<Application>(daw);
    run_gui(App);
    // Get input
    // std::cin.get();
    return 0;
}
