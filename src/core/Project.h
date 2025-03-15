#pragma once
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


class Project : public ISubject, public IObserver {
    public:
        Project(std::shared_ptr<Daw> daw);
        ~Project();
        StringParameter name = StringParameter("Name", "Untitled");
        FloatParameter tempo = FloatParameter("Tempo", 120, 30, 500, 1);
        IntParameter time_signature_numerator = IntParameter("Time Signature Numerator", 4, 1, 32, 1);
        IntParameter time_signature_denominator = IntParameter("Time Signature Denominator", 4, 1, 32, 1);
        BoolParameter metronome_enabled = BoolParameter("Metronome Enabled", true);
        void encode(YAML::Emitter &out);
        void decode(YAML::Node &node);
        std::shared_ptr<Daw> daw;

    private:
        void setup_parameters();
        

};

#endif // !PROJECT_H