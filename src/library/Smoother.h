#pragma once
#ifndef SMOOTHER_H
#define SMOOTHER_H

#include <cmath> // For std::abs
#include <limits>

#include <cmath>  // For std::abs
#include <limits> // For std::numeric_limits<float>


// A simple class for smoothing values over time
// The smoothing is linear and can be set to a target value over a specified time
// The time can be set in milliseconds or samples
// The sample rate can be set to the desired sample rate
// To use, call setTarget() to set the target value and then call process() to update the current value
// The current value can be retrieved with getValue()
// The smoothing status can be checked with isSmoothing()
class Smoother {
public:
  Smoother(float sampleRate = 44100.0f, float timeMs = 15.0f)
      : sampleRate(sampleRate), timeMs(timeMs), currentValue(0.0f),
        targetValue(0.0f), step(0.0f), samplesRemaining(0), smoothing(false) {
    updateStep();
  }

  // Set the target value for the smoother
  // @param newValue: The target value
  void setTarget(float newValue) {
    if (newValue != targetValue) { // Only update if there's a change
      targetValue = newValue;
      updateStep();
    }
  }

  // Set the time in milliseconds for the smoothing to reach the target value
  // A time of 0 will snap to the target instantly
  // @param newTimeMs: The time in milliseconds
  void setTime(float newTimeMs) {
    timeMs = newTimeMs;
    updateStep();
  }

  // Set the time in samples for the smoothing to reach the target value
  // A time of 0 will snap to the target instantly
  // @param newTimeSamples: The time in samples
  void setTimeSamples(int newTimeSamples) {
    timeMs = static_cast<float>(newTimeSamples) / sampleRate * 1000.0f;
    updateStep();
  }


  // Set the sample rate for the smoother
  // @param newSampleRate: The sample rate in Hz
  void setSampleRate(float newSampleRate) {
    sampleRate = newSampleRate;
    updateStep();
  }

  void process() {
    if (samplesRemaining > 0) {
      currentValue += step;
      --samplesRemaining;
      if (samplesRemaining == 0) {
        currentValue = targetValue; // Snap to target at end
        smoothing = false;
      }
    }
  }

  float getValue() const { return currentValue; }

  bool isSmoothing() const { return smoothing; }

  // Set the initial value of the smoother. This sets the current value and target value to the specified value
  // @param value: The value to set the current and target values to
  void set_initial_value(float value) {
    currentValue = value;
    targetValue = value;
  } 

private:
  float sampleRate;
  float timeMs;
  float currentValue;
  float targetValue;
  float step;
  int samplesRemaining;
  bool smoothing;

  void updateStep() {
    int totalSamples = static_cast<int>((timeMs / 1000.0f) * sampleRate);
    samplesRemaining = totalSamples;
    smoothing = (samplesRemaining > 0);
    if (smoothing) {
      step = (targetValue - currentValue) / samplesRemaining;
    } else {
      step = targetValue - currentValue;
      currentValue = targetValue; // If no smoothing needed, snap instantly
    }
  }
};

#endif // !SMOOTHER_H