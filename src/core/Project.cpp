#include "Project.h"
#include "Daw.h"
#include "TrackNode.h"
#include "yaml-cpp/emittermanip.h"
#include <cstddef>
#include <memory>


Project::Project(std::shared_ptr<Daw> daw){
    this->daw = daw;
    setup_parameters();
    master_track = make_shared<TrackNode>();
    daw->audio_engine->add_input_node(master_track);
    tracks.reserve(_MAX_TRACKS);
    busses.reserve(_MAX_BUSSES);

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

void Project::add_track(){
    if(tracks.size() < _MAX_TRACKS){

        
        // TODO: may need a track creation factory here.
        // Create a new track
        auto track_name = "Track " + std::to_string(tracks.size() + 1);
        auto new_track = make_shared<TrackNode>(track_name);
        tracks.emplace_back(new_track);
        new_track->set_is_source(true);

        daw->midi_router->add_node(new_track);
        new_track->set_midi_router(daw->midi_router);

        master_track->add_input_node(new_track);
        auto new_index = tracks.size() - 1;
        select_track(new_index);
        std::cout << "Track added\n";
    } else {
        std::cout << "Max tracks reached\n";
    }
}

void Project::remove_track(shared_ptr<TrackNode> track){


    auto track_index = get_track_index(track);
    remove_track(track_index);
}

void Project::remove_track(size_t index){
    if(index < tracks.size()){

        // Only remove track if it is not the last remaining track

        if(tracks.size() == 1){
            std::cout << "Cannot remove last track\n";
            return;
        }
        
        if(index < 0){
            std::cout << "Invalid index\n";
            return;
        }

        auto track = tracks[index];

        // If the track is the selected track, select the previous track
        if(_selected_track == index){
            if(index > 0){
                _selected_track = index - 1;
            } else {
                _selected_track = 0;
            }
        }
        daw->midi_router->remove_node(track);
        master_track->remove_input_node(track);
        tracks.erase(tracks.begin() + index);
        std::cout << "Track removed\n";
    } else {
        std::cout << "Track not found\n";
    }
}

void Project::move_track(unsigned int from, unsigned int to){

    moveElement<shared_ptr<TrackNode>>(tracks, from, to);
    
}

void Project::select_track(size_t index){
    if (index >=0 && index < tracks.size()){
        _selected_track = index;
    }
    
}

int Project::get_track_index(shared_ptr<TrackNode> track){
    for(size_t i = 0; i < tracks.size(); i++){
        auto t = tracks[i];
        if(t == track){
            return i;
        }
    }
    return -1;
}

void Project::select_track(shared_ptr<TrackNode> track){
    int track_index = get_track_index(track);
    if(track_index != -1){
        _selected_track = track_index;
    }
}

shared_ptr<TrackNode> Project::get_selected_track(){
    if(_selected_track != -1){
        return tracks[_selected_track];

    }else{
        return tracks[0];
    }
}

int Project::get_selected_track_index(){
    return _selected_track;
}


