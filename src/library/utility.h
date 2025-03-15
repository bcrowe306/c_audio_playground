#pragma once
#ifndef UTILITY_H
#define UTILITY_H

#include <cmath>
#include <iostream>

// Convert a linear value to decibels
double db_to_linear(double db);

// Convert a decibel value to linear
float linear_to_db(float linear);

// Lineear interpolation function for integers
int lerp(int a, int b, float t);

// Linear interpolation function for floats
float lerp(float a, float b, float t);

// Linear interpolation function for doubles
double lerp(double a, double b, double t);

// Linear interpolation function for long doubles

long double lerp(long double a, long double b, long double t);

// Linear interpolation function for unsigned integers
unsigned int lerp(unsigned int a, unsigned int b, float t);

// Map function for integers
int map(int x, int in_min, int in_max, int out_min, int out_max);

// Map function for floats
float map(float x, float in_min, float in_max, float out_min, float out_max);

float map_float(float x, float in_min, float in_max, float out_min, float out_max);

float weighted_map_float(float value, float value_min, float value_max, float scale_min, float scale_max, float mid_point, bool logarithmic = false);

// Scale midi value to unit value by sensitivity:
//    By default, this will return 0-127 in the range of 0-2 with a value of 64 being 1 and a sensitivity of 1
//    By adjusting the range and shift, you are able to adjust the output to your desired range.
//    This is useful for scaling midi velocity to amplitude attenuation by sensitivity. At a sensistivity of 1, the output will be linear. at a sensitivity of 0, the output will be constant.
// @param velocity: The value in the range of 0-127
// @param sensitivity: The sensitivity of the velocity in the range of 0-1
// @param range: The range of the sensitivity
// @param shift: The shift of the output
// @return The mapped linear amplitude attenuation for each sample of this voice
float scale_midi_to_unit(float value, float sensitivity, float range = 2, float shift = 1);

// constrain()
//
// Clips an input value to be between two end points
// x is the value to constrain; min_val and max_val are the range
template <typename T>
T constrain(T x, T min_val, T max_val)
{
    if (x < min_val)
        return min_val;
    if (x > max_val)
        return max_val;
    return x;
}

//
// max()
float max(float x, float y);

// min()
float min(float x, float y);

// Double precision version
double logarithmicMap(double value, double minValue, double maxValue, double minFreq = 20, double maxFreq = 20000);

// Single precision (float) version
float logarithmicMap_float(float value, float minValue, float maxValue, float minFreq = 20.0f, float maxFreq = 20000.0f);

#endif // !UTILITY_H

static inline float LogModulation(float x, float c = 9.0f) {
  return (x == 0.0f)
             ? 0.0f
             : std::copysign(std::log1p(c * std::fabs(x)) / std::log1p(c), x);
}

// Compute filter cutoff frequency with velocity tracking
// @param velocity: The velocity to track
// @param baseFreq: The base frequency to track from in Hz
// @param scaleAmount: The amount of scaling to apply in percentage. Values from 0 to 100
static float VelocityTrackedCutoff(float velocity, float baseFreq,
                          float scaleAmount, float power = 2.5f) {

    // Normalize velocity (0 to 1), then apply exponential shaping
    float normVelocity = velocity / 127.0f;

    // Scale from -1 to 1 for symmetric modulation
    float semitoneShift = (normVelocity * 2.0f - 1.0f) * (scaleAmount / 100.0f);
    float lower_range = baseFreq - 20.0f;
    float upper_range = 20000.0f - baseFreq;
    if(semitoneShift > 0){
        float new_freq = std::clamp<float>(
            baseFreq + (upper_range * semitoneShift),
            20.0f, 20000.0f);
        return new_freq;
    } else {
        float new_freq = std::clamp<float>(
            baseFreq + (lower_range * semitoneShift),
            20.0f, 20000.0f);
        return new_freq;
    }

  // Convert semitone shift to frequency
}

// Compute filter cutoff frequency with key tracking
// @param midiNote: The midi note to track
// @param baseFreq: The base frequency to track from in Hz
// @param baseNote: The base note to track from. This is midi note values
// @param scaleAmount: The amount of scaling to apply in percentage. Values from 0 to 100
static float KeyTrackedCutoff(float midiNote, float baseFreq,
                                         float baseNote, float scaleAmount) {
  // Convert scale percentage to factor
  float scaleFactor = scaleAmount / 100.0f;

  // Compute frequency shift in semitones, then convert to Hz
  float semitoneShift = (midiNote - baseNote) * scaleFactor / 12.0f;
  return baseFreq * std::pow(2.0f, semitoneShift);
}