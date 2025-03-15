#pragma once
#include "RtMidi.h"
#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <optional>
#include <any>
#include <unordered_map>
#include <functional>
#include <thread>
#include "MidiMsg.h"
#include "MidiPorts.h"
#include "MidiNode.h"
#include "MidiHandler.h"

struct MidiCallbackData
{
    std::any engine;
    MidiInPort *port;
};

class MidiEngine
{
public:
    std::shared_ptr<RtMidiIn> midiIn;
    std::shared_ptr<RtMidiOut> midiOut;
    std::unordered_map<std::string, std::shared_ptr<MidiInPort>> inputDevices;
    std::unordered_map<std::string, std::shared_ptr<MidiOutPort>> outputDevices;
    std::unordered_map<std::string, MidiCallbackData> inputCallbackMap;
    vector<shared_ptr<MidiNode>> midi_nodes;
    bool auto_enable = true;
    MidiEngine();
    ~MidiEngine();

    void addMidiNode(shared_ptr<MidiNode> node);
    void removeMidiNode(shared_ptr<MidiNode> node);
    void refreshDevices();
    void activate();
    void deactivate();
    static void midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData);
    std::function<void(MidiMsg &)> midiCallbackFunction;
    void process(MidiMsg msg);
};