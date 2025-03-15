#pragma once
#ifndef PLAYHEAD_H
#define PLAYHEAD_H

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <any>
#include <unordered_map>
#include "library/Observer.h"
#include "library/Parameter.h"
#include "core/SongPosition.h"

static void print(std::string message){
    std::cout << message << std::endl;
}

enum class PlayheadEvents {
    GLOBAL_FRAME,
    FRAME,
    TICK,
    HALF_BEAT,
    BEAT,
    BAR,
    LAUNCH,
    STATE_CHANGE,
    BPM_CHANGE,
    PRECOUNT_BARS_CHANGE,
    TIME_SIGNATURE_NUMERATOR_CHANGE,
    TIME_SIGNATURE_DENOMINATOR_CHANGE,
    LAUNCH_QUANTIZATION_CHANGE,
};
enum class PlayheadState
{
    PRECOUNT,
    PLAYING,
    RECORDING,
    STOPPED,
};

enum class LaunchQuantization
{
    Bar8,
    Bar4,
    Bar2,
    Bar,
    Beat,
    Eighth,
    Sixteenth,
    Off,
};

static const std::unordered_map<LaunchQuantization, std::string> launch_quantization_map = {
    {LaunchQuantization::Bar8, "8 Bars"},
    {LaunchQuantization::Bar4, "4 Bars"},
    {LaunchQuantization::Bar2, "2 Bars"},
    {LaunchQuantization::Bar, "1 Bar"},
    {LaunchQuantization::Beat, "Beat"},
    {LaunchQuantization::Eighth, "1/8 Note"},
    {LaunchQuantization::Sixteenth, "1/16 Note"},
    {LaunchQuantization::Off, "Off"},
};

static const std::unordered_map<std::string, LaunchQuantization> launch_quantization_enum = {
    {"8 Bars", LaunchQuantization::Bar8},
    {"4 Bars", LaunchQuantization::Bar4},
    {"2 Bars", LaunchQuantization::Bar2},
    {"1 Bar", LaunchQuantization::Bar},
    {"Beat", LaunchQuantization::Beat},
    {"1/8 Note", LaunchQuantization::Eighth},
    {"1/16 Note", LaunchQuantization::Sixteenth},
    {"Off", LaunchQuantization::Off},
};

static const std::vector<std::string> launch_quantization_strings = {
    "8 Bars",
    "4 Bars",
    "2 Bars",
    "1 Bar",
    "Beat",
    "1/8 Note",
    "1/16 Note",
    "Off",
};

static const std::unordered_map<LaunchQuantization, float> launch_quantization_values = {
    {LaunchQuantization::Bar8, 8},
    {LaunchQuantization::Bar4,
     4},
    {LaunchQuantization::Bar2,  2},
    {LaunchQuantization::Bar,  1},
    {LaunchQuantization::Beat,  0.25},
    {LaunchQuantization::Eighth,  0.125},
    {LaunchQuantization::Sixteenth,  0.0625},
    {LaunchQuantization::Off,  0},
};

class Playhead : public ISubject, public IObserver
{
public:
    struct Events
    {
        inline static const std::string GLOBAL_FRAME = "GLOBAL_FRAME";
        inline static const std::string FRAME = "FRAME";
        inline static const std::string TICK = "TICK";
        inline static const std::string HALF_BEAT = "HALF_BEAT";
        inline static const std::string BEAT = "BEAT";
        inline static const std::string BAR = "BAR";
        inline static const std::string LAUNCH = "LAUNCH";
        inline static const std::string STATE_CHANGE = "STATE_CHANGE";
        inline static const std::string BPM_CHANGE = "BPM_CHANGE";
        inline static const std::string PRECOUNT_BARS_CHANGE = "PRECOUNT_BARS_CHANGE";
        inline static const std::string TIME_SIGNATURE_NUMERATOR_CHANGE = "TIME_SIGNATURE_NUMERATOR_CHANGE";
        inline static const std::string TIME_SIGNATURE_DENOMINATOR_CHANGE = "TIME_SIGNATURE_DENOMINATOR_CHANGE";
        inline static const std::string LAUNCH_QUANTIZATION_CHANGE = "LAUNCH_QUANTIZATION_CHANGE";
    };
    Playhead(u_int sample_rate = 44100){
        this->_sample_rate = sample_rate;
        this->_song_position.set_sample_rate(sample_rate);
        this->setup_parameters();
    };
    ~Playhead(){
        print("Playhead destroyed");
    };

    // Public Variables
    
