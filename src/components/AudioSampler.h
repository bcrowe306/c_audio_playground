#pragma once
#include "audio/choc_AudioFileFormat_WAV.h"
#include "bw_chorus.h"
#include "bw_lp1.h"
#include "core/AudioNode.h"
#include "core/MidiMsg.h"
#include "core/MidiNode.h"
#include "library/ADSR.h"
#include "library/Biquad.h"
#include "library/Debouncer.h"
#include "library/Observer.h"
#include "library/Parameter.h"
#include "library/VoiceAllocator.h"
#include "library/VoiceManager.h"
#include "library/utility.h"
#include "nodes/BufferNode.h"
#include "nodes/FilterNode.h"
#include "nodes/LevelMeterNode.h"
#include "nodes/PanNode.h"
#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using namespace choc::audio;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;



class AudioSampler : public AudioNode, public MidiNode
{
public:
    unsigned int sample_rate = 44100;
    float counter = 0;
    int new_note = 60;
    bool is_playing = false;
    bool loading = false;
    shared_ptr<BufferNode> audio_buffer;

    IntParameter base_note = IntParameter("Base Note", 60, 0, 127, 1);
    IntParameter start = IntParameter("Start", 0, 0, 1000000, 1);
    IntParameter end = IntParameter ("End", 0, 0, 1000000, 1);
    BoolParameter loop = BoolParameter("Loop", false);
    BoolParameter enabled = BoolParameter("Enabled", true);
    FloatParameter attack = FloatParameter("Attack", 0.001, 0.0, 10, 0.001);
    FloatParameter decay = FloatParameter("Decay", 0.1, 0.0, 10, 0.01);
    FloatParameter sustain = FloatParameter("Sustain", 1, 0.0, 1, 0.01);
    FloatParameter release = FloatParameter("Release", 0.1, 0.0, 10, 0.01);
    FloatParameter volume = FloatParameter("Volume", -12, -60, 6, 0.1);
    FloatParameter pan = FloatParameter("Pan", 0, -1, 1, 0.01);
    BoolParameter filter_enabled = BoolParameter("Filter Enabled", true);
    FloatParameter filter_frequency = FloatParameter("Filter Frequency", 500, 20, 20000, 20);
    FloatParameter filter_resonance = FloatParameter("Filter Resonance", 0.707, 0.1, 10, 0.1);
    FloatParameter filter_peak_gain = FloatParameter("Filter Peak Gain", 0, -40, 10, 1);
    BoolParameter filter_db24 = BoolParameter("24db", false);
    BoolParameter reverse = BoolParameter("Reverse", false);
    IntParameter polyphony = IntParameter("Polyphony", 8, 1, 128, 1);
    BoolParameter legato    = BoolParameter("Legato", false);
    IntParameter transpose = IntParameter("Transpose", 0, -24, 24, 1);
    IntParameter detune = IntParameter("Detune", 0, -100, 100, 1);
    FloatParameter velocity_sestivity = FloatParameter("Velocity Sensitivity", 1, 0, 1, 0.01);
    FloatParameter glide_time = FloatParameter("Glide", 0, 0, 100, 0.1);

    // Unison
    IntParameter unison_voices = IntParameter("Unison Voices", 1, 1, 8, 1);
    FloatParameter unison_detune = FloatParameter("Unison Detune", 1, 0, 1, 0.01);
    FloatParameter unison_detune_max = FloatParameter("Unison Detune Max", 2, 0, 100, 0.1);
    FloatParameter unison_pan = FloatParameter("Unison Pan", 0.5, 0, 1, 0.01); 

