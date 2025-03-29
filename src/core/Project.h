#pragma once
#include <cstddef>
#ifndef PROJECT_H
#define PROJECT_H

// This is a project. 
// It represents an entire project in the DAW.
// It has a max of 32 tracks, and a master track
// Each track is an AudioNode and MidiNode, and will be connected to the master track
// The project will hold tempo, time signature, and other project wide settings.
// The project will also need to be serialized and deserialized to and from a file.
// The master track will be connected to the audio engine.
// Each track needs to be connected to the MidiRouter. This is how the tracks will get midi messages.

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "yaml-cpp/yaml.h"
#include "library/Parameter.h"
#include "library/Observer.h"
#include "core/Daw.h"
#include "core/TrackNode.h"
#include "library/utility.h"

using std::shared_ptr;
using std::make_shared;
using std::string;  
using std::vector;


class Project : public ISubject, public IObserver {
    public:
        Project(std::shared_ptr<Daw> daw);
        ~Project();
        StringParameter name = StringParameter("Name", "Untitled");
        FloatParameter tempo = FloatParameter("Tempo", 120, 30, 500, 1);
        IntParameter time_signature_numerator = IntParameter("Time Signature Numerator", 4, 1, 32, 1);
        IntParameter time_signature_denominator = IntParameter("Time Signature Denominator", 4, 1, 32, 1);
        BoolParameter metronome_enabled = BoolParameter("Metronome Enabled", true);
        vector<shared_ptr<TrackNode>> tracks;
        vector<shared_ptr<TrackNode>> busses;
        shared_ptr<TrackNode> master_track;
        void encode(YAML::Emitter &out);
        void decode(YAML::Node &node);
        std::shared_ptr<Daw> daw;
        void add_track();
        void remove_track(shared_ptr<TrackNode> track);
        void remove_track(size_t index);
        void move_track(unsigned int from, unsigned int to);
        void select_track(size_t index);
        void select_track(shared_ptr<TrackNode> track);
        shared_ptr<TrackNode> get_selected_track();
        int get_track_index(shared_ptr<TrackNode> track);
        int get_selected_track_index();

    private:
        int _selected_track = -1;
        void setup_parameters();
        const int _MAX_TRACKS = 32;
        const int _MAX_BUSSES = 8;
};

#endif // !PROJECT_H