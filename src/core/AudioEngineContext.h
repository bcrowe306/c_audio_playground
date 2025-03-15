#pragma once
#ifndef AUDIO_ENGINE_CONTEXT_H
#define AUDIO_ENGINE_CONTEXT_H
struct AudioEngineContext {

  unsigned long frames_per_buffer = 128;
  unsigned long frame_count = 1;
  unsigned long sample_rate = 44100;
  unsigned long input_channels = 0;
  unsigned long output_channels = 2;
};
#endif // !AUDIO_ENGINE_CONTEXT_H