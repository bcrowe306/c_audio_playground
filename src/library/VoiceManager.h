#pragma once

#include <sys/_types/_u_int64_t.h>
#ifndef VOICEMANAGER_H
#define VOICEMANAGER_H
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include "core/MidiMsg.h"
#include "library/utility.h"
#include "library/ADSR.h"
#include "library/Observer.h"
#include "nodes/FilterNode.h"
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;


// TODO: Implement pitch envelope
// TODO: Implement all getter and setter methods
// TODO: clean up class and remove unused methods, variables, and comments. Also keep consistent naming conventions for public and private variables and methods

struct Voice;
class VoiceManager
{
public:
    enum class Modes {
        ONE_SHOT,
        ADSR,
    };
    VoiceManager(unsigned int polyphony, unsigned int sample_rate);
    void trigger_voice_start(MidiMsg &msg);
    void trigger_voice_stop(MidiMsg &msg);
    void trigger_voice_start_adsr(MidiMsg &msg);
    void trigger_voice_stop_adsr(MidiMsg &msg);
    void trigger_voice_start_one_shot(MidiMsg &msg);
    void trigger_voice_stop_one_shot(MidiMsg &msg);
    void increment_counter(Voice &v);
    vector<Voice*> *process();

    // Getters

    u_int get_polyphony();
    bool get_legato();
    int get_transpose_offset();
    float get_detune_offset();
    u_int get_base_note();
    bool get_loop();
    bool get_reverse();
    u_int get_starting_sample();
    u_int get_active_voice_count();
    VoiceManager::Modes get_mode();


    // Setters

    void set_legato(bool legato);

    // Set the polyphony
    void set_polyphony(u_int polyphony);
    
    // Set the transpose offset
    void set_transpose_offset(int offset);

    // Set the detune offset
    void set_detune_offset(float offset);

    // Set the base note
    void set_base_note(unsigned int note);

    // Set the loop
    void set_loop(bool loop);

    // Set the start sample buffer index
    void set_start(unsigned int start);

    // Set the end sample buffer index
    void set_end(unsigned int end);

    // Set the reverse. If true, the sample will play in reverse
    void set_reverse(bool reverse);

    // Set the sample rate
    void set_sample_rate(unsigned int sample_rate);

    // Set the unison amount of voices
    void set_unison_voices(u_int unison_voices);

    // Set the unison detune ratio from 0 to 1
    void set_unison_detune(float detune_ratio);

    // Set the unison detune maximum amount
    void set_unison_detune_amount_max(float detune_amount_max);

    // Set the unison pan amount
    void set_unison_pan_amount(float pan_amount);

    void set_filter_enabled(bool enabled);

    void set_filter_frequency(float frequency);

    void set_filter_resonance(float resonance);

    void set_filter_peak_gain(float peak_gain);

    void set_filter_type(FilterType type);

    void set_filter_db24(bool db24);

    void set_filter_env_enabled(bool enabled);

    // Set the filter envelope amount in the range of -1 to 1
    void set_filter_env_amount(float amount);

    // Set the filter envelope attack in samples
    void set_filter_env_attack(float attack);

    // Set the filter envelope decay in samples
    void set_filter_env_decay(float decay);

    // Set the filter envelope sustain in the range of 0 to 1
    void set_filter_env_sustain(float sustain);

    // Set the filter envelope release in samples
    void set_filter_env_release(float release);

    void set_pitch_env_enabled(bool enabled);

    void set_pitch_env_amount(float amount);

    void set_pitch_env_attack(float attack);

    void set_pitch_env_decay(float decay);

    void set_pitch_env_sustain(float sustain);

    void set_pitch_env_release(float release);

    void set_mode(Modes mode);



    void set_attck(float attack);
    void set_decay(float decay);
    void set_sustain(float sustain);
    void set_release(float release);
    void reset();
    // Set the sustain pedal on or off
    void set_sustain_pedal(bool sustain_pedal);
    vector<Voice> voices;
    vector<Voice *> active_voices;
    vector<Voice*> results;
    

protected:
    struct active_notes
    {
        u_int note;
        u_int velocity;
        u_int64_t age;
    };
    vector<active_notes> _active_notes;
    int _global_transpose_offset = 0;
    float _global_detune_offset = 0;
    u_int _base_note = 60;
    u_int _start = 0;
    u_int _end = 0;
    bool _loop = false;
    bool _reverse = false;
    bool _legato = true;
    unsigned int _sample_rate = 44100;
    bool _sustain_pedal = false;
    Modes _mode = Modes::ADSR;
    u_int64_t _voice_age = 0;
    u_int max_polyphony = 1;

    // Unison settings
    u_int _unison_voices = 1;
    float _unison_detune_amount = 1;
    float _unison_detune_amount_max = 2;
    float _unison_pan_amount = 0.5;
    vector<float> _unison_offsets;
    vector<float> _unison_pan_offsets;

    void _calculate_unison_offsets();
    void _calculate_unison_pans();

    bool _filter_env_enabled = false;
    bool _pitch_env_enabled = false;
};



struct Voice
{
    Voice(VoiceManager *manager = nullptr)
    {
        amplitude_envelope.setAttackRate(0.1);
        amplitude_envelope.setDecayRate(0.1);
        amplitude_envelope.setSustainLevel(0.5);
        amplitude_envelope.setReleaseRate(0.1);
        this->manager = manager;
        voice_filter_node = make_unique<FilterNode>(44100);
    }
    VoiceManager *manager;
    unsigned int note = 60;
    float unison_detune = 0;
    float unison_pan = 0;
    unsigned int velocity = 0;
    bool note_on = false;
    bool is_playing = false;
    u_int64_t age;
    float counter = 0;
    unique_ptr<FilterNode> voice_filter_node;
    float base_filter_frequency = 20000;
    ADSR amplitude_envelope;
    ADSR filter_envelope;
    float filter_envelope_ratio = 0;
    ADSR pitch_envelope;
    float pitch_envelope_ratio = 0;
    unsigned int sample_rate = 44100;
    void reset()
    {
        counter = 0;
        stop();
        reset_adsr();
    }
    void trigger(int note, int velocity, int start = 0)
    {
        this->note = note;
        this->note_on = true;
        this->velocity = velocity;
        this->counter = start;
        this->is_playing = true;
        set_adsr_gate(true);
    }
    void set_adsr_gate(bool gate)
    {
        if (gate)
        {
          amplitude_envelope.gate(1);
          filter_envelope.gate(1);
          pitch_envelope.gate(1);
        }
        else
        {
          amplitude_envelope.gate(0);
          filter_envelope.gate(0);
          pitch_envelope.gate(0);
        }
    }
    void set_filter_frequency(float frequency)
    {
        base_filter_frequency = frequency;
        voice_filter_node->set_filter_frequency(frequency);
    }
    void process_adsr(int sample_count)
    {
      amplitude_envelope.process(sample_count);
      filter_envelope.process(sample_count);
      pitch_envelope.process(sample_count);
    }
    void reset_adsr()
    {
      amplitude_envelope.reset();
      filter_envelope.reset();
      pitch_envelope.reset();
    }
    void set_adsr(float attack, float decay, float sustain, float release)
    {
        amplitude_envelope.setAttackRate(attack);
        amplitude_envelope.setDecayRate(decay);
        amplitude_envelope.setSustainLevel(sustain);
        amplitude_envelope.setReleaseRate(release);
    }
    void stop()
    {
        this->velocity = 0;
        this->note_on = false;
        this->is_playing = false;
    }
};


#endif // !VOICEMANAGER_H