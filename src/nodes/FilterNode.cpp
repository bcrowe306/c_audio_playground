#include "FilterNode.h"
#include "bw_lp1.h"
#include <array>
#include <iostream>



vector<string> FilterNode::filter_types = {"lowpass", "highpass",
                                                  "bandpass", "bandsweep"};
float get_filter_frequency();

FilterNode::FilterNode(unsigned int sample_rate)
    {
        set_filter_settings(5000, 1.0, 1);
        smoother.setSampleRate(sample_rate);

    }

    FilterNode::~FilterNode()
    {
    }
    void FilterNode::set_filter_settings(float frequency, float resonance, float peak_gain, u_int sample_rate)
    {
        _frequency = frequency;
        _resonance = resonance;
        _peak_gain = peak_gain;
        _sample_rate = sample_rate;
        calc_coeff();
    }
    FilterType FilterNode::get_type()
    {
        return _type;
    }
    float FilterNode::get_filter_frequency()
    {
        return _frequency;
    }

    float FilterNode::get_filter_resonance()
    {
        return _resonance;
    }

    float FilterNode::get_peak_gain()
    {
        return _peak_gain;
    }

    bool FilterNode::get_db24()
    {
        return db24;
    }

    bool FilterNode::get_enabled()
    {
        return _enabled;
    }

    void FilterNode::set_type(FilterType type)
    {
        _type = type;
    }

    void FilterNode::set_filter_frequency(float value)
    {
        smoother.setTarget(value);
    }
    void FilterNode::update_filter_frequency(float value)
    {
        _frequency = value;
        calc_coeff();
    }
    void FilterNode::set_filter_resonance(float value)
    {
        _resonance = value;
        calc_coeff();
    }

    void FilterNode::set_peak_gain(float value)
    {
        _peak_gain = value;
        calc_coeff();
    }
    void FilterNode::calc_coeff()
    {
        so_lpf_l.calculate_coeffs(_resonance, _frequency, _sample_rate);
        so_lpf_r.calculate_coeffs(_resonance, _frequency, _sample_rate);

        so_hpf_l.calculate_coeffs(_resonance, _frequency, _sample_rate);
        so_hpf_r.calculate_coeffs(_resonance, _frequency, _sample_rate);

        so_bpf_l.calculate_coeffs(_resonance, _frequency, _sample_rate);
        so_bpf_r.calculate_coeffs(_resonance, _frequency, _sample_rate);

        so_bsf_l.calculate_coeffs(_resonance, _frequency, _sample_rate);
        so_bsf_r.calculate_coeffs(_resonance, _frequency, _sample_rate);
    }
    void FilterNode::set_enabled(bool enabled)
    {
        _enabled = enabled;
    }
    void FilterNode::set_db24(bool db24)
    {
        this->db24 = db24;
    }
    void FilterNode::process_audio(float *input_buffer, float* output_buffer, unsigned long frameCount, AudioEngineContext &context)
    {
        smoother.process();
        if (smoother.isSmoothing())
        {
            update_filter_frequency(smoother.getValue());
        }
        if (_enabled)
        {
            switch (_type)
            {
            case FilterType::lowpass:
                output_buffer[0] = so_lpf_l.process(input_buffer[0]);
                output_buffer[1] = so_lpf_r.process(input_buffer[1]);
                break;
            case FilterType::highpass:
                output_buffer[0] = so_hpf_l.process(input_buffer[0]);
                output_buffer[1] = so_hpf_r.process(input_buffer[1]);
                break;
            case FilterType::bandpass:
                output_buffer[0] = so_bpf_l.process(input_buffer[0]);
                output_buffer[1] = so_bpf_r.process(input_buffer[1]);
                break;
            case FilterType::bandsweep:
                output_buffer[0] = so_bsf_l.process(input_buffer[0]);
                output_buffer[1] = so_bsf_r.process(input_buffer[1]);
                break;
            default:
                break;
            }

        }
    }
