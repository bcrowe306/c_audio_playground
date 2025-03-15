#pragma once
#include <sys/_types/_u_int.h>
#ifndef VOICEALLOCATOR_H
#define VOICEALLOCATOR_H

#include <iostream>
#include <algorithm>
#include <utility>
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
#include "library/Debouncer.h"
#include "library/Smoother.h"

using std::vector;
using std::string;
using std::unique_ptr;
using std::make_unique;


static vector<std::string> voice_allocator_mode_strings = {"One Shot", "ADSR"};

struct NoteT {
    u_int note;
    u_int velocity;
    u_int age;
};

struct VoiceT {
    u_int note;
    u_int velocity;
    u_int age;
    u_int id;
};

struct SynthVoice {
    SynthVoice(u_int note, u_int velocity, u_int id, u_int start) : id(id), note(note), velocity(velocity), age(start) {
      note_smoother.set_initial_value((float )note);
      this->counter = start;
      voice_filter_node = make_unique<FilterNode>(44100);
      
    }
    float note = 60;
    unsigned int velocity = 0;
    u_int age;
    u_int id;
    float counter = 0;
    ADSR amplitude_envelope;
    ADSR filter_envelope;
    ADSR pitch_envelope;
    unique_ptr<FilterNode> voice_filter_node;
    float filter_frequency = 20000;
    float filter_resonance;
    float detune = 0;
    float unison_detune = 0;
    float pan;
    Smoother note_smoother = Smoother(44100, 0.1);

    // retune_note retunes the note of this voice to the target note over time specified by glide_time.
    // This method is used for portamento and glide effects. Not intended for immediate note changes.
    // @params target_note: The target note to retune to
    // @params glide_time: The time in samples to glide to the target note
    void retune_note(float target_note, float glide_time) {
      note_smoother.setTarget(target_note);
      note_smoother.setTimeSamples(glide_time);
    }
    
    // reset resets the voice to its initial state
    void reset() {
      counter = 0;
      stop();
      reset_adsr();
    }

    // trigger triggers the voice by setting the ADSR gate to true
    void trigger() {
      set_adsr_gate(true);
    }

    // Set release in samples
    void set_amplitude_env_release(float release) {
      amplitude_envelope.setReleaseRate(release);
    }

    // retrigger retrigger the voice with a new note, velocity, and start time
    // @params note: The note to retrigger
    // @params velocity: The velocity of the note
    // @params start: The start time in samples
    void retrigger(u_int note, u_int velocity, u_int start  ) {
      this->note = note;
      this->velocity = velocity;
      this->counter = start;
      // set_adsr_gate(false);
      set_adsr_gate(true);
    }

    // set_adsr_gate sets the ADSR gate to the specified value
    // @params gate: The value to set the gate to
    void set_adsr_gate(bool gate) {
      if (gate) {
        amplitude_envelope.gate(1);
        filter_envelope.gate(1);
        pitch_envelope.gate(1);
      } else {
        amplitude_envelope.gate(0);
        filter_envelope.gate(0);
        pitch_envelope.gate(0);
      }
    }
    void set_filter_frequency(float frequency) {
      filter_frequency = frequency;
      voice_filter_node->set_filter_frequency(frequency);
    }
    void set_filter_resonance(float resonance) {
      filter_resonance = resonance;
      voice_filter_node->set_filter_resonance(resonance);
    }
    void process_adsr(int sample_count) {
      amplitude_envelope.process(sample_count);
      filter_envelope.process(sample_count);
      pitch_envelope.process(sample_count);
    }
    void reset_adsr() {
      amplitude_envelope.reset();
      filter_envelope.reset();
      pitch_envelope.reset();
    }
    void set_adsr(float attack, float decay, float sustain, float release) {
      amplitude_envelope.setAttackRate(attack);
      amplitude_envelope.setDecayRate(decay);
      amplitude_envelope.setSustainLevel(sustain);
      amplitude_envelope.setReleaseRate(release);
    }
    void stop() {
      this->set_adsr_gate(false);
      this->velocity = 0;
    }

