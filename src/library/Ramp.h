// Ramp.h: header file for defining a second order resonant kow pass filter

#pragma once
#ifndef RAMP_H
#define RAMP_H

#endif // !RAMP_H
class Ramp
{

public:
    // Constructor
    Ramp();

    // Constructor specifying a sample rate
    Ramp(float sampleRate);

    // Set the sample rate, used for all calculations
    void setSampleRate(float rate);

    // Jump to a value
    void setValue(float value);

    // Ramp to a value over a period of time
    void rampTo(float value, float time);

    // Generate and return the next ramp output
    float process();

    // Return whether the ramp is finished
    bool finished();

    // Destructor
    ~Ramp();

private:
    // State variables, not accessible to the outside world
    float sampleRate_;
    float currentValue_;
    float increment_;
    int counter_;
};