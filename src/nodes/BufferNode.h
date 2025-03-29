#pragma once
#include <iostream>
#include "AudioEngineContext.h"
#include "core/AudioNode.h"
#include "audio/choc_AudioFileFormat_WAV.h"
#include "library/Observer.h"
#include <memory>
#include <filesystem>
#include <string>
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace choc::audio;
using namespace choc::buffer;
using namespace std::filesystem;
using std::unique_ptr;
using std::string;
using std::vector;

struct AudioPeak {
  float min;
  float max;
};

struct AudioAverage {
  float meanAbs; // Mean absolute amplitude
  float rms;     // RMS amplitude
};

// TODO: This class needs to be more functional.
// Should implement file loading and error handling around files, and loading differnt types of files.
// Should implement bufferViews for the audio data and ways to access and calculate those views
// Should expose methods for accessing various samples of audio data.
// Should have helper methods to get stereo data in one call.
// Should be able to play itself, but also allow external retrieval of samples based on counters

class BufferNode : public AudioNode, public IObserver, public ISubject
{
public:

    struct Events {
      inline static const std::string STATE_CHANGE = "STATE_CHANGE";
    };
    struct States {
        inline static const std::string LOADING = "LOADING";
        inline static const std::string READY = "READY";
        inline static const std::string OFFLINE = "OFFLINE";
        inline static const std::string DISABLED = "DISABLED";
    };
    BufferNode(u_int sample_rate=44100);
    BufferNode(std::string file_path, u_int sample_rate=44100);
    ~BufferNode();

    // Listener for state changes
    std::function<void(string)> on_state_changed;
    // AudioNode methods

    // Load a sample from a file path
    void load_sample(std::string file_path);

    // Set the loop value
    void set_loop(bool value);

    // Set the start value
    void set_start(u_int value);

    // Set the end value
    void set_end(u_int value);

    // Set the sample rate
    void set_sample_rate(u_int value);

    // Set the interpolation value
    void set_interpolation(bool value);

    // Set the reverse value
    void set_reverse(bool value);

    // Set the offline value
    void set_offline(bool value);

    // Set the counter rate
    void set_counter_rate(float value);

    // Set the counter value
    void set_counter(float value);

    // Get the loop value
    bool get_loop();

    // Get the start value
    u_int get_start();

    // Get the end value
    u_int get_end();

    // Get the sample rate
    u_int get_sample_rate();

    // Get the interpolation value
    bool get_interpolation();

    std::string get_file_name();

    // Is the buffer offline
    bool is_offline();

    // Is the buffer loading
    // @return: True if the buffer is loading
    bool is_loading();

    // Get samples from the buffer. Returns reference. 
    // This is a facade for the audio_buffer.frames.getSample method
    // @param index: The read pointer for the buffer.
    // @param channel: The channel to read from
    // @return: A reference to the sample
    float & get_samples(u_int index, u_int channel);

    // Get a stereo interleaved pair of interpolated samples from the internal buffer.
    // @param read_pointer: The read pointer for the buffer. this can be a float value
    // @param buffer: A float * buffer to write the samples to. If none is provided, a new buffer is created and returned.
    // @return: A float * buffer of stereo interleaved samples
    float * get_interpolated_samples(float read_pointer, float *buffer = nullptr);

    // Get a stereo interleaved pair of samples from the internal buffer.
    // This does not use interpolation, as such the read_pointer is an unsigned integer value.
    // @param read_pointer: The read pointer for the buffer.
    // @param buffer: A float * buffer to write the samples to. If none is provided, a new buffer is created and returned.
    // @return: A float * buffer of stereo interleaved samples
    float * get_stereo_interleaved_samples(u_int read_pointer, float *buffer = nullptr);

    // Get the waveform of the buffer
    // @param start: The start index of the buffer
    // @param end: The end index of the buffer
    // @param pixel_width: The width of the waveform in pixels
    float *get_waveform(int &start, int &end, int &pixel_width);

    std::vector<AudioPeak> * get_audio_peaks(int start, int end, int pixel_width);

    vector<AudioAverage> * get_audio_averages(int start, int end, int pixel_width);

    // Trigger the sample
    void trigger();

    // Stop the sample
    void stop();

    // Reset the sample
    void reset();

    

protected:
    bool _offline = true;
    WAVAudioFileFormat<false> _wav = WAVAudioFileFormat<false>();
    BufferView<float, SeparateChannelLayout> _buffer_view;
    u_int _sample_rate = 44100;
    unique_ptr<AudioFileReader> _reader;
    AudioFileData _audio_buffer;
    string _file_name;
    string _file_path;
    vector<float> _waveform;
    vector<AudioPeak> _peaks;
    vector<AudioAverage> _averages;
    bool waveform_generated = false;
    u_int waveform_start = 0;
    u_int waveform_end = 0;


    float _counter = 0;
    bool _is_playing = false;
    bool _loop = false;
    u_int _start = 0;
    u_int _end = 0;
    u_int _length = 0;
    float _counter_rate = 1;
    bool _reverse = false;
    bool _interpolation = true;
    bool _loading = false;

    // Get the samples. returns one frame of float * buffer using the internal counter
    // This is used internally to get the samples for processing. Linear interpolation is used.
    float *get_samples_internal_counter(float *buffer);

    // Process the samples
    void generate_audio(float * buffer_to_fill, unsigned long frameCount, AudioEngineContext &context) override;

    void notify_state_change(std::string state){
        if(on_state_changed){
            on_state_changed(state);
        }
        notify(BufferNode::Events::STATE_CHANGE, state);
    }
};