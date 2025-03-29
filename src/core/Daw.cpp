#include "Daw.h"
#include "MidiMsg.h"
#include <iostream>
#include <string>

static void log(std::string message)
{
    std::cout << message << std::endl;
};

Daw::Daw()
    {
        std::cout << "Setting up audio and midi engines\n";

        //  Setup Audio Engine
        audio_engine = make_shared<AudioEngine>();
        audio_engine->set_sample_rate(44100);
        audio_engine->set_buffer_size(32);
        audio_engine->name = "Audio Engine";

        // Setup Midi Engine
        midi_engine = make_shared<MidiEngine>();
        midi_router = make_shared<MidiRouter>(midi_engine);
        midi_script_handler = make_shared<MidiScriptHandler>();
        midi_script_handler->setNextHandler(midi_router);

        // Set Midi Engine Callback
        midi_engine->midiCallbackFunction = [&](MidiMsg msg){
            midi_script_handler->handle(msg);
        };

        // Setup Playhead and Metronome
        playhead = make_shared<Playhead>(audio_engine->get_sample_rate());
        audio_engine->set_playhead(playhead);
        playhead->bpm.set_value(120);

        metronome = make_shared<Metronome>();
        metronome->set_enabled(true);

        playhead->add_event_lister(
            std::bind(&Metronome::on_playhead_event, metronome.get(), std::placeholders::_1)
        );

        playhead->attach(Playhead::Events::STATE_CHANGE, playhead, metronome);
        midi_engine->activate();

        // Audio Sampler
        // audio_sampler = make_shared<AudioSampler>("/Users/bcrowe/Documents/Audio Samples/BVKER - Elevate Beamaker Kit/Tonal Shots/BVKER - Artifacts Keys 09 - C.wav");
        // midi_router->add_node(audio_sampler);
        // audio_engine->add_input_node(audio_sampler);


        // Connect nodes to engines
        audio_engine->add_input_node(metronome->metronome_sound);

        // Start engine
        audio_engine->start();

    }
Daw::~Daw()
{
    cout << "Daw destructor\n";
    cout << "Shutting down audio and midi engines\n";
    audio_engine->stop();
    midi_engine->deactivate();
}

