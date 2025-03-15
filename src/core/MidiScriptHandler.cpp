#include "MidiMsg.h"
#include "core/MidiScriptHander.h"
#include <sol/forward.hpp>
#include <sol/state_handling.hpp>



MidiScriptHandler::MidiScriptHandler()
{
    std::cout << "MidiScriptHandler created\n";
    // py::initialize_interpreter();
    // guard = new py::scoped_interpreter();
    refresh_scripts();
    // load_all_scripts();
    lua.open_libraries(sol::lib::base);
    MidiMsg::generateLuaType(lua);
}
MidiScriptHandler::~MidiScriptHandler()
{
    std::cout << "MidiScriptHandler destroyed\n";
    // delete guard;
}

void MidiScriptHandler::process(MidiMsg &event)
{   
    for(auto &script : control_scripts){
        if(script.type == ControlScriptType::Python){
            auto func = script.python_module->attr("process");
            func(event);
        }
        else if(script.type == ControlScriptType::Lua){
            // sol::state lua;
            
            // std::cout << "Running lua script: " << script.lua_bytecode.as_string_view() << std::endl;
            auto result2 = lua.safe_script(script.lua_bytecode.as_string_view(),
                                            sol::script_throw_on_error);
            sol::protected_function pf = lua["onMidiInEvent"];
            auto result = pf(event);
        }
    }
}

void MidiScriptHandler::refresh_scripts(){
    // TODO: Clean up and decide script implementation
    // py::gil_scoped_acquire acquire;
    // // Search the /control_scripts directory for python modules. Each folder is a module.

    // sol::state lua;
    auto application_path = PathUtils::getApplicationPath();

    // // Append the control_scripts directory to the application path
    std::filesystem::path control_script_python_path = application_path + "/Contents/Resources/control_scripts/python";
    std::filesystem::path control_script_lua_path = application_path + "/Contents/Resources/control_scripts/lua";


    // std::cout << "Control scripts path: " << control_scripts_path.c_str() << std::endl;
    // py::module_ sys = py::module_::import("sys");
    // py::list path = sys.attr("path");
    // path.insert(0, control_scripts_path.string());

    // for (auto &p : std::filesystem::directory_iterator(control_scripts_path)) {

    //     auto control_script_module = p.path().filename().string();
    //     std::cout << "Found control script: " << control_script_module << std::endl;
    //     auto mod = make_shared<py::module>(py::module::import(control_script_module.c_str()));
    //     ControlScript script{control_script_module, mod};
    //     control_scripts.emplace_back(script);
    // }

    //
    // Search the /control_scripts directory for lua files. load each .lua file extention
    for (auto &p : std::filesystem::directory_iterator(control_script_lua_path)) {

        if(p.path().extension() != ".lua"){
            continue;
        }
        auto control_script_module = p.path().string();
        auto control_script_module_name = p.path().filename().string();
        std::cout << "Found control script: " << control_script_module << std::endl;
        auto mod = lua.load_file(control_script_module);
        auto lua_bytecode = mod.get<sol::protected_function>().dump();
        ControlScript script{control_script_module_name, lua_bytecode};
        control_scripts.push_back(std::move(script));
    }

    
}

void MidiScriptHandler::load_script(const std::string &script_path){
    // Load the script into the interpreter
    auto script = py::module::import(script_path.c_str());
    loaded_scripts[script_path] = std::make_shared<py::module>(script);
    
}

void MidiScriptHandler::load_all_scripts(){
    // for (const auto & script : control_scripts){
    //     // load_script(script);
    // }
}