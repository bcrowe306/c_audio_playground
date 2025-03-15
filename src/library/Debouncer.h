#pragma once
#ifndef DEBOUNCER_H
#define DEBOUNCER_H
#include <iostream>
#include <functional>
#include <cmath>

using namespace std;


template <typename T>
class Debouncer
{
public:
    Debouncer(u_int sample_rate = 44100, u_int debounce_time_ms = 50)
    {
        this->_sample_rate = sample_rate;
        this->_debounce_time_ms = debounce_time_ms;
        _samples_to_wait = (int)round(debounce_time_ms * sample_rate / 1000);
    }
    ~Debouncer() {};

    void process()
    {
        if (_waiting)
        {
            if (_sample_count >= _samples_to_wait)
            {
                _sample_count = 0;
                _waiting = false;
                callback(_value);
                cout << "samples_to_wait: " << _samples_to_wait << "\n";
            }
            _sample_count++;
        }
    };
    T get_state()
    {
        return _value;
    }

    void set_debounce_time_ms(u_int debounce_time_ms)
    {
        this->_debounce_time_ms = debounce_time_ms;
        _samples_to_wait = (int)debounce_time_ms * _sample_rate / 1000;
    }

    void set_sample_rate(u_int sample_rate)
    {
        this->_sample_rate = sample_rate;
        set_debounce_time_ms(_debounce_time_ms);
    }
    void update(T value)
    {

        if (value != this->_value)
        {
            this->_value = value;
            _sample_count = 0;
            _waiting = true;
        }
    }
    void on_change(std::function<void(T)> callback)
    {
        this->callback = callback;
    }

protected:
    T _value;
    bool _waiting = false;
    u_int _sample_rate = 44100;
    u_int _samples_to_wait = 441;
    u_int _sample_count = 0;
    u_int _debounce_time_ms = 50;
    std::function<void(bool)> callback;
};

#endif // !DEBOUNCER_H

