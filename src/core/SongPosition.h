#pragma once

#ifndef SONGPOSITION_H
#define SONGPOSITION_H

#include <string>
#include <iostream>

class SongPosition
{

public:
    SongPosition(float bpm = 120, u_int sample_rate = 44100, u_int tpqn = 960, u_int numerator = 4, u_int denominator = 4)
    {
        this->_bpm = bpm;
        this->_sample_rate = sample_rate;
        this->_tpqn = tpqn;
        this->_ts_numerator = numerator;
        this->_ts_denominator = denominator;
        this->update_song_position();
    };
    ~SongPosition() {};

    void reset()
    {
        _bar = 1;
        _beat = 1;
        _sixteenth = 1;
        _tick = 0;
        _current_frame = 0;
    }
    void set_bpm(float bpm)
    {
        this->_bpm = bpm;
        update_song_position();
    }
    void set_time_signature(u_int numerator, u_int denominator)
    {
        this->_ts_numerator = numerator;
        this->_ts_denominator = denominator;
        update_song_position();
    }
    void set_time_signature_numerator(u_int numerator)
    {
        this->_ts_numerator = numerator;
        update_song_position();
    }
    void set_time_signature_denominator(u_int denominator)
    {
        // contstrain denominator to 2 4 8 16
        if (denominator != 2 && denominator != 4 && denominator != 8 && denominator != 16)
        {
            denominator = 4;
        }
        this->_ts_denominator = denominator;
        update_song_position();
    }
    void set_tpqn(u_int tpqn)
    {
        this->_tpqn = tpqn;
        update_song_position();
    }
    void set_sample_rate(u_int sample_rate)
    {
        this->_sample_rate = sample_rate;
    }
    void set_frame(u_int frame)
    {
        this->_current_frame = frame;
        this->_current_tick = frame / get_samples_per_tick();
        update_song_position();
    }
    void set_song_position(u_int bar, u_int beat, u_int sixteenth)
    {
        // contrain bar to min value of 1
        if (bar < 1)
        {
            bar = 1;
        }
        // constrain beat to min value of 1 and max value of time signature numerator
        if (beat < 1)
        {
            beat = 1;
        }
        if (beat > _ts_numerator)
        {
            beat = _ts_numerator;
        }
        // constrain sixteenth to min value of 1 and max value of 4
        if (sixteenth < 1)
        {
            sixteenth = 1;
        }
        if (sixteenth > 4)
        {
            sixteenth = 4;
        }
        u_int ticks = (bar - 1) * get_tick_per_bar();
        ticks += (beat - 1) * get_tick_per_beat();
        ticks += (sixteenth - 1) * (_tpqn / 4);
        this->_current_frame = ticks * get_samples_per_tick();
        update_song_position();
    }
    u_int get_current_frame()
    {
        return _current_frame;
    }
    u_int get_current_tick()
    {
        return _current_tick;
    }

    std::string get_song_position_string()
    {
        return std::to_string(_bar) + ":" + std::to_string(_beat) + ":" + std::to_string(_sixteenth);
    }

    // Returns true if the current frame is a tick
    bool is_tick()
    {
        return _current_frame % get_samples_per_tick() == 0;
    }

    // Returns true if the current frame is a beat
    bool is_beat()
    {
        return _current_tick % get_tick_per_beat() == 0;
    }

    // Return true is the current frame is half a beat
    bool is_half_beat()
    {
        return _current_tick % (get_tick_per_beat() / 2) == 0;
    }

    // Returns true if the current frame is a bar
    bool is_bar()
    {
        return _current_tick % get_tick_per_bar() == 0;
    }

    bool is_bar_number(u_int bar_number)
    {
        return _current_tick % (get_tick_per_bar() * bar_number) == 0 && _current_tick != 0;
    }

    // is_launch (float launch_quantization)
    // returns true if the current frame is a launch quantization
    // @param launch_quantization: The quantization of the launch in bars can be from 1/16 to 8 bars
    // See launch_quantization_strings for the available quantizations
    bool is_launch(float launch_quantization)
    {
        return _current_tick % (u_int)(get_tick_per_bar() * launch_quantization) == 0;
    }

    // Increment the current frame by 1. This should be done for every audio frame processed
    void increment_frame()
    {
        if (is_tick())
        {
            _current_tick++;
        }
        _current_frame++;
        update_song_position();
    }
    void decrement_frame()
    {
        _current_frame--;
        update_song_position();
    }
    void update_song_position()
    {
        auto samples_per_tick = get_samples_per_tick();
        auto ticks_per_beat = get_tick_per_beat();
        auto current_tick = _current_tick;
        _tick = current_tick % _tpqn;
        _sixteenth = _tick / (_tpqn / 4) % 4 + 1;
        _beat = current_tick / ticks_per_beat % _ts_numerator + 1;
        _bar = current_tick / (ticks_per_beat * _ts_numerator) + 1;
    }

protected:
    u_int _bar = 1;
    u_int _beat = 1;
    u_int _sixteenth = 1;
    u_int _tick = 0;
    u_int _current_frame = 0;
    u_int _current_tick = 0;
    float _bpm = 120;
    u_int _ts_numerator = 4;
    u_int _ts_denominator = 4;
    u_int _tpqn = 960;
    u_int _sample_rate = 44100;

    u_int get_tick_per_beat()
    {
        return _tpqn * (4 / _ts_denominator);
    }
    u_int get_tick_per_bar()
    {
        return get_tick_per_beat() * _ts_numerator;
    }
    u_int get_samples_per_tick()
    {
        return (u_int)round((60.f / _bpm * (float)_sample_rate) / (float)_tpqn);
    }
};

#endif // !SONGPOSITION_H