    FloatParameter bpm = FloatParameter("bpm", 120, 30, 300, 1);
    IntParameter precount_bars = IntParameter("Precount Bars", 1, 0, 4, 1);
    IntParameter time_signature_numerator = IntParameter("Time Signature Numerator", 4, 1, 16, 1);
    IntParameter time_signature_denominator = IntParameter("Time Signature Denominator", 4, 1, 16, 1);
    EnumParameter launch_quantization = EnumParameter( "Launch Quantization",launch_quantization_strings,(int)LaunchQuantization::Bar);

    // Public Methods
    void play()
    {
        switch (_state)
        {
        case PlayheadState::PLAYING:
            // Do nothing
            break;
        case PlayheadState::RECORDING:
            set_state(PlayheadState::PLAYING);
            break;
        case PlayheadState::STOPPED:
            set_state(PlayheadState::PLAYING);
            break;
        case PlayheadState::PRECOUNT:
            set_state(PlayheadState::PLAYING);
            break;
        default:
            break;  
        }
    }
    void stop()
    {
        set_state(PlayheadState::STOPPED);
        if (_return_to_zero)
            {
                _song_position.set_frame(0);
            }
            _precount_position.set_frame(0);
    }
    void record()
    {
        switch (_state)
        {
        case PlayheadState::PLAYING:
            set_state(PlayheadState::RECORDING);
            break;
        case PlayheadState::RECORDING:
            // Do nothing
            break;
        case PlayheadState::STOPPED:
            if(precount_bars.get_value() > 0){
                set_state(PlayheadState::PRECOUNT);
            }
            else{
                set_state(PlayheadState::RECORDING);
            }
        default:
            break;
        }
    }
    void toggle_play()
    {
        switch (_state)
        {
        case PlayheadState::PLAYING:
            stop();
            break;
        case PlayheadState::RECORDING:
            play();
            break;
        case PlayheadState::STOPPED:
            play();
            break;
        case PlayheadState::PRECOUNT:
            stop();
            break;
        default:
            break;
        }
    }
    void toggle_record()
    {
        switch (_state)
        {
        case PlayheadState::PLAYING:
            record();
            break;
        case PlayheadState::RECORDING:
            play();
            break;
        case PlayheadState::STOPPED:
            record();
            break;
        case PlayheadState::PRECOUNT:
            // Do nothing
            break;
        default:
            break;
        }
    }

    bool is_playing() {
      return _state != PlayheadState::STOPPED;
    }

    bool is_recording() {
      return _state == PlayheadState::RECORDING || _state == PlayheadState::PRECOUNT;
    }

    void add_event_lister(std::function<void(PlayheadEvents)> listener)
    {
        playhead_event_listeners.push_back(listener);
    }

    std::string get_song_position_string()
    {
        return _song_position.get_song_position_string();
    }

    void process()
    {
        switch (_state)
        {
        case PlayheadState::PRECOUNT:
            precount_state_process();
            break;
        case PlayheadState::PLAYING:
            playing_state_process();
            break;
        case PlayheadState::RECORDING:
            recording_state_process();
            break;
        case PlayheadState::STOPPED:
            stopped_state_process();
            break;
        }
    };

protected:
    PlayheadState _state = PlayheadState::STOPPED;
    u_int _sample_rate;
    u_int64_t _global_frame_count = 0;
    bool _return_to_zero = true;
    SongPosition _song_position;
    SongPosition _precount_position;
    std::unordered_map<PlayheadState, std::string> playhead_state_strings = {
        {PlayheadState::PRECOUNT, "Precount"},
        {PlayheadState::PLAYING, "Playing"},
        {PlayheadState::RECORDING, "Recording"},
        {PlayheadState::STOPPED, "Stopped"},
    };

    // TODO: Temporary Event Registry until observer pattern is implemented in a way that suites my needs
    std::vector<std::function<void(PlayheadEvents)>> playhead_event_listeners;

    // Protected methods

    void notify_event_listeners(PlayheadEvents event)
    {
        for (auto listener : playhead_event_listeners)
        {
            listener(event);
        }
    };
    
    void set_state(PlayheadState state)
    {
        _state = state;
        notify_event_listeners(PlayheadEvents::STATE_CHANGE);
        notify(Playhead::Events::STATE_CHANGE, (int)state);
    }
    
    void is_launch()
    {
        if (launch_quantization.get_value() == (int)LaunchQuantization::Off)
        {
            return;
        }
        else{
            auto lq_int = launch_quantization.get_value();
            auto lq = (LaunchQuantization)lq_int;
            auto lq_value = launch_quantization_values.at(lq);
            if (_song_position.is_launch(lq_value)){
                notify_event_listeners(PlayheadEvents::LAUNCH);
                notify(Playhead::Events::LAUNCH, _song_position);
            }
        }
    }

