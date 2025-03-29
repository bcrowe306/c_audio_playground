#include "AudioSampler.h"
#include "BufferNode.h"
#include "FilterNode.h"
#include "LevelMeterNode.h"
#include "MidiMsg.h"
#include "VoiceManager.h"
#include "rch_oscillators.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include "library/VoiceAllocator.h"



AudioSampler::AudioSampler(std::string file_path, unsigned int sample_rate) : voice_manager(4)
    {

        // Setup nodes
        audio_buffer = make_shared<BufferNode>(file_path, sample_rate);
        audio_buffer->on_state_changed = std::bind(&AudioSampler::on_buffer_node_state_changed, this, std::placeholders::_1);
        filter = make_shared<FilterNode>(sample_rate);
        pan_node = make_shared<PanNode>();

        setup_parameters();

        // TODO: Need a better way to get sample rate from audio engine
        this->sample_rate = sample_rate;
        audio_buffer->load_sample(file_path);

        // Add level meter node to chain
        level_meter_node = make_shared<LevelMeterNode>();
        add_node_to_chain(level_meter_node);
        std::cout << "AudioSampler created\n";


    }
    AudioSampler::~AudioSampler()
    {
        std::cout << "AudioSampler destroyed\n";
    }
    void AudioSampler::setup_parameters(){

        // Setup parameters and listeners


        mode.on_change = [&](int value) {
            voice_manager.set_mode((VoiceAllocator::Modes)value);
        };
        mode.update();

        volume.on_change =
            std::bind(&AudioSampler::set_volume, this, std::placeholders::_1);
        volume.update();
        attach_parameter(&volume);

        pan.on_change = [&](float value) { pan_node->set_pan(value); };
        pan.update();
        attach_parameter(&pan);

        enabled.on_change =
            std::bind(&AudioSampler::set_enabled, this, std::placeholders::_1);
        enabled.update();
        attach_parameter(&enabled);

        loop.on_change =
            std::bind(&AudioSampler::set_loop, this, std::placeholders::_1);
        loop.update();
        attach_parameter(&loop);


        legato.on_change = [&](bool value) {
            voice_manager.set_legato(value);
        };
        legato.update();
        attach_parameter(&legato);

        glide_time.on_change = [&](float value) {
            value = value * sample_rate / 1000;
            voice_manager.set_glide_time(value);
        };
        glide_time.update();
        attach_parameter(&glide_time);

        polyphony.on_change = [&](int value) {
            voice_manager.set_polyphony(value);
        };
        polyphony.update();
        attach_parameter(&polyphony);

        start.on_change =
            std::bind(&AudioSampler::set_start, this, std::placeholders::_1);
        start.update();
        attach_parameter(&start);

        end.on_change =
            std::bind(&AudioSampler::set_end, this, std::placeholders::_1);
        end.update();
        attach_parameter(&end);

        base_note.on_change =
            std::bind(&AudioSampler::set_base_note, this, std::placeholders::_1);
        base_note.update();
        attach_parameter(&base_note);

        attack.on_change =
            std::bind(&AudioSampler::set_attack, this, std::placeholders::_1);
        attack.update();
        attach_parameter(&attack);

        decay.on_change =
            std::bind(&AudioSampler::set_decay, this, std::placeholders::_1);
        decay.update();
        attach_parameter(&decay);

        sustain.on_change =
            std::bind(&AudioSampler::set_sustain, this, std::placeholders::_1);
        sustain.update();
        attach_parameter(&sustain);

        release.on_change =
            std::bind(&AudioSampler::set_release, this, std::placeholders::_1);
        release.update();
        attach_parameter(&release);

        filter_enabled.on_change = std::bind(&AudioSampler::set_filter_enabled,
                                            this, std::placeholders::_1);
        filter_enabled.update();
        attach_parameter(&filter_enabled);

        filter_frequency.on_change = std::bind(
            &AudioSampler::set_filter_frequency, this, std::placeholders::_1);
        filter_frequency.update();
        attach_parameter(&filter_frequency);

        filter_resonance.on_change = std::bind(
            &AudioSampler::set_filter_resonance, this, std::placeholders::_1);
        filter_resonance.update();
        attach_parameter(&filter_resonance);

        filter_peak_gain.on_change = std::bind(
            &AudioSampler::set_filter_peak_gain, this, std::placeholders::_1);
        filter_peak_gain.update();
        attach_parameter(&filter_peak_gain);

        filter_db24.on_change = std::bind(&AudioSampler::set_filter_db24, this,
                                        std::placeholders::_1);
        filter_db24.update();
        attach_parameter(&filter_db24);
        filter_type.on_change = std::bind(&AudioSampler::set_filter_type, this,
                                        std::placeholders::_1);
        filter_type.update();

        // Filter Envelope Parameters ==================================
        
        // Filter Envelope Enabled
        filter_keytrack.on_change = [&](float value) {
            voice_manager.set_filter_keytrack(value);
        };
        filter_keytrack.update();
        attach_parameter(&filter_keytrack);

        filter_velocity_track.on_change = [&](float value) {
            voice_manager.set_filter_velocity_track(value);
        };
        filter_velocity_track.update();
        attach_parameter(&filter_velocity_track);

        filter_env_enabled.on_change = [&](bool value) {
            std::cout << "Filter Envelope Enabled: " << value << std::endl;
            voice_manager.set_filter_env_enabled(value);
        };
        filter_env_enabled.update();
        attach_parameter(&filter_env_enabled);

        // Filter Envelope Attack
        filter_env_attack.on_change = [&](float value) {
            voice_manager.set_filter_env_attack(value * sample_rate);
        };
        filter_env_attack.update();
        attach_parameter(&filter_env_attack);

        // Filter Envelope Decay
        filter_env_decay.on_change = [&](float value) {
            voice_manager.set_filter_env_decay(value * sample_rate);
        };
        filter_env_decay.update();
        attach_parameter(&filter_env_decay);

        // Filter Envelope Sustain
        filter_env_sustain.on_change = [&](float value) {
            voice_manager.set_filter_env_sustain(value);
        };
        filter_env_sustain.update();
        attach_parameter(&filter_env_sustain);

        // Filter Envelope Release
        filter_env_release.on_change = [&](float value) {
            voice_manager.set_filter_env_release(value * sample_rate);
        };
        filter_env_release.update();
        attach_parameter(&filter_env_release);

        // Filter Envelope Amount
        filter_env_amount.on_change = [&](float value) {
            voice_manager.set_filter_env_amount(value);
        };
        filter_env_amount.update();
        attach_parameter(&filter_env_amount);

        // Pitch Envelope Parameters

        // Pitch Envelope Attack
        pitch_env_attack.on_change = [&](float value) {
            voice_manager.set_pitch_env_attack(value * sample_rate);
        };
        pitch_env_attack.update();
        attach_parameter(&pitch_env_attack);

        // Pitch Envelope Decay
        pitch_env_decay.on_change = [&](float value) {
            voice_manager.set_pitch_env_decay(value * sample_rate);
        };
        pitch_env_decay.update();
        attach_parameter(&pitch_env_decay);

        // Pitch Envelope Sustain
        pitch_env_sustain.on_change = [&](float value) {
            voice_manager.set_pitch_env_sustain(value);
        };
        pitch_env_sustain.update();
        attach_parameter(&pitch_env_sustain);

        // Pitch Envelope Release
        pitch_env_release.on_change = [&](float value) {
            voice_manager.set_pitch_env_release(value * sample_rate);
        };
        pitch_env_release.update();
        attach_parameter(&pitch_env_release);

        // Pitch Envelope Amount
        pitch_env_amount.on_change = [&](float value) {
            voice_manager.set_pitch_env_amount(value);
        };
        pitch_env_amount.update();
        attach_parameter(&pitch_env_amount);

        // Unison Parameters
        unison_voices.on_change = [&](int value) {
        voice_manager.set_unison_voices(value);
        };
        unison_voices.update();
        attach_parameter(&unison_voices);

        unison_detune.on_change = [&](float value) {
        voice_manager.set_unison_detune(value);
        };
        unison_detune.update();
        attach_parameter(&unison_detune);

        unison_detune_max.on_change = [&](float value) {
        voice_manager.set_unison_detune_amount_max(value);
        };
        unison_detune_max.update();
        attach_parameter(&unison_detune_max);

        unison_pan.on_change = [&](float value) {
        voice_manager.set_unison_pan_amount(value);
        };
        unison_pan.update();
        attach_parameter(&unison_pan);


        // detune
        detune.on_change = [&](int value) {
        voice_manager.set_detune_offset(value);
        };
        detune.update();
        attach_parameter(&detune);


        transpose.on_change = [&](int value) {
        this->voice_manager.set_transpose_offset(value);
        };
        transpose.update();
        attach_parameter(&transpose);

        reverse.on_change = [&](bool value) { this->_reverse = value; };
        reverse.update();
        attach_parameter(&reverse);

        velocity_sestivity.on_change = [&](float value) {
            this->_velocity_sensitivity = value;
        };
        velocity_sestivity.update();
        attach_parameter(&velocity_sestivity);

    }
    void AudioSampler::on_buffer_node_state_changed(string state){
        cout << "Buffer Node State: " << state << "\n";
        if(state == BufferNode::States::OFFLINE){
            voice_manager.reset();
        }
        else if (state == BufferNode::States::LOADING) {
            voice_manager.reset();
        }
        else if (state == BufferNode::States::READY) {
            start.set_value(audio_buffer->get_start());
            end.set_value(audio_buffer->get_end());
        }
    }
    void AudioSampler::audition()
    {
        voice_manager.audition();
    }
    float AudioSampler::get_playhead_position()
    {
        auto earliest_counter = 0;
        for (auto &voice : *voice_manager.get_voices())
        {
            if (earliest_counter == 0 && voice.counter > 0)
            {
                earliest_counter = voice.counter;
                continue;
            }
    
            if (voice.counter < earliest_counter)
            {
                earliest_counter = voice.counter;
            }
        }
        return (float)earliest_counter / (float)end.get_value();
    }
    u_int AudioSampler::get_max_length()
    {
        return length;
    }
    
    void AudioSampler::set_enabled(bool enabled)
    {
        _enabled = enabled;
    }

    void AudioSampler::set_filter_enabled(bool enabled)
    {
        voice_manager.set_filter_enabled(enabled);
    }

    void AudioSampler::set_filter_frequency(float frequency)
    {
        voice_manager.set_filter_frequency(frequency);
    }
    void AudioSampler::set_filter_peak_gain(float peak_gain)
    {
        // filter->set_peak_gain(peak_gain);
        voice_manager.set_filter_peak_gain(peak_gain);
    }
    void AudioSampler::set_filter_type(int enum_value)
    {
        FilterType type = static_cast<FilterType>(enum_value);
        voice_manager.set_filter_type(type);
    }

    void AudioSampler::set_filter_db24(bool db24)
    {
        // filter->set_db24(db24);
        voice_manager.set_filter_db24(db24);
    }

    void AudioSampler::set_filter_frequency_from_midi(bool direction)
    {
        // set frequency rate by logarithmic formula
        auto rate = 1.0594630943592952645618252949463;
        auto current_frequency = filter_frequency.get_value();
        if (direction)
        {
            // increase frequency on logaritmic scale
            current_frequency *= rate;
        }
        else
        {
            // decrease frequency on logaritmic scale
            // logarithmic formula: f = f0 * 2^(n/12)

            current_frequency /= rate;
        }
        
        filter_frequency.set_value_interval(current_frequency, 20);
    }
    void AudioSampler::set_filter_resonance(float resonance)
    {
        // filter->set_filter_resonance(resonance);
        voice_manager.set_filter_resonance(resonance);
    }

    void AudioSampler::set_volume(float volume)
    {
    }

    void AudioSampler::set_attack(float attack)
    {
        voice_manager.set_attck(attack);
    }

    void AudioSampler::set_decay(float decay)
    {
        voice_manager.set_decay(decay);
    }

    void AudioSampler::set_sustain(float sustain)
    {
        voice_manager.set_sustain(sustain);
    }

    void AudioSampler::set_release(float release)
    {
        voice_manager.set_release(release);
    }

    void AudioSampler::set_base_note(unsigned int note)
    {
        _base_note = note;
        voice_manager.set_base_note(note);
    }

    void AudioSampler::set_loop(bool loop)
    {
        this->_loop = loop;
        voice_manager.set_loop(loop);
    }

    void AudioSampler::set_start(unsigned int start)
    {
        voice_manager.set_start(start);
    }

    void AudioSampler::set_end(unsigned int end)
    {
        voice_manager.set_end(end);
    }

    void AudioSampler::generate_audio(float *buffer_to_fill, unsigned long frameCount, AudioEngineContext &context)
    {

        // Check if sampler is offline, loading, disabled, or has empty buffer, and return if true
        
        if (audio_buffer->is_offline() || audio_buffer->is_loading() || _enabled == false)
        {
            return;
        }

        // Process each voice
        // reset processing samples
        processing_samples[0] = 0;
        processing_samples[1] = 0;
        
        auto voice_process_results = voice_manager.process();
        if(voice_process_results->size() == 0){
            return;
        }
        // std::cout << "Voice Process Results: " << voice_process_results->size() << std::endl;
        for (auto &voice : *voice_process_results)
        {

                // Get samples from buffer
                audio_buffer->get_interpolated_samples(voice.counter, processing_samples);

                // Apply velocity sensitivity
                processing_samples[0] = calculate_velocity_sensitivity(voice.velocity, _velocity_sensitivity) * processing_samples[0];
                processing_samples[1] = calculate_velocity_sensitivity(voice.velocity, _velocity_sensitivity) * processing_samples[1];

                // Apply ADSR
                float amp_env_modulator = voice.amplitude_envelope.getOutput();
                processing_samples[0] *= amp_env_modulator;
                processing_samples[1] *= amp_env_modulator;

                // Apply Pan per voice
                // TODO: Fix this... This is hacky and wrong
                process_pan(processing_samples, voice.pan);


                // Apply Filter
                if (voice.voice_filter_node->get_enabled()) {
                  voice.voice_filter_node->process_audio(processing_samples, processing_samples, frameCount, context);
                }

                // std::cout << "Processing Samples: " << processing_samples[0] << ", " << processing_samples[1] << std::endl;
                buffer_to_fill[0] += processing_samples[0];
                buffer_to_fill[1] += processing_samples[1];

                
        }

        // Apply Pan
        // auto panned_samples = pan_node->process(filtered_samples);

        // Apply volume
        buffer_to_fill[0] = buffer_to_fill[0] * db_to_linear(volume.get_value());
        buffer_to_fill[1] = buffer_to_fill[1] * db_to_linear(volume.get_value());

        return;
    }
    float AudioSampler::calculate_velocity_sensitivity(float velocity, float sensitivity, float range, float shift)
    {
        auto amplitude_attenuation = ((velocity / 127 - 0.5) * range) * sensitivity + shift;
        return amplitude_attenuation;
    }
    

    void AudioSampler::_process_midi(MidiMsg msg)
    {
        voice_manager.midi_input(msg);
    }