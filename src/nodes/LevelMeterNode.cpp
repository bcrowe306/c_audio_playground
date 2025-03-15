#include "LevelMeterNode.h"
#include <iostream>

LevelMeterNode::LevelMeterNode(size_t bufferSize, float peakHoldTime,
                               float smoothingFactor, float sample_rate)
    : bufferL(bufferSize, 0.0f), bufferR(bufferSize, 0.0f),
      writeIndex(0), peakLevel{0.0f, 0.0f}, rmsLevel{0.0f, 0.0f},
      peakLinear{0.0f, 0.0f}, rmsLinear{0.0f, 0.0f}, peakHoldMs(peakHoldTime),
      smoothing(smoothingFactor), timeSinceLastPeak{0, 0}, lufsLevel(0.0f),
      stereoLevels()

{
  for (size_t i = 0; i < 2; ++i) {
    peakLevel[i].store(-std::numeric_limits<float>::infinity(),
                       std::memory_order_relaxed);
    rmsLevel[i].store(-std::numeric_limits<float>::infinity(),
                      std::memory_order_relaxed);
  }
    stereoLevels.peak = peakValues;
    stereoLevels.rms = rmsValues;
    stereoLevels.peakLinear = peakLinear;
    stereoLevels.rmsLinear = rmsLinear;
    stereoLevels.lufs = lufsLevel;
}

    void LevelMeterNode::process_audio(float *input_buffer, float *output_buffer, unsigned long frameCount, AudioEngineContext &context)  {
      auto leftSample = std::clamp(output_buffer[0], -1.0f, 1.0f);
      auto rightSample = std::clamp(output_buffer[1], -1.0f, 1.0f);

      leftSample = std::clamp(leftSample, -1.0f, 1.0f);
      rightSample = std::clamp(rightSample, -1.0f, 1.0f);

      bufferL[writeIndex] = leftSample;
      bufferR[writeIndex] = rightSample;
      writeIndex = (writeIndex + 1) % bufferL.size();

      // Update peak levels (linear)
      if (std::abs(leftSample) > peakLinear[0]) {
        peakLinear[0] = std::abs(leftSample);
        timeSinceLastPeak[0] = 0; // Reset hold timer
      }
      if (std::abs(rightSample) > peakLinear[1]) {
        peakLinear[1] = std::abs(rightSample);
        timeSinceLastPeak[1] = 0; // Reset hold timer
      }

      timeSinceLastPeak[0]++;
      timeSinceLastPeak[1]++;
    }

    // Compute RMS and Peak levels in dBFS
    void LevelMeterNode::computeLevels() {
      float sumSquaresL = 0.0f;
      float sumSquaresR = 0.0f;
      float maxSampleL = 0.0f;
      float maxSampleR = 0.0f;
      float loudnessSum = 0.0f;

      for (size_t i = 0; i < bufferL.size(); ++i) {
        float absL = std::abs(bufferL[i]);
        float absR = std::abs(bufferR[i]);

        sumSquaresL += bufferL[i] * bufferL[i];
        sumSquaresR += bufferR[i] * bufferR[i];

        maxSampleL = std::max(maxSampleL, absL);
        maxSampleR = std::max(maxSampleR, absR);

        loudnessSum += bufferL[i] * bufferL[i] +
                       bufferR[i] * bufferR[i]; // LUFS calculation
      }

      // Compute linear RMS
      float newRMSL = std::sqrt(sumSquaresL / bufferL.size());
      float newRMSR = std::sqrt(sumSquaresR / bufferR.size());

      // Apply smoothing (EMA)
      rmsLinear[0] = (1.0f - smoothing) * rmsLinear[0] + smoothing * newRMSL;
      rmsLinear[1] = (1.0f - smoothing) * rmsLinear[1] + smoothing * newRMSR;

      // Store values in dBFS
      rmsLevel[0].store(20.0f * std::log10(rmsLinear[0] + 1e-10f),
                        std::memory_order_relaxed);
      rmsLevel[1].store(20.0f * std::log10(rmsLinear[1] + 1e-10f),
                        std::memory_order_relaxed);

      // Peak hold logic
      if (timeSinceLastPeak[0] > peakHoldMs)
        peakLinear[0] *= 0.95f;
      if (timeSinceLastPeak[1] > peakHoldMs)
        peakLinear[1] *= 0.95f;

      peakLevel[0].store(20.0f * std::log10(peakLinear[0] + 1e-10f),
                         std::memory_order_relaxed);
      peakLevel[1].store(20.0f * std::log10(peakLinear[1] + 1e-10f),
                         std::memory_order_relaxed);

      // LUFS calculation (simplified, assumes equal energy per channel)
      lufsLevel =
          -0.691 +
          10.0f * std::log10(loudnessSum / (bufferL.size() * 2) + 1e-10f);

        stereoLevels.peak[0] = peakLevel[0].load(std::memory_order_relaxed);
        stereoLevels.peak[1] = peakLevel[1].load(std::memory_order_relaxed);
        stereoLevels.rms[0] = rmsLevel[0].load(std::memory_order_relaxed);
        stereoLevels.rms[1] = rmsLevel[1].load(std::memory_order_relaxed);
        stereoLevels.lufs = lufsLevel;

    }

    float *LevelMeterNode::getPeakdB() { return stereoLevels.peak; }
    float *LevelMeterNode::getRMSdB() { return stereoLevels.rms; }

    LevelMeterNode::StereoLevels LevelMeterNode::getStereoLevels() {
      return stereoLevels;
    }

    // Reset peak hold
    void LevelMeterNode::resetPeakHold() {
      peakLinear[0] = 0.0f;
      peakLinear[1] = 0.0f;
      peakLevel[0] = -INFINITY;
      peakLevel[1] = -INFINITY;
      timeSinceLastPeak[0] = 0;
      timeSinceLastPeak[1] = 0;
    }