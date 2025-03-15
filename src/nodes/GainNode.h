#pragma once
#include <iostream>
#include "core/AudioNode.h"
#include "library/utility.h"

class GainNode : public AudioNode
{
public:
    GainNode();
    ~GainNode();
    
    void set_gain(float gain);
    float get_gain();
    void set_gain_db(float gain_db);
    float get_gain_db();

protected:
    float _gain = 1.0;
    void process_audio(float *input_buffer, float *output_buffer, unsigned long frameCount, AudioEngineContext &context) override;
};