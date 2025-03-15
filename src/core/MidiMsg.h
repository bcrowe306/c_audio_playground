#pragma once

#include <_types/_uint8_t.h>
#include <sol/state.hpp>
#include <string>
#include <optional>
#include "audio/choc_MIDI.h"
#include "sol/sol.hpp"

using namespace std;
using choc::midi::ShortMessage;

struct PortableMidiMessage
{
    int status;
    int data1;
    int data2;
    int channel;
    bool isNoteOn;
    bool isNoteOff;
    bool isControlChange;
    bool isPitchBend;
    bool isProgramChange;
    bool isAftertouch;
    bool isChannelPressure;
    bool isHandled = false;
};
struct MidiMsg : public ShortMessage
{
    bool isHandled = false;
    optional<string> device;
    optional<int> index;
    MidiMsg(){};

    MidiMsg(const void *sourceData, size_t numBytes, optional<string> device = nullopt, optional<int> index = nullopt);
    MidiMsg(uint8_t byte0, uint8_t byte1, uint8_t byte2, optional<string> device = nullopt, optional<int> index = nullopt)
        : ShortMessage(byte0, byte1, byte2), device(device), index(index){};
    ~MidiMsg();
    string getDeviceName() const;
    int getDeviceIndex() const;
    float getUnitVelocity(){
        return (int)(getVelocity()) / 128.f;
    }
    PortableMidiMessage toPortableMidiMessage();
    static void generateLuaType(sol::state &lua);
};
