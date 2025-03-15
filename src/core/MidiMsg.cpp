#include "MidiMsg.h"
#include <string>

using namespace std;
using choc::midi::ShortMessage;

MidiMsg::MidiMsg(const void *sourceData, size_t numBytes, optional<string> device, optional<int> index)
    : ShortMessage(sourceData, numBytes),
        device(device), index(index){};

MidiMsg::~MidiMsg()
{}

string MidiMsg::getDeviceName() const
{
    return device.value_or("");
}

int MidiMsg::getDeviceIndex() const
{
    return index.value_or(-1);
}


PortableMidiMessage MidiMsg::toPortableMidiMessage(){
    PortableMidiMessage msg;
    auto msg_data = data();
    msg.status = msg_data[0];
    msg.data1 = msg_data[1];
    msg.data2 = msg_data[2];
    msg.channel = getChannel0to15();
    msg.isNoteOn = isNoteOn();
    msg.isNoteOff = isNoteOff();
    msg.isControlChange = isController();
    msg.isPitchBend = isPitchWheel();
    msg.isProgramChange = isProgramChange();
    msg.isAftertouch = isAftertouch();
    msg.isChannelPressure = isChannelPressure();
    msg.isHandled = isHandled;
    return msg;

}


void MidiMsg::generateLuaType(sol::state &lua){
    auto MidiMsgLua = lua.new_usertype<MidiMsg>("MidiMsg",
        sol::constructors<MidiMsg(), MidiMsg(const void*, size_t, optional<string>, optional<int>)>(),
        "getDeviceName", &MidiMsg::getDeviceName,
        "getDeviceIndex", &MidiMsg::getDeviceIndex,
        "getUnitVelocity", &MidiMsg::getUnitVelocity,
        "toPortableMidiMessage", &MidiMsg::toPortableMidiMessage
    );
    MidiMsgLua["isHandled"] = &MidiMsg::isHandled;
    MidiMsgLua["device"] = &MidiMsg::device;
    MidiMsgLua["index"] = &MidiMsg::index;
    MidiMsgLua["getChannel0to15"] = &MidiMsg::getChannel0to15;
    MidiMsgLua["isNoteOn"] = &MidiMsg::isNoteOn;
    MidiMsgLua["isNoteOff"] = &MidiMsg::isNoteOff;
    MidiMsgLua["isController"] = &MidiMsg::isController;
    MidiMsgLua["isPitchWheel"] = &MidiMsg::isPitchWheel;
    MidiMsgLua["isProgramChange"] = &MidiMsg::isProgramChange;
    MidiMsgLua["isAftertouch"] = &MidiMsg::isAftertouch;
    MidiMsgLua["isChannelPressure"] = &MidiMsg::isChannelPressure;
    MidiMsgLua["data"] = &MidiMsg::data;
    MidiMsgLua["getVelocity"] = &MidiMsg::getVelocity;
    MidiMsgLua["getNoteNumber"] = &MidiMsg::getNoteNumber;
    MidiMsgLua["getControllerNumber"] = &MidiMsg::getControllerNumber;
    MidiMsgLua["getPitchWheelValue"] = &MidiMsg::getPitchWheelValue;
    MidiMsgLua["getChannelPressureValue"] = &MidiMsg::getChannelPressureValue;
    MidiMsgLua["toHexString"] = &MidiMsg::toHexString;
}