    // Attach parameter change listeners
    void setup_parameters()
    {
        
        bpm.on_change = [&](float value)
        {
            _song_position.set_bpm(value);
            _precount_position.set_bpm(value);
            notify_event_listeners(PlayheadEvents::BPM_CHANGE);
            notify(Playhead::Events::BPM_CHANGE, value);

            
        };
        bpm.update();

        time_signature_numerator.on_change = [&](u_int value)
        {
            _song_position.set_time_signature_numerator(value);
            _precount_position.set_time_signature_numerator(value);
            notify_event_listeners(PlayheadEvents::TIME_SIGNATURE_NUMERATOR_CHANGE);
            notify(Playhead::Events::TIME_SIGNATURE_NUMERATOR_CHANGE, value);
        };
        time_signature_numerator.update();

        time_signature_denominator.on_change = [&](u_int value)
        {
            _song_position.set_time_signature_denominator(value);
            _precount_position.set_time_signature_denominator(value);
            notify_event_listeners(PlayheadEvents::TIME_SIGNATURE_DENOMINATOR_CHANGE);
            notify(Playhead::Events::TIME_SIGNATURE_DENOMINATOR_CHANGE, value);

        };
        time_signature_denominator.update();

        precount_bars.on_change = [&](u_int value)
        {
            notify_event_listeners(PlayheadEvents::PRECOUNT_BARS_CHANGE);
            notify(Playhead::Events::PRECOUNT_BARS_CHANGE, value);
        };
        precount_bars.update();

        launch_quantization.on_change = [&](int value)
        {
            notify_event_listeners(PlayheadEvents::LAUNCH_QUANTIZATION_CHANGE);
            notify(Playhead::Events::LAUNCH_QUANTIZATION_CHANGE, value);
        };
        launch_quantization.update();
    }

    // Process method for playing state
    void precount_state_process(){
        // If precount is finished, change state to recording
        if (_precount_position.is_bar_number(precount_bars.get_value()))
        {
            set_state(PlayheadState::RECORDING);
            _precount_position.set_frame(0);
            return;
        }

        // Check if it is time to send a tick event
        if (_precount_position.is_tick())
        {
            // Send a tick event
            // notify(PlayheadEvents::TICK);

            // Check if it is time to send a beat event
            process_tick(_precount_position);
        }
        // Send frame event
        // notify(PlayheadEvents::FRAME);

        // Increment song position frame
        _precount_position.increment_frame();

        // Send a global frame event
        // notify(PlayheadEvents::GLOBAL_FRAME);

        // Increment global frame count
        _global_frame_count++;
    }
    
    // Process method for playing state
    void playing_state_process(){

        // Tick(tpqn) logic processing
        process_tick(_song_position);

        // Send frame event
        // notify(PlayheadEvents::FRAME);

        // Increment song position frame
        _song_position.increment_frame();

        // Send a global frame event
        // notify(PlayheadEvents::FRAME);

        // Increment global frame count
        _global_frame_count++;
    }

    void recording_state_process(){
        // Tick(tpqn) logic processing
        process_tick(_song_position);

        // Send frame event
        // notify(PlayheadEvents::FRAME);

        // Increment song position frame
        _song_position.increment_frame();

        // Send a global frame event
        // notify(PlayheadEvents::FRAME);

        // Increment global frame count
        _global_frame_count++;
    }

    void process_tick(SongPosition song_position)
    {
        process_beat_event(song_position);
    }

    void process_beat_event(SongPosition song_position){
      if(song_position.is_half_beat()){

                if(song_position.is_bar()){
                // Send bar event
                // notify(PlayheadEvents::BAR);
                notify_event_listeners(PlayheadEvents::BAR);
                notify(Playhead::Events::BAR, song_position);
                }
                else if(song_position.is_beat()){
                // Send beat event
                // notify(PlayheadEvents::BEAT);
                notify_event_listeners(PlayheadEvents::BEAT);
                notify(Playhead::Events::BEAT, song_position);
                }
                else{   
                    // Send half beat event
                    // notify(PlayheadEvents::HALF_BEAT);
                    notify_event_listeners(PlayheadEvents::HALF_BEAT);
                    notify(Playhead::Events::HALF_BEAT, song_position);
                }
        }
    }
    void stopped_state_process(){
        _song_position.set_frame(0);
    }
    
};


#endif // !PLAYHEAD_H