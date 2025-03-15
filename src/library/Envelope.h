#pragma once
#include "ADSR.h"
#include "Observer.h"
#ifndef ENVELOPE_H
#define ENVELOPE_H

class Envelope : public ISubject, public IObserver
{
public:
    struct Events
    {
        inline static const std::string OUTPUT = "OUTPUT";
    };
    Envelope(unsigned int sample_rate);
    ~Envelope();

    void set_attack_rate_ms(float rate);
    void set_decay_rate_ms(float rate);
    void set_sustain_level_ms(float level);
    void set_release_rate_ms(float rate);
    void gate(bool on);
    void process();
    float get_output();
    void reset();

protected:
    ADSR _adsr;
    unsigned int _sample_rate;
    bool enabled = true;
};

#endif // !ENVELOPE_H
