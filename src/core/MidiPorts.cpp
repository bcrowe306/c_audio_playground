#include "MidiPorts.h"

MidiInPort::MidiInPort(std::string device_name, int index, std::shared_ptr<RtMidiIn> port, bool enabled)
    {
        this->device_name = device_name;
        this->index = index;
        this->port = port;
        this->enabled = enabled;
    };
MidiInPort::~MidiInPort()
    {
        if (port->isPortOpen())
        {
            port->closePort();
        }
    }

// MidiOutPort
MidiOutPort::MidiOutPort(std::string device_name, int index, std::shared_ptr<RtMidiOut> port, bool enabled)
    {
        // MidiNode specific details
        this->name = device_name;
        this->_is_source = false;
        this->_is_destination = true;

        // MidiOutPort specific details
        this->device_name = device_name;
        this->index = index;
        this->port = port;
        this->enabled = enabled;
    };

MidiOutPort::~MidiOutPort()
    {
        if (port->isPortOpen())
        {
            port->closePort();
        }
    }

void MidiOutPort::sendMessage(unsigned char status, unsigned char data1, unsigned char data2)
    {
        std::vector<unsigned char> midiMessage;
        midiMessage.push_back(status);
        midiMessage.push_back(data1);
        midiMessage.push_back(data2);
        port->sendMessage(&midiMessage);
    }

void MidiOutPort::sendMessage(std::vector<unsigned char> *message)
    {
        port->sendMessage(message);
    }

void MidiOutPort::sendSysexMessage(std::vector<unsigned char> *message)
    {
        // wrap the message with sysex start and end bytes
        std::vector<unsigned char> sysexMessage;
        sysexMessage.push_back(0xF0);
        for (auto &byte : *message)
        {
            sysexMessage.push_back(byte);
        }
        sysexMessage.push_back(0xF7);
        port->sendMessage(&sysexMessage);
    }

void MidiOutPort::on_midi_in_event(MidiMsg &msg)
    {
        if(port->isPortOpen()){
            std::vector<unsigned char> midiMessage;
            auto message_data = msg.data();
            for(int i = 0; i < msg.size(); i++){
                midiMessage.push_back(message_data[i]);
            }

            port->sendMessage(&midiMessage);
        }
    }