    void process(){
      note_smoother.process();
      if (note_smoother.isSmoothing()) {
        note = note_smoother.getValue();
      }
    }
};


class VoiceAllocator {
public:
    VoiceAllocator(u_int polyphony = 3){
        _polyphony = polyphony;
        _voices.reserve(128);
        _voice_tracker.reserve(128);
        _note_tracker.reserve(128);
        sustain_pedal_debouncer = Debouncer<bool>(44100, 15);
        sustain_pedal_debouncer.on_change(std::bind(&VoiceAllocator::set_sustain_pedal, this, std::placeholders::_1));
    };
    ~VoiceAllocator(){};

    enum class Modes {
      ONE_SHOT,
      ADSR,
    };
    // static vector<std::string> mode_strings;
    // Handle midi input
    void midi_input(MidiMsg &msg){

      if(msg.isNoteOn()){
        // Handle note on and note off
        _update_note_tracker(msg);
        _update_voice_tracker(msg);
        _process_flag = true;
        _voice_age++;
      }
      else if(msg.isNoteOff()){
        // Handle note on and note off
        _update_note_tracker(msg);
        _update_voice_tracker(msg);
        _process_flag = true;
      }
      else if(msg.isController()){
        if (msg.getControllerNumber() == 64) {
          if (msg.getControllerValue() == 127) {
            sustain_pedal_debouncer.update(true);
          } else {
            sustain_pedal_debouncer.update(false);
          }
        }

      }

     
      // reset age if all notes are off
      if(get_active_voice_count() == 0){
          _voice_age = 1;
      }
    };

    u_int get_active_voice_count(){
        return _voice_tracker.size();
    }
    void reset() {
      for (auto &voice : _voices) {
        voice.reset();
      }
    }

    // Filter Modulation function
    float get_filter_modulation(float filter_frequency, float modulation) {
        float upper_range = 20000.0 - filter_frequency;
        float lower_range = filter_frequency - 20.0;

        if (modulation > 0) {
          float new_freq = clamp<float>(
              filter_frequency + (upper_range * modulation),
              20.0, 20000.0);
          return new_freq;
        } else {
          float new_freq = clamp<float>(
              filter_frequency + (lower_range * modulation),
              20.0, 20000.0);
          return new_freq;
        }
    }
    float get_frequency_from_keytrack(float midiNote, float baseFreq,
                           float baseNote, float scaleAmount) {
      // Convert scale percentage to factor
      float scaleFactor = scaleAmount / 100.0f;

      // Compute frequency shift in semitones, then convert to Hz
      float semitoneShift = (midiNote - baseNote) * scaleFactor / 12.0f;
      return baseFreq * std::pow(2.0f, semitoneShift);
    }
    vector<SynthVoice> *process() {
      sustain_pedal_debouncer.process();
      // Process each voice

        // This process flag is set when a midi note is played
        // This process flag makes sure we don't update the voices while we are processing them
        if(_process_flag){
          _update_voices();

          _process_flag = false;
        }

      _synthvoices_to_remove.clear();
      for (auto &voice : _voices) {

        voice.process();


        // Apply filter keytracking
        auto new_filter_freq = KeyTrackedCutoff(voice.note, voice.filter_frequency, _base_note, _filter_keytrack);
        voice.voice_filter_node->set_filter_frequency(new_filter_freq);
        
        // Apply Filter Velocity Tracking
        new_filter_freq = VelocityTrackedCutoff(voice.velocity, new_filter_freq, _filter_velocity_track, 10.0);
        voice.voice_filter_node->set_filter_frequency(new_filter_freq);

        // Apply Filter Envelope if enabled
        if (_filter_env_enabled) {
          new_filter_freq = get_filter_modulation(new_filter_freq, voice.filter_envelope.getOutput());
          voice.voice_filter_node->set_filter_frequency(new_filter_freq);
        }

        // Remove voices that are done playing
        if (voice.amplitude_envelope.getState() == env_idle) {
            voice.stop();
            _synthvoices_to_remove.push_back(&voice);
        }

        // Increment the envelopes counter
        voice.process_adsr(1);

        // Increment the voice counter
        increment_counter(voice);
      }
      for (auto &voice : _synthvoices_to_remove) {
        _voices.erase(
            remove_if(_voices.begin(), _voices.end(),
                      [&](SynthVoice &v) { return v.id == voice->id; }),
            _voices.end());
      }
      return &_voices;
    }

