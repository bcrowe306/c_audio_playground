
#include "PanNode.h"
// Pan operation. Modifies the stereo frame in place

PanNode::PanNode(){
    std::cout << "PanNode created\n";
};
PanNode::~PanNode(){
    std::cout << "PanNode destroyed\n";
};
void PanNode::process_audio(float *input_buffer, float * output_buffer, unsigned long frameCount, AudioEngineContext &context)
{
    // Check if node is enabled
    if (!_enabled)
        return;


    // All is normal, process the input through the pan algorithm
    float left_gain = 1.0;
    float right_gain = 1.0;

    if (_pan < 0)
    {
        right_gain = 1.0 + _pan;
    }
    else if (_pan > 0)
    {
        left_gain = 1.0 - _pan;
    }

    output_buffer[0] = output_buffer[0] * left_gain;
    output_buffer[1] = output_buffer[1] * right_gain;
    return;
    };
    
    void PanNode::set_pan(float pan)
    {
        _pan = pan;
    };

    float PanNode::get_pan()
    {
        return _pan;
    };
