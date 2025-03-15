#include "MidiEngine.h"



    MidiEngine::MidiEngine()
    {
        this->midiIn = std::make_shared<RtMidiIn>(RtMidiIn());
        this->midiOut = std::make_shared<RtMidiOut>(RtMidiOut());
        refreshDevices();
        std::cout << "MidiEngine created\n";
    }
    MidiEngine::~MidiEngine()
    {
        std::cout << "MidiEngine destroyed\n";
    }
    void MidiEngine::refreshDevices()
    {
        for (unsigned int i = 0; i < midiIn->getPortCount(); i++)
        {
            auto device = midiIn->getPortName(i);
            inputDevices[device] = make_shared<MidiInPort>(device, (int)i, std::make_shared<RtMidiIn>(), auto_enable);
            inputCallbackMap[device] = MidiCallbackData{this, &(*inputDevices[device])};
            inputDevices[device]->port->setCallback(&midiCallback, &(inputCallbackMap[device]));
            // inputDevices[device]->port->openPort(i);
        }
        for (unsigned int i = 0; i < midiOut->getPortCount(); i++)
        {
            auto device = midiOut->getPortName(i);
            outputDevices[device] = make_shared<MidiOutPort>(device, (int)i, std::make_shared<RtMidiOut>(), auto_enable);
            // outputDevices[device]->port->openPort(i);
        }
    };
    void MidiEngine::activate()
    {
        for (auto &[name, midiInPort] : inputDevices)
        {
            if (midiInPort->enabled && !midiInPort->port->isPortOpen())
            {
                midiInPort->port->openPort(midiInPort->index);
            }
        }
        for (auto &[name, midiOutPort] : outputDevices)
        {
            if (midiOutPort->enabled && !midiOutPort->port->isPortOpen())
            {
                midiOutPort->port->openPort(midiOutPort->index);
            }
        }
    }
    void MidiEngine::deactivate()
    {
        for (auto &[name, midiInPort] : inputDevices)
        {
            if (midiInPort->port->isPortOpen())
            {
                midiInPort->port->closePort();
            }
        }
        for (auto &[name, midiOutPort] : outputDevices)
        {
            if (midiOutPort->port->isPortOpen())
            {
                midiOutPort->port->closePort();
            }
        }
    }
    void MidiEngine::midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData)
    {
        // Get the user data pointer.
        MidiCallbackData *callbackData = (MidiCallbackData *)userData;
        MidiMsg msg{(*message)[0], (*message)[1], (*message)[2], callbackData->port->device_name, callbackData->port->index};

        // // Send the message to the midi engine for processing through process chain
        auto midi_engine = std::any_cast<MidiEngine *>(callbackData->engine);
        midi_engine->process(msg);
    }
    void MidiEngine::process(MidiMsg msg)
    {
        if (midiCallbackFunction)
        {
            auto device_name = msg.device.value_or("");
            auto midiInPort = inputDevices.find(device_name);
            if(midiInPort != inputDevices.end()){
                if(midiInPort->second->enabled){
                    midiCallbackFunction(msg);
                }
            }
        }
    }

    void MidiEngine::addMidiNode(shared_ptr<MidiNode> node)
    {
        midi_nodes.push_back(node);
    }
    void MidiEngine::removeMidiNode(shared_ptr<MidiNode> node)
    {
        midi_nodes.erase(std::remove(midi_nodes.begin(), midi_nodes.end(), node), midi_nodes.end());
    }