    // Getters
    vector<SynthVoice> *get_voices(){
        return &_voices;
    }

    // Setters

    // Set the mode
    void set_mode(Modes mode) {
      _mode = mode;
    }

    // Set legato
    void set_legato(bool legato) { this->_legato = legato; }

    // Set Glide Time: Set the glide time in samples. This is the time it takes for the note to glide to the next note
    // In the user facing code, they will specify the glide time in milliseconds. Convert to samples before calling this method.
    // @param glide_time: The glide time in samples
    void set_glide_time(float glide_time) { 
      this->_glide_time = glide_time; 
    }

    // Set the polyphony
    void set_polyphony(u_int polyphony) { this->_polyphony = polyphony; }

    // Set the transpose offset
    void set_transpose_offset(int offset) {
      _global_transpose_offset = offset;
    }

    // Set the detune offset
    void set_detune_offset(float offset) {
      _global_detune_offset = offset;
    }

    // Set the base note
    void set_base_note(unsigned int note) {
      _base_note = note;
    }

    // Set the loop
    void set_loop(bool loop) { this->_loop = loop; }

    // Set the start sample buffer index
    void set_start(unsigned int start) { this->_start = start; }

    // Set the end sample buffer index
    void set_end(unsigned int end) { this->_end = end; }

    // Set the reverse. If true, the sample will play in reverse
    void set_reverse(bool reverse) { this->_reverse = reverse; }

    // Set the sample rate
    void set_sample_rate(unsigned int sample_rate) {
      this->_sample_rate = sample_rate;
    }

    // Set the amount of unison voices
    // @param unison_voices: The amount of unison voices
    void set_unison_voices(u_int unison_voices) {
      this->_unison_voices = unison_voices;
      this->_calculate_unison_offsets();
      this->_calculate_unison_pans();
    }

    // Set the unison detune
    // @param detune_ratio: The detune ratio
    void set_unison_detune(float detune_ratio) {
      this->_unison_detune_amount = detune_ratio;
      this->_calculate_unison_offsets();
      this->_calculate_unison_pans();
    }

    // Set the unison detune maximum amount
    void set_unison_detune_amount_max(float detune_amount_max) {
      this->_unison_detune_amount_max = detune_amount_max;
      this->_calculate_unison_offsets();
      this->_calculate_unison_pans();
    }

    // Set the unison pan amount
    void set_unison_pan_amount(float pan_amount) {
      this->_unison_pan_amount = pan_amount;
      this->_calculate_unison_offsets();
      this->_calculate_unison_pans();
    }

    void set_attck(float attack) {
      _attack = attack;
      for (auto &voice : _voices) {
        voice.amplitude_envelope.setAttackRate(attack * _sample_rate);
      }
    }

    void set_decay(float decay) {
      _decay = decay;
      for (auto &voice : _voices) {
        voice.amplitude_envelope.setDecayRate(decay * _sample_rate);
      }
    }

    void set_sustain(float sustain) {
      _sustain = sustain;
      for (auto &voice : _voices) {
        voice.amplitude_envelope.setSustainLevel(sustain);
      }
    }

    void set_release(float release) {
      _release = release;
      for (auto &voice : _voices) {
        voice.amplitude_envelope.setReleaseRate(release * _sample_rate);
      }
    }

    void set_filter_keytrack(float keytrack) {
      _filter_keytrack = keytrack;
    }

