
#pragma once
#include <iostream>
#include "core/AudioNode.h"
#include "library/utility.h"

static void process_pan(float *stereo_frame, float pan)
{
    float left_gain = 1.0;
    float right_gain = 1.0;

    if (pan < 0)
    {
        right_gain = 1.0 + pan;
    }
    else if (pan > 0)
    {
        left_gain = 1.0 - pan;
    }

    stereo_frame[0] = stereo_frame[0] * left_gain;
    stereo_frame[1] = stereo_frame[1] * right_gain;
}

class PanNode : public AudioNode
{
public:
    PanNode();
    ~PanNode();
    
    void set_pan(float pan);
    float get_pan();

protected:
    float _pan = 0.0;
    void process_audio(float *input_buffer, float * output_buffer, unsigned long frameCount, AudioEngineContext &context) override;
};