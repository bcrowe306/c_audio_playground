#pragma once

#include "MidiMsg.h"
#ifndef TRACKNODE_H
#define TRACKNODE_H

#include "core/AudioNode.h"
#include "core/MidiNode.h"
#include "library/Parameter.h"
#include "library/Observer.h"
#include "nodes/GainNode.h"
#include "nodes/PanNode.h" 
#include "nodes/LevelMeterNode.h"
#include "util/uuid_generator.h"
#include <string>
#include <vector>
#include <memory>
#include "core/MidiRouter.h"

using std::make_shared;
using std::shared_ptr;
using std::vector;
using std::string;

// Dummy class
// TODO Implement a track manager to manage:
// - Track order
// - Track solo
// - Track mute
// - Track arm
// Track number limits

class TrackManager{
    public:
        void add_track(){}
        void remove_track(){}
        void move_track(){}
        void set_track_solo(){}
};

class TrackNode : public AudioNode, public MidiNode, public IObserver, public ISubject
{
public:
    TrackNode();
    ~TrackNode();
    StringParameter name = StringParameter("Name", "Track");
    StringParameter id = StringParameter("ID", generateUUID());
    StringParameter midi_input_device = StringParameter("MIDI Input Device", "All");
    StringParameter midi_output_device = StringParameter("MIDI Output Device", "None");
    IntParameter midi_input_channel = IntParameter("MIDI Input Channel", -1, -1, 16, 1);
    IntParameter midi_output_channel = IntParameter("MIDI Output Channel", 0, -1, 16, 1);
    FloatParameter volume_db = FloatParameter("Volume (dB)", 0.0, -60.0, 12.0, 0.1);
    FloatParameter pan = FloatParameter("Pan", 0.0, -1.0, 1.0, 0.01);
    BoolParameter mute = BoolParameter("Mute", false);
    BoolParameter solo = BoolParameter("Solo", false);
    BoolParameter arm = BoolParameter("Arm", false);

    void set_midi_router(shared_ptr<MidiRouter> midiRouter);

protected:
    
    shared_ptr<AudioNode> instrument_device;
    shared_ptr<MidiRouter> midiRouter;;
    shared_ptr<GainNode> volume = make_shared<GainNode>();
    shared_ptr<PanNode> panNode = make_shared<PanNode>();
    shared_ptr<LevelMeterNode> levelMeter = make_shared<LevelMeterNode>();
    shared_ptr<GainNode> muteNode = make_shared<GainNode>();
    shared_ptr<TrackManager> trackManager;
    void setup_nodes();
    void setup_parameters();
    void _process_midi(MidiMsg msg);
};

#endif // !TRACKNODE_H