    void set_filter_velocity_track(float velocity_track) {
      _filter_velocity_track = velocity_track;
    }

    void set_filter_env_enabled(bool enabled) { _filter_env_enabled = enabled; }

    void set_filter_env_amount(float amount) {
      _filter_env_amount = amount;
      for (auto &voice : _voices) {
        voice.filter_envelope.setAmount(amount);
      }
    }

    void set_filter_env_attack(float attack) {
      _filter_env_attack = attack;
      for (auto &voice : _voices) {
        voice.filter_envelope.setAttackRate(attack);
      }
    }

    void set_filter_env_decay(float decay) {
      _filter_env_decay = decay;
      for (auto &voice : _voices) {
        voice.filter_envelope.setDecayRate(decay);
      }
    }

    void set_filter_env_sustain(float sustain) {
      _filter_env_sustain = sustain;
      for (auto &voice : _voices) {
        voice.filter_envelope.setSustainLevel(sustain);
      }
    }

    void set_filter_env_release(float release) {
      _filter_env_release = release;
      for (auto &voice : _voices) {
        voice.filter_envelope.setReleaseRate(release);
      }
    }

    void set_pitch_env_enabled(bool enabled) { _pitch_env_enabled = enabled; }

    void set_pitch_env_amount(float amount) {
      _filter_env_amount = amount;
      for (auto &voice : _voices) {
        voice.pitch_envelope.setAmount(amount);
      }
    }

    void set_pitch_env_attack(float attack) {
      _pitch_env_attack = attack;
      for (auto &voice : _voices) {
        voice.pitch_envelope.setAttackRate(attack);
      }
    }

    void set_pitch_env_decay(float decay) {
      _pitch_env_decay = decay;
      for (auto &voice : _voices) {
        voice.pitch_envelope.setDecayRate(decay);
      }
    }

    void set_pitch_env_sustain(float sustain) {
      _pitch_env_sustain = sustain;
      for (auto &voice : _voices) {
        voice.pitch_envelope.setSustainLevel(sustain);
      }
    }

    void set_pitch_env_release(float release) {
      _pitch_env_release = release;
      for (auto &voice : _voices) {
        voice.pitch_envelope.setReleaseRate(release);
      }
    }

    void set_filter_frequency(float frequency) {
      _filter_frequency = frequency;
      for (auto &voice : _voices) {
        voice.set_filter_frequency(_filter_frequency);
      }
    }

    void set_filter_resonance(float resonance) {
      _filter_resonance = resonance; 
      for (auto &voice : _voices) {
        voice.voice_filter_node->set_filter_resonance(resonance);
      }
    }

    void set_filter_peak_gain(float peak_gain) {
      for (auto &voice : _voices) {
        voice.voice_filter_node->set_peak_gain(peak_gain);
      }
    }

    void set_filter_type(FilterType type) {
      _filter_type = type;
      for (auto &voice : _voices) {
        voice.voice_filter_node->set_type(_filter_type);
      }
    }

    void set_filter_enabled(bool enabled) {
      for (auto &voice : _voices) {
        voice.voice_filter_node->set_enabled(enabled);
      }
    }

    void set_filter_db24(bool db24) {
      for (auto &voice : _voices) {
        voice.voice_filter_node->set_db24(db24);
      }
    }

    // Set the sustain pedal on or off
    void set_sustain_pedal(bool sustain_pedal) {

      // Set it only if it has changed
      if(sustain_pedal != _sustain_pedal){
        _sustain_pedal = sustain_pedal;
      }
      
      // Sustain pedal is released
      if (!_sustain_pedal) {

        // Remove all voices from voice_tracker that are not in note tracker
        _voices_to_remove.clear();
        vector<VoiceT> _voicesr_to_remove;
        for (auto it = _voice_tracker.begin(); it != _voice_tracker.end(); it++) {
          bool found = false;
          for (auto &note : _note_tracker) {
            if (it->note == note.note) {
              found = true;
              break;
            }
          }
          if (!found) {
            _voices_to_remove.push_back(&(*it));
            _voicesr_to_remove.push_back(*it);
          }
        }
        
        for(auto it = _voicesr_to_remove.begin(); it != _voicesr_to_remove.end(); it++){
          std::erase_if(_voice_tracker, [&](VoiceT &v) {
            return v.id == it->id;
          });
        }
      }
      _process_flag = true;
    }