    // Filter envelope
    FloatParameter filter_keytrack = FloatParameter("Filter Keytrack", 0, 0, 100, 1.0);
    FloatParameter filter_velocity_track = FloatParameter("Filter Velocity Track", 0, 0, 100, 1.0);
    FloatParameter filter_env_attack = FloatParameter("Filter Env Attack", 0.001, 0.0, 10, 0.001);
    FloatParameter filter_env_decay = FloatParameter("Filter Env Decay", 0.1, 0.0, 10, 0.01);
    FloatParameter filter_env_sustain = FloatParameter("Filter Env Sustain", 1.0, 0.0, 1, 0.01);
    FloatParameter filter_env_release = FloatParameter("Filter Env Release", 0.1, 0.0, 10, 0.01);
    FloatParameter filter_env_amount = FloatParameter("Filter Env Amount", 0, -1, 1, .01);
    BoolParameter filter_env_enabled = BoolParameter("Filter Env Enabled", true);

    // Pitch envelope
    FloatParameter pitch_env_attack = FloatParameter("Pitch Env Attack", 0.001, 0.0, 10, 0.001);
    FloatParameter pitch_env_decay = FloatParameter("Pitch Env Decay", 0.1, 0.0, 10, 0.01);
    FloatParameter pitch_env_sustain = FloatParameter("Pitch Env Sustain", 1, 0.0, 1, 0.01);
    FloatParameter pitch_env_release = FloatParameter("Pitch Env Release", 0.1, 0.0, 10, 0.01);
    FloatParameter pitch_env_amount = FloatParameter("Pitch Env Amount", 0, -100, 100, 1);
    BoolParameter pitch_env_enabled = BoolParameter("Pitch Env Enabled", false);


    EnumParameter filter_type = EnumParameter("Filter Type", FilterNode::filter_types, 0);
    EnumParameter mode = EnumParameter("Mode", voice_allocator_mode_strings, 0);
    AudioSampler(std::string file_path, unsigned int sample_rate = 44100);
    ~AudioSampler();

    // Nodes
    shared_ptr<FilterNode> filter;
    shared_ptr<PanNode> pan_node;;
    shared_ptr<LevelMeterNode> level_meter_node;
    VoiceAllocator voice_manager;
    u_int get_max_length();
    ADSR _filter_envelope;

    float get_playhead_position();
    void audition();

    // Voices methods
    void set_polyphony(u_int polyphony);

protected:
    
    unsigned int length = 0;
    ADSR _pitch_envelope;
    float *processing_samples = new float[2]{0, 0};
    float _base_note;
    bool _loop = false;
    bool _reverse = false;
    bool _sustain_pedal = false;
    Debouncer<bool> sustain_pedal_debouncer;
    choc::buffer::BufferView<float, choc::buffer::SeparateChannelLayout> _buffer_view;
    MidiMsg _audition_message = MidiMsg(0x90, 60, 100);
    Brickworks::Chorus<2> chorus;


    void setup_parameters();
    void set_filter_enabled(bool enabled);
    void set_filter_frequency(float frequency);
    void set_filter_resonance(float resonance);
    void set_filter_peak_gain(float peak_gain);
    void set_filter_type(int enum_value);
    void set_filter_db24(bool db24);
    void set_filter_frequency_from_midi(bool direction);
    void set_enabled(bool enabled);
    void set_volume(float volume);
    void set_attack(float attack);
    void set_decay(float decay);
    void set_sustain(float sustain);
    void set_release(float release);
    void set_base_note(unsigned int note);
    void set_loop(bool loop);
    void set_start(unsigned int start);
    void set_end(unsigned int end);
    void on_buffer_node_state_changed(string state);


    // Velocity Sensitivity
    // @param velocity: The velocity of the note in the range of 0-127
    // @param sensitivity: The sensitivity of the velocity in the range of 0-1
    // @param range: The range of the sensitivity
    // @param shift: The shift of the sensitivity
    // @return The mapped linear amplitude attenuation for each sample of this voice
    float calculate_velocity_sensitivity(float velocity, float sensitivity, float range = 2, float shift = 1);
    void process_audio(float *input_buffer, float *output_buffer, unsigned long frameCount , AudioEngineContext &context) override;
    void get_interpolated_samples(SynthVoice &v);
    void _process_midi(MidiMsg msg) override;
    bool offline = true;
    int _transpose_offset = 0;
    float _velocity_sensitivity = 1;

    
};