#include "AudioEngine.h"
#include <iostream>
using namespace std;

// TODO: Make the AudioEngine more robust. Allow for device selection, sample rate selection, buffer size selection, etc.
// Should be able open and close the stream multiple times on different devices without crashing.
// TODO: Need to figure out how to handle input channels and streams.
// TODO: Need some data structure to allow for the choosing and processing of multiple input channels.
// TODO: Need to figure out how to handle the output channels.
// TODO: Need some data structure to allow for the choosing and processing of multiple output channels.
AudioEngine::AudioEngine()
{
    
    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
        std::cerr << "Error initializing PortAudio: " << Pa_GetErrorText(err) << std::endl;
        return;
    }
    auto host_api_count = Pa_GetHostApiCount();
    for (int i = 0; i < host_api_count; i++)
    {
        auto host_api_info = Pa_GetHostApiInfo(i);
        std::cout << "Host API: " << host_api_info->name << std::endl;

        for (int j = 0; j < host_api_info->deviceCount - 1; j++)
        {
            auto device_info = Pa_GetDeviceInfo(Pa_HostApiDeviceIndexToDeviceIndex(i, j));
            std::cout << "    Device: " << device_info->name << std::endl;
            std::cout << "          Max Input Channels: " << device_info->maxInputChannels << std::endl;
            std::cout << "          Max Output Channels: " << device_info->maxOutputChannels << std::endl;
            std::cout << "          Default Sample Rate: " << device_info->defaultSampleRate << std::endl;
            std::cout << "          Default Low Input Latency: " << device_info->defaultLowInputLatency << std::endl;
            std::cout << "          Default Low Output Latency: " << device_info->defaultLowOutputLatency << std::endl;
            std::cout << "          Default High Input Latency: " << device_info->defaultHighInputLatency << std::endl;
        }
    }
}

AudioEngine::~AudioEngine()
{
    PaError err = Pa_Terminate();
    if (err != paNoError)
    {
        std::cerr << "Error terminating PortAudio: " << Pa_GetErrorText(err) << std::endl;
        return;
    }
}

void AudioEngine::open_stream()
{
    PaError err = Pa_OpenDefaultStream(&stream, engine_context.input_channels, engine_context.output_channels, paFloat32, engine_context.sample_rate, engine_context.frames_per_buffer, &AudioEngine::paCallback, this);
    if (err != paNoError)
    {
        std::cerr << "Error opening default stream: " << Pa_GetErrorText(err) << std::endl;
        return;
    }
}

void AudioEngine::start_stream()
{
    PaError err = Pa_StartStream(stream);
    if (err != paNoError)
    {
        std::cerr << "Error starting stream: " << Pa_GetErrorText(err) << std::endl;
        return;
    }
}

void AudioEngine::stop_stream()
{
    PaError err = Pa_StopStream(stream);
    if (err != paNoError)
    {
        std::cerr << "Error stopping stream: " << Pa_GetErrorText(err) << std::endl;
        return;
    }
}

void AudioEngine::close_stream()
{
    PaError err = Pa_CloseStream(stream);
    if (err != paNoError)
    {
        std::cerr << "Error closing stream: " << Pa_GetErrorText(err) << std::endl;
        return;
    }
}

void AudioEngine::setStream(PaStream *stream)
{
    this->stream = stream;
}


void AudioEngine::set_sample_rate(int sample_rate)
{
    this->engine_context.sample_rate = sample_rate;
    dispatcher.reset(sample_rate);
}

void AudioEngine::set_output_channels(int output_channels)
{
    this->engine_context.output_channels = output_channels;
}

void AudioEngine::set_input_channels(int input_channels)
{
    this->engine_context.input_channels = input_channels;
}   

void AudioEngine::set_buffer_size(int buffer_size)
{
    this->engine_context.frames_per_buffer = buffer_size;
}   

void AudioEngine::start()
{
    open_stream();
    start_stream();
}

void AudioEngine::stop()
{
    stop_stream();
    close_stream();
}

int AudioEngine::paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData)
{
    /* Cast data passed through stream to our structure. */
    AudioEngine *audio_engine = (AudioEngine *)userData;

    // Prepare the graph for processing only if it is dirty
    audio_engine->prepare_graph();
    
    float *out = (float *)outputBuffer;
    unsigned int i;
    (void)inputBuffer; /* Prevent unused variable warning. */

    for (i = 0; i < framesPerBuffer; i++)
    {
        
        audio_engine->reset_output_buffer();
        audio_engine->reset_input_buffer();
        if(audio_engine->playhead != nullptr){
            audio_engine->playhead->process();
        }
        audio_engine->process(audio_engine->input_buffer,
                              audio_engine->output_buffer, 1,
                              audio_engine->processCycle, audio_engine->engine_context);
        audio_engine->global_frame_count++;
        *out++ = audio_engine->output_buffer[0];
        *out++ = audio_engine->output_buffer[1];
        audio_engine->processCycle++;
    }
    return 0;
}
void AudioEngine::reset_output_buffer()
{
    output_buffer[0] = 0.0;
    output_buffer[1] = 0.0;
}
void AudioEngine::reset_input_buffer()
{
    input_buffer[0] = 0.0;
    input_buffer[1] = 0.0;
}
int AudioEngine::get_sample_rate()
{
    return sample_rate;
}

int AudioEngine::get_output_channels()
{
    return output_channels;
}   

int AudioEngine::get_input_channels()
{
    return input_channels;
}

int AudioEngine::get_buffer_size()
{
    return buffer_size;
}

void AudioEngine::set_playhead(std::shared_ptr<Playhead> playhead)
{
    this->playhead = playhead;
    
}