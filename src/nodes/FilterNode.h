#pragma once
#include "AudioEngineContext.h"
#ifndef FILTERNODE_H
#define FILTERNODE_H

#include <iostream>
#include "core/AudioNode.h"
#include <vector>
#include <string>
#include "filter_common.h"
#include "filter_includes.h"
#include "library/Smoother.h"
using std::vector;
using std::string;



enum class FilterType { lowpass, highpass, bandpass, bandsweep };
class FilterNode : public AudioNode
{

private:
    bool db24 = false;

public:
     
    FilterNode(unsigned int sample_rate);
    ~FilterNode();
    static vector<string> filter_types;
    float get_filter_frequency();
    float get_filter_resonance();
    float get_peak_gain();
    bool get_enabled();
    FilterType get_type();
    void set_filter_frequency(float value);
   
    void set_filter_resonance(float value);
    void set_peak_gain(float value);
    void set_type(FilterType type);
    void set_enabled(bool enabled);
    void set_db24(bool db24);
    bool get_db24();
    void set_filter_settings(float frequency, float resonance, float peak_gain, u_int sample_rate = 44100);
    void process_audio(float *input_buffer, float * output_buffer, unsigned long frameCount, AudioEngineContext &context) override;
    

protected:
    Smoother smoother;
    SO_LPF so_lpf_l = SO_LPF();
    SO_LPF so_lpf_r = SO_LPF();

    SO_HPF so_hpf_l = SO_HPF();
    SO_HPF so_hpf_r = SO_HPF();

    SO_BPF so_bpf_l = SO_BPF();
    SO_BPF so_bpf_r = SO_BPF();

    SO_BSF so_bsf_l = SO_BSF();
    SO_BSF so_bsf_r = SO_BSF();

    float _frequency;
    float _resonance;
    float _peak_gain;
    u_int _sample_rate;
    FilterType _type = FilterType::lowpass;
    void calc_coeff();
    void update_filter_frequency(float value);
};

#endif // !FILTERNODE_H