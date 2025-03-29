#pragma once
#ifndef DAW_H
#define DAW_H

#include "core/AudioEngine.h"
#include "core/MidiEngine.h"
#include "core/MidiRouter.h"
#include "core/MidiScriptHander.h"
#include "core/Playhead.h"
#include "components/AudioSampler.h"
#include "components/Metronome.h"
#include <memory>

using std::make_shared;
using std::shared_ptr;

class Daw
{
public:
    shared_ptr<AudioEngine> audio_engine;
    shared_ptr<MidiEngine> midi_engine;
    shared_ptr<MidiRouter> midi_router;
    shared_ptr<MidiScriptHandler> midi_script_handler;
    shared_ptr<Playhead> playhead;
    shared_ptr<Metronome> metronome;
    shared_ptr<AudioSampler> audio_sampler;
    Daw();
    ~Daw();

    void quit();

};
#endif // DAW_H