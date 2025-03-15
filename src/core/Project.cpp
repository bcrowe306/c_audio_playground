#include "Project.h"
#include "Daw.h"
#include "yaml-cpp/emittermanip.h"
#include <memory>


Project::Project(std::shared_ptr<Daw> daw){
    this->daw = daw;
    setup_parameters();
    std::cout << "Project created\n";
}
Project::~Project(){
    std::cout << "Project destroyed\n";
}

void Project::encode(YAML::Emitter &out){
    out << YAML::BeginDoc;
    out << YAML::BeginMap;
    out << YAML::Key << "Project";
    out << YAML::Value << YAML::BeginMap;
    name.encode(out);
    tempo.encode(out);
    time_signature_numerator.encode(out);
    time_signature_denominator.encode(out);
    metronome_enabled.encode(out);
    out << YAML::EndMap;
    std::cout << out.c_str() << std::endl;
}

void Project::decode(YAML::Node &node){
    auto project = node["Project"];
    name.set_value(project["Name"].as<std::string>());
    tempo.set_value(project["Tempo"].as<float>());
    time_signature_numerator.set_value(project["Time Signature Numerator"].as<int>());
    time_signature_denominator.set_value(project["Time Signature Denominator"].as<int>());
    metronome_enabled.set_value(project["Metronome Enabled"].as<bool>());
}

void Project::setup_parameters(){
    name.on_change = [&](std::string value){
        // notify("Name", value);
    };
    name.update();

    tempo.on_change = [&](float value){
        daw->playhead->bpm.set_value(value);
    };
    tempo.update();

    time_signature_numerator.on_change = [&](int value){
        notify("Time Signature Numerator", value);
    };
    time_signature_numerator.update();

    time_signature_denominator.on_change = [&](int value){
        notify("Time Signature Denominator", value);
    };
    time_signature_denominator.update();


    metronome_enabled.on_change = [&](bool value){
        daw->metronome->set_enabled(value);
    };
    metronome_enabled.update();
}