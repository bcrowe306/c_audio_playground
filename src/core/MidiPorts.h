#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include "RtMidi.h"
#include "core/MidiNode.h"

class MidiInPort
{

public:
    MidiInPort(std::string device_name, int index, std::shared_ptr<RtMidiIn> port, bool enabled = false);
    ~MidiInPort();
    std::string device_name;
    std::shared_ptr<RtMidiIn> port;
    int index;
    bool enabled;
    bool sync = false;
    bool remote = false;
};
class MidiOutPort : public MidiNode
{

public:
    MidiOutPort(std::string device_name, int index, std::shared_ptr<RtMidiOut> port, bool enabled = false);
    ~MidiOutPort();
    std::string device_name;
    std::shared_ptr<RtMidiOut> port;
    void on_midi_in_event(MidiMsg &msg) override;
    int index;
    bool enabled;
    bool sync = false;

    // Send a midi message to the midi port
    // @param status - The status byte of the midi message
    // @param data1 - The first data byte of the midi message
    // @param data2 - The second data byte of the midi message
    void sendMessage(unsigned char status, unsigned char data1, unsigned char data2);

    // Send a midi message to the midi port
    // @param message - The midi message to send
    void sendMessage(std::vector<unsigned char> *message);

    // Send a sysex message to the midi port. The message should not include the start and end sysex bytes
    // @param message - The sysex message to send
    void sendSysexMessage(std::vector<unsigned char> *message);
};
