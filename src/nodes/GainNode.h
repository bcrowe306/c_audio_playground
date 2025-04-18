#pragma once
#include <iostream>
#include "core/AudioNode.h"
#include "library/utility.h"
#include "library/Smoother.h"

class GainNode : public AudioNode
{
public:
    GainNode();
    ~GainNode();
    
    void set_gain(float gain, float time_ms = 10.0);
    float get_gain();
    void set_gain_db(float gain_db, float time_ms = 10.0);
    float get_gain_db();
    float get_smoothing_time_ms();
    void set_smoothing_time_ms(float time_ms);

protected:
    float _gain = 1.0;
    Smoother smoother;
    float smoothing_time_ms = 10.0;
    void process_audio(float *input_buffer, float *output_buffer, unsigned long frameCount, AudioEngineContext &context) override;
};