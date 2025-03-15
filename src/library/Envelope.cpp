#include "Envelope.h"

Envelope::Envelope(unsigned int sample_rate)
    {
        _sample_rate = sample_rate;

        set_attack_rate_ms(1);
        set_decay_rate_ms(5000);
        set_sustain_level_ms(1);
        set_release_rate_ms(10);
    };
    Envelope::~Envelope(){
        
    };

    void Envelope::set_attack_rate_ms(float rate)
    {
        _adsr.setAttackRate(rate * _sample_rate);
    };

    void Envelope::set_decay_rate_ms(float rate)
    {
        _adsr.setDecayRate(rate * _sample_rate);
    };

    void Envelope::set_sustain_level_ms(float level)
    {
        _adsr.setSustainLevel(level);
    };

    void Envelope::set_release_rate_ms(float rate)
    {
        _adsr.setReleaseRate(rate * _sample_rate);
    };
    void Envelope::gate(bool on)
    {
        if(on){
            _adsr.gate(1);
        } else {
            _adsr.gate(0);
        }
    };
    void Envelope::process()
    {
        if(enabled){
            _adsr.process();
            notify(Envelope::Events::OUTPUT, _adsr.getOutput());
        }
    };

    float Envelope::get_output()
    {
        return _adsr.getOutput();
    };

    void Envelope::reset()
    {
        _adsr.reset();
    };