    // Sends a note on message to the synth
    void audition(u_int note = 60, bool on = true){
        _audition_message_on = MidiMsg(0x90, note, 100);
        _audition_message_off = MidiMsg(0x80, note, 0);
        if(on){
          midi_input(_audition_message_on);
        }
        else{
          midi_input(_audition_message_off);
        }
    }
    vector<SynthVoice> _voices;

  protected:
    MidiMsg _audition_message_on = MidiMsg(0x90, 60, 100);
    MidiMsg _audition_message_off = MidiMsg(0x80, 60, 0);
    vector<NoteT> _note_tracker;
    vector<VoiceT> _voice_tracker;
    vector<VoiceT *> _voices_to_remove;
    vector<SynthVoice *> _synthvoices_to_remove;
    Debouncer<bool> sustain_pedal_debouncer;
    float _global_detune_offset = 0;
    int _global_transpose_offset = 0;
    bool _reverse;
    bool _legato = false;
    float _glide_time = 0;
    unsigned int _sample_rate = 44100;
    bool _sustain_pedal = false;
    Modes _mode = Modes::ADSR;
    u_int _end;
    u_int _start;
    bool _loop;
    bool _filter_env_enabled;
    bool _pitch_env_enabled;
    u_int _base_note = 60;
    u_int _polyphony;
    u_int _voice_age = 1;
    u_int _voice_id = 0;

    // Amplitude Envelope
    float _attack = 0.1;
    float _decay = 0.1;
    float _sustain = 0.5;
    float _release = 0.1;

    // Filter Envelope
    float _filter_keytrack = 0;
    float _filter_velocity_track = 0;
    FilterType _filter_type = FilterType::lowpass;
    float _filter_frequency = 20000;
    float _filter_resonance = 0;
    float _filter_env_amount = 0;
    float _filter_env_attack = 0.1;
    float _filter_env_decay = 0.1;
    float _filter_env_sustain = 0.5;
    float _filter_env_release = 0.1;

    // Pitch Envelope
    float _pitch_env_attack = 0.1;
    float _pitch_env_decay = 0.1;
    float _pitch_env_sustain = 0.5;
    float _pitch_env_release = 0.1;

    // Unison
    u_int _unison_voices = 1;
    float _unison_detune_amount = 1;
    float _unison_detune_amount_max = 2;
    float _unison_pan_amount = 0.5;
    vector<float> _unison_offsets;
    vector<float> _unison_pan_offsets;
    bool _process_flag = false;


    void _update_note_tracker(MidiMsg &msg){
        if (msg.isNoteOn()) 
        {
            _note_tracker.push_back({msg.getNoteNumber(), msg.getVelocity(), 0});
        }
        else if (msg.isNoteOff()) 
        {
            // remove note from note tracker
            for (auto it = _note_tracker.begin(); it != _note_tracker.end();
                it++) {
            if (it->note == msg.getNoteNumber()) {
                _note_tracker.erase(it);
                break;
            }
            }
            
        }
    };

