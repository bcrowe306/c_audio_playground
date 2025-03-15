#pragma once
#include <pybind11/pybind11.h>
#include <sol/load_result.hpp>
#include <sol/state.hpp>
#ifndef MIDI_SCRIPT_HANDLER_H
#define MIDI_SCRIPT_HANDLER_H

#include "MidiHandler.h"
#include "MidiMsg.h"
#include "MidiNode.h"
#include "pybind11/embed.h"
#include "sol/sol.hpp"
#include "unordered_map"
#include "util/path_utils.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <pybind11/gil.h>
#include <pybind11/pybind11.h>
#include <string>
#include <vector>

// TODO: This class will be used to handle midi messages in the script engine

using std::shared_ptr;
using std::make_shared;
using std::vector;
using std::string;
using std::unordered_map;

namespace py = pybind11;

enum class ControlScriptType {
    Python,
    Lua,
 };
struct ControlScript {
    ControlScript(string name, shared_ptr<py::module> python_module) : name(name), python_module(python_module) {
        type = ControlScriptType::Python;

    }
    ControlScript(string name, sol::bytecode lua_bytecode) : name(name), lua_bytecode(lua_bytecode) {
      type = ControlScriptType::Lua;
    }
    string name;
    shared_ptr<py::module> python_module = nullptr;
    sol::bytecode lua_bytecode;
    ControlScriptType type;
    string input_device = "";
    string output_device = "";
    bool enabled = false;
    void set_input_device(string device_name){
        input_device = device_name;
    }
    void set_output_device(string device_name){
        output_device = device_name;
    }
    void set_enabled(bool value){
        enabled = value;
    }
    
};

class MidiScriptHandler : public MidiHandler
{
    public:
        void refresh_scripts();
        void load_script(const std::string &script_path);
        void load_all_scripts();
        sol::state lua;
        vector<ControlScript> control_scripts;
        MidiScriptHandler();
        ~MidiScriptHandler();


        // Process the midi message. This function is passed into the script engine to be called when a midi message is received.
        // To stop the message from being passed to the next handler set event.isHandled = true.
        // Setting the event.isHandled = true will stop the message from being passed to the tracks to be played and recorded.
        // @param event - The midi message to process
        
        void process(MidiMsg &event) override;
    protected:
        unordered_map<string, shared_ptr<py::module>> loaded_scripts;
};

#endif // !MIDI_SCRIPT_HANDLER_H