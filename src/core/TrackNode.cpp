#include "core/TrackNode.h"

TrackNode::TrackNode() {
    this->name.set_value("Track");
}
TrackNode::~TrackNode() {}

void TrackNode::setup_nodes() {
    this->add_node_to_chain(panNode);
    this->add_node_to_chain(volume);
    this->add_node_to_chain(levelMeter);
    this->add_node_to_chain(muteNode);
}

void TrackNode::setup_parameters() {
    volume_db.on_change = [this](float value) {
        volume->set_gain_db(value);
    };
    volume_db.update();
    attach_parameter(&volume_db);

    pan.on_change = [this](float value) {
        panNode->set_pan(value);
    };
    pan.update();
    attach_parameter(&pan);

    mute.on_change = [this](bool value) {
        if(value) {
            muteNode->set_gain(0.0);
        } else {
            muteNode->set_gain(1.0);
        }
    };
    mute.update();
    attach_parameter(&mute);

    // TODO: Implement solo
    solo.on_change = [this](bool value) {
        if(trackManager) {
            trackManager->set_track_solo();
        } else {
            trackManager->set_track_solo();
        }
    };
    solo.update();

    midi_input_device.on_change = [this](string value) {
        this->set_input_device_filter(value);
    };
    midi_input_device.update();
    attach_parameter(&midi_input_device);

    midi_input_channel.on_change = [this](int value) {
        this->set_input_channel_filter(value);
    };
    midi_input_channel.update();
    attach_parameter(&midi_input_channel);


}

void TrackNode::set_midi_router(shared_ptr<MidiRouter> midiRouter) {
    this->midiRouter = midiRouter;
}

void TrackNode::_process_midi(MidiMsg msg) {

    // Check if the midi message matches the input device and channel filters

    // Send the midi message to the instrument device if the track is armed
    if(arm.get_value() && instrument_device) {
        // instrument_device->on_midi_in_event(msg);
    }
   
}