    void _voice_tracker_adsr(MidiMsg &msg){
      if (msg.isNoteOn()) {
        if (get_active_voice_count() >= _polyphony) {
          // steal voice
          VoiceT *oldest_voice = _get_oldest_voice();

          if (oldest_voice != nullptr) {
            // If polyphony is 1, we're going to remove the current voice
            // and add the new one
            if (_polyphony == 1) {

              if (_legato) {
                oldest_voice->note = msg.getNoteNumber();
                oldest_voice->velocity = msg.getVelocity();
                oldest_voice->age = _voice_age;
              } else {
                _remove_from_voice_tracker_by_midi_note(oldest_voice->note);
                _voice_tracker.push_back({msg.getNoteNumber(),
                                          msg.getVelocity(), _voice_age,
                                          _voice_id});
                _voice_id++;
              }

            } else {
              oldest_voice->note = msg.getNoteNumber();
              oldest_voice->velocity = msg.getVelocity();
              oldest_voice->age = _voice_age;
            }

          } else {
            _voice_tracker.push_back({msg.getNoteNumber(), msg.getVelocity(),
                                      _voice_age, _voice_id});
            _voice_id++;
          }
        } else {
          _voice_tracker.push_back(
              {msg.getNoteNumber(), msg.getVelocity(), _voice_age, _voice_id});
          _voice_id++;
        }
      }

      // if legato is on, only update the oldest voice
      else if (msg.isNoteOff()) {
        // Only remove the note from the active notes if the sustain pedal is
        // not pressed
        if (!_sustain_pedal) {
          if (_polyphony == 1 && _legato) {
            VoiceT *oldest_voice = _get_oldest_voice();
            NoteT *youngest_note = _get_youngest_note();

            // If the note being release is the current note, retune
            if (oldest_voice != nullptr && youngest_note != nullptr) {
              if (oldest_voice->note == msg.getNoteNumber()) {
                oldest_voice->note = youngest_note->note;
                oldest_voice->velocity = youngest_note->velocity;
              }
            } else {
              _remove_from_voice_tracker_by_midi_note(msg.getNoteNumber());
            }
          } else {
            _remove_from_voice_tracker_by_midi_note(msg.getNoteNumber());
          }
        }
      }
    }

    void _voice_tracker_one_shot(MidiMsg &msg){

      // Hard set polyphone to 1
      int one_shot_polyphony = 1;
      if (msg.isNoteOn()) {
        if (get_active_voice_count() >= one_shot_polyphony) {
          // steal voice
          VoiceT *oldest_voice = _get_oldest_voice();

          if (oldest_voice != nullptr) {
            // If polyphony is 1, we're going to remove the current voice
            // and add the new one

            if (_legato) {
              oldest_voice->note = msg.getNoteNumber();
              oldest_voice->velocity = msg.getVelocity();
              oldest_voice->age = _voice_age;
            } 
            else {
              _remove_from_voice_tracker_by_midi_note(oldest_voice->note);
              _voice_tracker.push_back({msg.getNoteNumber(),
                                        msg.getVelocity(), _voice_age,
                                        _voice_id});
              _voice_id++;
            }

          } 
          else {
            _voice_tracker.push_back({msg.getNoteNumber(), msg.getVelocity(),
                                      _voice_age, _voice_id});
            _voice_id++;
          }
        } 
        else {
          _voice_tracker.push_back(
              {msg.getNoteNumber(), msg.getVelocity(), _voice_age, _voice_id});
          _voice_id++;
        }
      }

      // if legato is on, only update the oldest voice
      else if (msg.isNoteOff()) {
        // Only remove the note from the active notes if the sustain pedal is
        // not pressed
        if (!_sustain_pedal) {
          if (_legato) {
            VoiceT *oldest_voice = _get_oldest_voice();
            NoteT *youngest_note = _get_youngest_note();

            // If the note being release is the current note, retune
            if (oldest_voice != nullptr && youngest_note != nullptr) {
              if (oldest_voice->note == msg.getNoteNumber()) {
                oldest_voice->note = youngest_note->note;
                oldest_voice->velocity = youngest_note->velocity;
              }
            } 
          }
        }
      }
    }

    void _update_voice_tracker(MidiMsg &msg){
        switch(_mode){
            case Modes::ADSR:
                _voice_tracker_adsr(msg);
                break;
            case Modes::ONE_SHOT:
                _voice_tracker_one_shot(msg);
                break;
        }
    };

