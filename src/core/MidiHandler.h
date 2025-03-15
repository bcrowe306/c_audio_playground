#pragma once
#ifndef MIDI_HANDLER_H
#define MIDI_HANDLER_H

#include "core/MidiMsg.h"
#include <memory>

using std::shared_ptr;

class MidiHandler
{
protected:
    shared_ptr<MidiHandler> nextHandler;

public:
    MidiHandler();
    void setNextHandler(shared_ptr<MidiHandler> handler);
    void handle(MidiMsg &event);

    // Process the midi message, to stop the message from being passed to the next handler set event.isHandled = true
    // Note: This function should be overridden by the child class
    // @param event - The midi message to process
    virtual void process(MidiMsg &event) = 0;
};

#endif // !MIDI_HANDLER_H