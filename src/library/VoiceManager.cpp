#include "VoiceManager.h"
#include "utility.h"
#include <iostream>
#include <string>
#include <sys/_types/_u_int64_t.h>

// Static method to trigger a new voice. Done to minimize code duplication.
static void trigger_new_voice(vector<Voice> &voices, MidiMsg &msg, unsigned int starting_sample, u_int64_t new_age, vector<Voice *> &active_voices)
{
    for (auto &voice : voices)
    {
        if (!voice.is_playing)
        {
            voice.unison_detune = 0;
            voice.unison_pan = 0;
            voice.age = new_age;
            voice.trigger(msg.getNoteNumber(), msg.getVelocity(), starting_sample);
            active_voices.push_back(&voice);
            break;
        }
    }
}

static Voice * find_oldest_voice(vector<Voice> &voices)
{
    Voice *oldest_voice = nullptr;
    for (auto &voice : voices)
    {
        if (!oldest_voice)
        {
            oldest_voice = &voice;
        }
        else if (voice.age < oldest_voice->age)
        {
            oldest_voice = &voice;
        }
    }
    return oldest_voice;
}

VoiceManager::VoiceManager(unsigned int polyphony, unsigned int sample_rate)
    {
        // reserve and fill voices vector by polyphony
        voices.reserve(128);
        this->results.reserve(128);
        this->_sample_rate = sample_rate;
        for (int i = 0; i < 128; i++)
        {
            voices.push_back(Voice(this));
        }
    }
    u_int VoiceManager::get_starting_sample()
    {
        return (_reverse) ? _end - 1 : _start;
    }
    void VoiceManager::set_mode(Modes mode)
    {
        _mode = mode;
        reset();
    }
    VoiceManager::Modes VoiceManager::get_mode()
    {
        return _mode;
    }
    void VoiceManager::trigger_voice_start(MidiMsg &msg)
    {
        switch (_mode)
        {
        case Modes::ADSR:
            trigger_voice_start_adsr(msg);
            break;
        case Modes::ONE_SHOT:
            trigger_voice_start_one_shot(msg);
            break;
        default:
            break;
        }
    }
    void VoiceManager::trigger_voice_stop(MidiMsg &msg)
    {
        switch (_mode)
        {
        case Modes::ADSR:
            trigger_voice_stop_adsr(msg);
            break;
        case Modes::ONE_SHOT:
            trigger_voice_stop_one_shot(msg);
            break;
        default:
            break;
        }
    }
    void VoiceManager::trigger_voice_start_adsr(MidiMsg &msg)
    {
        auto note_number = msg.getNoteNumber();
        auto velocity = msg.getVelocity();

        // Reverse sample related logic
        auto starting_sample = get_starting_sample();
        auto new_age = ++_voice_age;

        // Add the note to the active notes
        _active_notes.push_back({note_number, velocity, new_age});

        // Unison voice handling
        if(_unison_voices > 1)
        {
            // Voice stealing when polyphony is reached
            if(get_active_voice_count() >= max_polyphony * _unison_voices){
                // Find the oldest voice and replace it
                Voice *oldest_voice = find_oldest_voice(voices);
                if(oldest_voice){
                    // Trigger or retune each one of the unison voices that share the same note based on legato
                    if(_legato){

                        // If the oldest note is still being played, retune every unison voice
                            // retune every unison voice
                            for(auto &v : voices){
                                if(v.age == oldest_voice->age){
                                    v.note = note_number;
                                }
                            }
                            
                        // If the oldest note is not being played, retrigger every unison voice with the new note
                        // else{
                        //     std::cout << "Oldest voice not playing, retrigger" << std::endl;
                        //     for(auto &v : voices){
                        //         if(v.note == oldest_voice->note){
                        //             v.age = new_age;
                        //             v.trigger(note_number, velocity, starting_sample);
                        //         }
                        //     }
                        // }
                    }
                    // No legato, just retrigger every unison voice with the new note
                    else{
                        for(auto &v : voices){
                            if(v.note == oldest_voice->note){
                                v.age = new_age;
                                v.trigger(note_number, velocity, starting_sample);
                            }
                        }
                    }
                }
                // If no oldest voice is found, trigger a new voice
                else
                {
                    for (int i = 0; i < _unison_voices; i++)
                    {
                        for (auto &voice : voices)
                        {
                            if (!voice.is_playing)
                            {
                                voice.unison_detune = _unison_offsets[i];
                                voice.unison_pan = _unison_pan_offsets[i];
                                voice.age = new_age;
                                voice.trigger(note_number, velocity, starting_sample);
                                active_voices.push_back(&voice);
                                break;
                            }
                        }
                    }
                }
            }
            // If polyphony is not reached, trigger a new voice
            else
            {
                for (int i = 0; i < _unison_voices; i++)
                    {
                        for (auto &voice : voices)
                        {
                            if (!voice.is_playing)
                            {
                                voice.unison_detune = _unison_offsets[i];
                                voice.unison_pan = _unison_pan_offsets[i];
                                voice.age = new_age;
                                voice.trigger(note_number, velocity, starting_sample);
                                active_voices.push_back(&voice);
                                break;
                            }
                        }
                    }
            }
        }
        // Single Unison voice handling =======================================
        else
        {
            // Voice stealing when polyphony is reached
            if(get_active_voice_count() >= max_polyphony)
            {
                // Find the oldest voice and replace it
                Voice *oldest_voice = find_oldest_voice(voices);

                // Trigger or retune the oldest voice based on legato
                if(oldest_voice)
                {
                    // Trigger or retune the oldest voice based on legato
                    if(_legato){
                        if (oldest_voice->note_on)
                        {
                            oldest_voice->note = note_number;
                            oldest_voice->velocity = velocity;
                        }
                        else{
                            oldest_voice->unison_detune = 0;
                            oldest_voice->unison_pan = 0;
                            oldest_voice->age = new_age;
                            oldest_voice->trigger(note_number, velocity, starting_sample);
                        }
                    }

                    // No legato, just trigger the oldest voice
                    else{
                        oldest_voice->unison_detune = 0;
                        oldest_voice->unison_pan = 0;
                        oldest_voice->age = new_age;
                        oldest_voice->trigger(note_number, velocity, starting_sample);
                    }
                }

                // If no oldest voice is found, trigger a new voice
                else {
                    trigger_new_voice(voices, msg, starting_sample, new_age, active_voices);
                }
            }

            // If polyphony is not reached, trigger a new voice
            else
            {
                trigger_new_voice(voices, msg, starting_sample, new_age, active_voices);
            }
            
        }
    }

    // TODO: Test legato and polyphony with sustain pedal.
    void VoiceManager::trigger_voice_stop_adsr(MidiMsg &msg)
    {
        // Remove the note from the active notes
        _active_notes.erase(std::remove_if(_active_notes.begin(), _active_notes.end(), [&](auto &note){
            return note.note == msg.getNoteNumber();
        }), _active_notes.end());

        for (auto it = voices.begin(); it != voices.end(); it++)
        {
            if (it->note == msg.getNoteNumber())
            {
                it->note_on = false;

                // Sustain pedal handling
                if (!_sustain_pedal)
                {
                    // If the max_polyphony is 1, and legato is enabled, we need to retune the voice to the most youngest _active_note
                    if(max_polyphony == 1 && _legato){
                        // find the youngest note and retune the voice
                        auto youngest_note = std::max_element(_active_notes.begin(), _active_notes.end(), [&](auto &a, auto &b){
                            return a.age < b.age;
                        });
                        if(youngest_note != _active_notes.end()){
                            it->note = youngest_note->note;
                        }
                        else{
                            it->set_adsr_gate(false);
                        }
                    }
                    // If the voice is not playing, stop the voice
                    else{
                        it->set_adsr_gate(false);
                    }

                }

                
            }
        }
    }

    // TODO: Implement one shot mode
    void VoiceManager::trigger_voice_start_one_shot(MidiMsg &msg){

    }
    void VoiceManager::trigger_voice_stop_one_shot(MidiMsg &msg){

    }
    u_int VoiceManager::get_active_voice_count()
    {
        return active_voices.size();
    }

    void VoiceManager::increment_counter(Voice &v)
    {
        // Calculate semitone difference and transposition
        int semitone_difference = ((int)v.note + _global_transpose_offset) - (int)_base_note;

        // Apply detune
        // TODO: Need to implement pitch envelope here. Possible modulate the detune amount
        float rate = powf(2, (float)semitone_difference / 12);
        float detune_ratio = pow(2.0f, (float)_global_detune_offset / 1200.0f);
        float unison_detune = pow(2.0f, (float)v.unison_detune / 1200.0f);
        rate = detune_ratio * rate * unison_detune;

        // Reverse counter if reverse is enabled
        if (_reverse)
        {
            v.counter -= rate;
            if (v.counter <= _start)
            {
                v.counter = _end;
                if (!_loop)
                {
                    v.stop();

                }
            }
        }

        // Increment counter
        else
        {
            v.counter += rate;
            if (v.counter >= _end)
            {
                v.counter = _start;
                if (!_loop)
                {
                    v.stop();
                }
            }
        }
        
    }

    vector<Voice*> * VoiceManager::process()
    {
        results.clear();
        // Process each voice
        for (auto &voice : voices)
        {
            if (voice.is_playing)
            {
                // Apply ADSR
                // cout << "Counter: " << voice.counter << "Start: " << _start << " End: " << _end << " \n";

                results.push_back(&voice);
                // Apply Filter Envelope if enabled
                if(_filter_env_enabled){
                    auto filter_envelope_muodulator = voice.filter_envelope.getOutput();
                    float upper_range  = 20000.0 - voice.base_filter_frequency;
                    float lower_range = voice.base_filter_frequency - 20.0;
                    if(filter_envelope_muodulator > 0){
                        float new_freq = clamp<float>(voice.base_filter_frequency + (upper_range * filter_envelope_muodulator), 20.0, 20000.0);
                        voice.voice_filter_node->set_filter_frequency(new_freq);
                    }
                    else{
                        float new_freq = clamp<float>(voice.base_filter_frequency + (lower_range * filter_envelope_muodulator), 20.0, 20000.0);
                        voice.voice_filter_node->set_filter_frequency(new_freq);
                    }
                      
                }
                
                // // Apply Pitch Envelope if enabled
                // if(_pitch_env_enabled){
                //     auto pitch_envelope_muodulator = voice.pitch_envelope.getOutput();
                //     auto pitch_ratio = weighted_map_float(
                //         pitch_envelope_muodulator, -1.0, 1.0, 0.5, 2.0, 1.0);
                //     voice.unison_detune = pitch_ratio;  
                // }

                // Add the voice to the reults vector for processing

                // When the Envelope is finished, call stop which will remove the voice from active_voices
                if (voice.amplitude_envelope.getState() == env_idle){
                    voice.stop();
                    active_voices.erase(std::remove(active_voices.begin(), active_voices.end(), &voice), active_voices.end());
                }

                // Increment the envelopes counter
                voice.process_adsr(1);

                // Increment the voice counter
                increment_counter(voice);
            }
        }
        return &results;
    }

    void VoiceManager::reset(){
        for (auto &voice : voices)
        {
            voice.reset();
        }
    }
    // Setters


    // Set legato
    void VoiceManager::set_legato(bool legato)
    {
        this->_legato = legato;
    }

    // Set the polyphony
    void VoiceManager::set_polyphony(u_int polyphony)
    {
        this->max_polyphony = polyphony;
    }

    // Set the transpose offset
    void VoiceManager::set_transpose_offset(int offset)
    {
        cout << "Transpose Offset: " << offset << "\n";
        _global_transpose_offset = offset;
    }

    // Set the detune offset
    void VoiceManager::set_detune_offset(float offset)
    {
        cout << "Detune Offset: " << offset << "\n";
        _global_detune_offset = offset;
    }

    // Set the base note
    void VoiceManager::set_base_note(unsigned int note)
    {
        cout << "Base Note: " << note << "\n";
        _base_note = note;
    }

    // Set the loop
    void VoiceManager::set_loop(bool loop)
    {
        this->_loop = loop;
    }

    // Set the start sample buffer index
    void VoiceManager::set_start(unsigned int start)
    {
        this->_start = start;
    }

    // Set the end sample buffer index
    void VoiceManager::set_end(unsigned int end)
    {
        this->_end = end;
    }

    // Set the reverse. If true, the sample will play in reverse
    void VoiceManager::set_reverse(bool reverse)
    {
        this->_reverse = reverse;
    }

    // Set the sample rate
    void VoiceManager::set_sample_rate(unsigned int sample_rate)
    {
        this->_sample_rate = sample_rate;
    }
    void VoiceManager::_calculate_unison_offsets()
    {
        _unison_offsets.clear();
        float detune_range = float(_unison_detune_amount_max - (-_unison_detune_amount_max)) * float(_unison_detune_amount);
        float interval = detune_range / float(_unison_voices+1);
        float start = 0 - float(detune_range) / 2 ;
        start += interval;

        for (int i = 0; i < _unison_voices; i++)
        {
            _unison_offsets.push_back(start);
            start += interval;
        }
    }

    void VoiceManager::_calculate_unison_pans()
    {
        _unison_pan_offsets.clear();
        float pan_offset_range = 2.0f * float(_unison_pan_amount);
        float interval = pan_offset_range / float(_unison_voices + 1);
        float start = 0 - float(pan_offset_range) / 2;
        start += interval;
        for (int i = 0; i < _unison_voices; i++)
        {
            _unison_pan_offsets.push_back(start);
            start += interval;
        }
    }
    void VoiceManager::set_unison_voices(u_int unison_voices)
    {
        this->_unison_voices = unison_voices;
        this->_calculate_unison_offsets();
        this->_calculate_unison_pans();
    }

    // Set the unison detune
    void VoiceManager::set_unison_detune(float detune_ratio)
    {
        this->_unison_detune_amount = detune_ratio;
        this->_calculate_unison_offsets();
        this->_calculate_unison_pans();
    }

    // Set the unison detune maximum amount
    void VoiceManager::set_unison_detune_amount_max(float detune_amount_max)
    {
        this->_unison_detune_amount_max = detune_amount_max;
        this->_calculate_unison_offsets();
        this->_calculate_unison_pans();
    }

    // Set the unison pan amount
    void VoiceManager::set_unison_pan_amount(float pan_amount)
    {
        this->_unison_pan_amount = pan_amount;
        this->_calculate_unison_offsets();
        this->_calculate_unison_pans();

    }
    void VoiceManager::set_attck(float attack)
    {
        for (auto &voice : voices)
        {
            voice.amplitude_envelope.setAttackRate(attack * _sample_rate);
        }
    }
    void VoiceManager::set_decay(float decay)
    {
        for (auto &voice : voices)
        {
            voice.amplitude_envelope.setDecayRate(decay * _sample_rate);
        }
    }
    void VoiceManager::set_sustain(float sustain)
    {
        for (auto &voice : voices)
        {
            voice.amplitude_envelope.setSustainLevel(sustain);
        }
    }
    void VoiceManager::set_release(float release)
    {
        for (auto &voice : voices)
        {
            voice.amplitude_envelope.setReleaseRate(release * _sample_rate);
        }
    }

    void VoiceManager::set_filter_env_enabled(bool enabled)
    {
        _filter_env_enabled = enabled;
    }

    void VoiceManager::set_filter_env_amount(float amount)
    {
        for (auto &voice : voices)
        {
            voice.filter_envelope.setAmount(amount);
        }
    }

    void VoiceManager::set_filter_env_attack(float attack)
    {
        for (auto &voice : voices)
        {
            voice.filter_envelope.setAttackRate(attack );
        }
    }

    void VoiceManager::set_filter_env_decay(float decay)
    {
        for (auto &voice : voices)
        {
            voice.filter_envelope.setDecayRate(decay );
        }
    }

    void VoiceManager::set_filter_env_sustain(float sustain)
    {
        for (auto &voice : voices)
        {
            voice.filter_envelope.setSustainLevel(sustain);
        }
    }

    void VoiceManager::set_filter_env_release(float release)
    {
        for (auto &voice : voices)
        {
            voice.filter_envelope.setReleaseRate(release );
        }
    }

    void VoiceManager::set_pitch_env_enabled(bool enabled)
    {
        _pitch_env_enabled = enabled;
    }

    void VoiceManager::set_pitch_env_amount(float amount)
    {
        for (auto &voice : voices)
        {
            voice.pitch_envelope.setAmount(amount);
        }
    }

    void VoiceManager::set_pitch_env_attack(float attack)
    {
        for (auto &voice : voices)
        {
            voice.pitch_envelope.setAttackRate(attack);
        }
    }

    void VoiceManager::set_pitch_env_decay(float decay)
    {
        for (auto &voice : voices)
        {
            voice.pitch_envelope.setDecayRate(decay );
        }
    }

    void VoiceManager::set_pitch_env_sustain(float sustain)
    {
        for (auto &voice : voices)
        {
            voice.pitch_envelope.setSustainLevel(sustain);
        }
    }

    void VoiceManager::set_pitch_env_release(float release)
    {
        for (auto &voice : voices)
        {
            voice.pitch_envelope.setReleaseRate(release );
        }
    }

    // Set the sustain pedal on or off
    void VoiceManager::set_sustain_pedal(bool sustain_pedal)
    {

        // Sustain pedal is pressed
        if (sustain_pedal)
        {
            if (!_sustain_pedal)
            {
                _sustain_pedal = true;
            }
        }

        // Sustain pedal is released
        else
        {
            // Only processed if the sustain pedal was pressed
            if (_sustain_pedal)
            {
                _sustain_pedal = false;
                for (auto &voice : voices)
                {
                    if (!voice.note_on)
                    {
                        voice.set_adsr_gate(false);
                    }
                }
            }
        }
    }
    void VoiceManager::set_filter_frequency(float frequency)
    {
        for (auto &voice : voices)
        {   
            voice.set_filter_frequency(frequency);
        }
    }
    void VoiceManager::set_filter_resonance(float resonance)
    {
        for (auto &voice : voices)
        {
            voice.voice_filter_node->set_filter_resonance(resonance);
        }
    }
    void VoiceManager::set_filter_peak_gain(float peak_gain)
    {
        for (auto &voice : voices)
        {
            voice.voice_filter_node->set_peak_gain(peak_gain);
        }
    }
    void VoiceManager::set_filter_type(FilterType type)
    {
        for (auto &voice : voices)
        {
            voice.voice_filter_node->set_type(type);
        }
    }
    void VoiceManager::set_filter_enabled(bool enabled)
    {
        for (auto &voice : voices)
        {
            voice.voice_filter_node->set_enabled(enabled);
        }
    }
    void VoiceManager::set_filter_db24(bool db24)
    {
        for (auto &voice : voices)
        {
            voice.voice_filter_node->set_db24(db24);
        }
    }

