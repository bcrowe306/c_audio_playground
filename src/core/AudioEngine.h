#include "portaudio.h"
#include <vector>
#include <memory>
#include "AudioNode.h"
#include "core/AudioEngineContext.h"
#include "Playhead.h"
#include "audio/choc_AudioMIDIBlockDispatcher.h"

class AudioEngine: public AudioNode
{
private:
    /* data */
public:
    AudioEngine(/* args */);
    ~AudioEngine();
    AudioEngineContext engine_context;
    choc::audio::AudioMIDIBlockDispatcher dispatcher;
    long global_frame_count;
    void reset_output_buffer();
    void reset_input_buffer();
    void open_stream();
    void start_stream();
    void stop_stream();
    void close_stream();
    void setStream(PaStream *stream);
    void set_sample_rate(int sample_rate);
    void set_output_channels(int output_channels);
    void set_input_channels(int input_channels);
    void set_buffer_size(int buffer_size);
    void start();
    void stop();
    void set_playhead(std::shared_ptr<Playhead> playhead);
    int get_sample_rate();
    int get_output_channels();
    int get_input_channels();
    int get_buffer_size();
    static int paCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData);

    // Variables
    PaStream *stream;
    int sample_rate = 44100;
    int output_channels = 2;
    int input_channels = 0;
    int buffer_size = 64;
    std::shared_ptr<Playhead> playhead;
    std::atomic<uint64_t> processCycle;
    float output_buffer[2] = { 0.0, 0.0 };
    float input_buffer[2] = { 0.0, 0.0 };
};