    VoiceT * _get_oldest_voice(){
        VoiceT * oldest_voice = nullptr;
        for(auto &voice : _voice_tracker){
          if (oldest_voice == nullptr) {
            oldest_voice = &voice;
          } 
          else {
            if (voice.age < oldest_voice->age) {
              oldest_voice = &voice;
            }
          }
        }
        return oldest_voice;
    }

    NoteT * _get_youngest_note(){
        NoteT * youngest_note = nullptr;
        for(auto &note : _note_tracker){
          if (youngest_note == nullptr) {
            youngest_note = &note;
          } 
          else {
            if (note.age >= youngest_note->age) {
              youngest_note = &note;
            }
          }
        }
        return youngest_note;
    }
    void _remove_from_voice_tracker_by_midi_note(u_int note){
      _voices_to_remove.clear();
      for (auto it = _voice_tracker.begin(); it != _voice_tracker.end(); it++) {
        if (it->note == note) {
          _voices_to_remove.push_back(&(*it));
        }
      }
      for (auto &voice : _voices_to_remove) {
        _voice_tracker.erase(
            remove_if(_voice_tracker.begin(), _voice_tracker.end(),
                      [&](VoiceT &v) { return v.id == voice->id; }),
            _voice_tracker.end());
      }
    }
    void trigger_voice(VoiceT &voice){
        // trigger voice
    }
    void set_voice_parameters(SynthVoice &voice){
        voice.set_filter_frequency(_filter_frequency);
        voice.set_filter_resonance(_filter_resonance);
        voice.voice_filter_node->set_type(_filter_type);
        voice.amplitude_envelope.setAttackRate(_attack * _sample_rate);
        voice.amplitude_envelope.setDecayRate(_decay * _sample_rate);
        voice.amplitude_envelope.setSustainLevel(_sustain);
        voice.amplitude_envelope.setReleaseRate(_release * _sample_rate);
        voice.filter_envelope.setAmount(_filter_env_amount);
        voice.filter_envelope.setAttackRate(_filter_env_attack);
        voice.filter_envelope.setDecayRate(_filter_env_decay);
        voice.filter_envelope.setSustainLevel(_filter_env_sustain);
        voice.filter_envelope.setReleaseRate(_filter_env_release);
        voice.pitch_envelope.setAttackRate(_pitch_env_attack);
        voice.pitch_envelope.setDecayRate(_pitch_env_decay);
        voice.pitch_envelope.setSustainLevel(_pitch_env_sustain);
        voice.pitch_envelope.setReleaseRate(_pitch_env_release);
    }

    void _update_voices_adrs(){
      // update voices

      // remove voices from _voices that are not in _voice_tracker
      for (auto &voice : _voices) {
        bool found = false;
        for (auto &v : _voice_tracker) {
          if (voice.id == v.id) {
            found = true;
            break;
          }
        }
        if (!found) {
          voice.set_adsr_gate(false);
        }
      }

      // add voices to _voices that are in _voice_tracker multiplied by unison
      // but not in _voices
      for (auto &voice : _voice_tracker) {
        bool found = false;
        for (auto &v : _voices) {
          if (voice.id == v.id) {
            found = true;
            break;
          }
        }
        if (!found) {
          for (int i = 0; i < _unison_voices; i++) {
            SynthVoice new_voice =
                SynthVoice(voice.note, voice.velocity, voice.id, _start);
            new_voice.unison_detune = _unison_offsets[i];
            new_voice.pan = _unison_pan_offsets[i];
            set_voice_parameters(new_voice);
            new_voice.set_adsr_gate(true);
            _voices.push_back(std::move(new_voice));
          }
        }
      }

      // update voices in _voices with _voice_tracker
      for (auto &voice : _voice_tracker) {
        for (auto &v : _voices) {
          if (voice.id == v.id && v.note != voice.note) {

            // Retune Note if Legato is true, otherwise, retrigger the note
            if (_legato) {
              v.retune_note((float)voice.note, _glide_time);
              v.age = voice.age;
            } else {
              v.retrigger(voice.note, voice.velocity, _start);
              v.age = voice.age;
            }
          }
        }
      }
    };

