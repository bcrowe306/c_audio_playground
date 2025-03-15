#include "GainNode.h"

GainNode::GainNode()
    {
        std::cout << "GainNode created\n";
    }
    GainNode::~GainNode()
    {
        std::cout << "GainNode destroyed\n";
    }
    void GainNode::process_audio(float *input_buffer, float * output_buffer, unsigned long frameCount, AudioEngineContext &context)
    {
        
        output_buffer[0] = input_buffer[0] * _gain;
        output_buffer[1] = input_buffer[1] * _gain;
        return;
    }
    void GainNode::set_gain(float gain)
    {
        _gain = gain;
    }
    float GainNode::get_gain()
    {
        return _gain;
    }
    void GainNode::set_gain_db(float gain_db)
    {
        _gain = db_to_linear(gain_db);
    }
    float GainNode::get_gain_db()
    {
        return linear_to_db(_gain);
    }
