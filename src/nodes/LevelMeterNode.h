#pragma once

#ifndef LEVELMETERNODE_H
#define LEVELMETERNODE_H

#include "core/AudioNode.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>
#include "containers/choc_SingleReaderSingleWriterFIFO.h"

class LevelMeterNode : public AudioNode {

  public:
    struct StereoLevels {
      float *peak;
      float *rms;
      float *peakLinear;
      float *rmsLinear;
      float lufs;
    };

    LevelMeterNode(size_t bufferSize = 128, float peakHoldTime = 500.0f,
                   float smoothingFactor = 0.2f, float sample_rate = 44100);

    // Process stereo samples (call this in your audio callback)
    void process_audio(float *input_buffer, float *output_buffer, unsigned long frameCount, AudioEngineContext &context) override ;

    // Compute RMS and Peak levels in dBFS
    void computeLevels() ;
    float* getPeakdB() ;
    float* getRMSdB() ;
    StereoLevels getStereoLevels() ;
    void resetPeakHold() ;

private:
  std::vector<float> bufferL;
  std::vector<float> bufferR;
  size_t writeIndex;
  std::atomic<float> peakLevel[2];
  std::atomic<float> rmsLevel[2];

  float peakLinear[2] = {0.0f, 0.0f};
  float rmsLinear[2] = {0.0f, 0.0f};
  float peakValues[2] = {0.0f, 0.0f};
  float rmsValues[2] = {0.0f, 0.0f};

  float lufsLevel;
  float peakHoldMs;
  float smoothing;
  int timeSinceLastPeak[2];
  StereoLevels stereoLevels;
};

#endif // LEVELMETERNODE_H