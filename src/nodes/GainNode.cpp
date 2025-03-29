#include "GainNode.h"

GainNode::GainNode()
    {
        std::cout << "GainNode created\n";
        smoother.setTime(smoothing_time_ms);
    }
    GainNode::~GainNode()
    {
        std::cout << "GainNode destroyed\n";
    }
    void GainNode::process_audio(float *input_buffer, float * output_buffer, unsigned long frameCount, AudioEngineContext &context)
    {
        smoother.process();
        
        output_buffer[0] = input_buffer[0] * smoother.getValue();
        output_buffer[1] = input_buffer[1] * smoother.getValue();
    }
    void GainNode::set_gain(float gain, float time_ms)
    {
        smoother.setTime(time_ms);
        smoother.setTarget(gain);
    }
    float GainNode::get_gain()
    {
        return smoother.getValue();
    }

    void GainNode::set_gain_db(float gain_db, float time_ms)
    {
        smoother.setTime(time_ms);
        smoother.setTarget(db_to_linear(gain_db));
    }

    float GainNode::get_gain_db()
    {
        return linear_to_db(smoother.getValue());
    }

    float GainNode::get_smoothing_time_ms()
    {
        return smoothing_time_ms;
    }

    void GainNode::set_smoothing_time_ms(float time_ms)
    {
        smoothing_time_ms = time_ms;
        smoother.setTime(time_ms);
    }