    void _update_voices_one_shot(){
      // update voices

      // remove voices from _voices that are not in _voice_tracker
      for (auto &voice : _voices) {
        bool found = false;
        for (auto &v : _voice_tracker) {
          if (voice.id == v.id) {
            found = true;
            break;
          }
        }
        if (!found) {
          voice.set_adsr_gate(false);
        }
      }

      // add voices to _voices that are in _voice_tracker multiplied by unison
      // but not in _voices
      for (auto &voice : _voice_tracker) {
        bool found = false;
        for (auto &v : _voices) {
          if (voice.id == v.id) {
            found = true;
            break;
          }
        }
        if (!found) {
          for (int i = 0; i < _unison_voices; i++) {
            SynthVoice new_voice =
                SynthVoice(voice.note, voice.velocity, voice.id, _start);
            new_voice.unison_detune = _unison_offsets[i];
            new_voice.pan = _unison_pan_offsets[i];
            set_voice_parameters(new_voice);
            new_voice.set_adsr_gate(true);
            _voices.push_back(std::move(new_voice));
          }
        }
      }

      // update voices in _voices with _voice_tracker
      for (auto &voice : _voice_tracker) {
        for (auto &v : _voices) {
          if (voice.id == v.id && v.note != voice.note) {

            // Retune Note if Legato is true, otherwise, retrigger the note
            if (_legato) {
              v.retune_note((float)voice.note, _glide_time);
              v.age = voice.age;
            } else {
              v.retrigger(voice.note, voice.velocity, _start);
              v.age = voice.age;
            }
          }
        }
      }
    }

    void _update_voices(){
        switch(_mode){
            case Modes::ADSR:
                _update_voices_adrs();
                break;
            case Modes::ONE_SHOT:
                _update_voices_one_shot();
                break;
        }
    };

    void increment_counter(SynthVoice &v) {

      // Calculate semitone difference and transposition
      float semitone_difference =
          ((float)v.note + _global_transpose_offset) - (float)_base_note;

      // Apply detune
      // TODO: Need to implement pitch envelope here. Possible modulate the
      // detune amount
      float rate = powf(2, (float)semitone_difference / 12);
      float detune_ratio = pow(2.0f, (float)_global_detune_offset / 1200.0f);
      float unison_detune = pow(2.0f, (float)v.unison_detune / 1200.0f);
      rate = detune_ratio * rate * unison_detune;

      // Reverse counter if reverse is enabled
      if (_reverse) {
        v.counter -= rate;
        if (v.counter <= _start) {
          v.counter = _end;
          if (!_loop) {
            v.stop();
          }
        }
      }

      // Increment counter
      else {
        v.counter += rate;
        if (v.counter >= _end) {
          v.counter = _start;
          if (!_loop) {
            v.stop();
          }
        }
      }
    }
    
    void _calculate_unison_offsets() {
      _unison_offsets.clear();
      float detune_range =
          float(_unison_detune_amount_max - (-_unison_detune_amount_max)) *
          float(_unison_detune_amount);
      float interval = detune_range / float(_unison_voices + 1);
      float start = 0 - float(detune_range) / 2;
      start += interval;

      for (int i = 0; i < _unison_voices; i++) {
        _unison_offsets.push_back(start);
        start += interval;
      }
    }

    void _calculate_unison_pans() {
      _unison_pan_offsets.clear();
      float pan_offset_range = 2.0f * float(_unison_pan_amount);
      float interval = pan_offset_range / float(_unison_voices + 1);
      float start = 0 - float(pan_offset_range) / 2;
      start += interval;
      for (int i = 0; i < _unison_voices; i++) {
        _unison_pan_offsets.push_back(start);
        start += interval;
      }
    }
    
};


#endif // !VOICEALLOCATOR_H