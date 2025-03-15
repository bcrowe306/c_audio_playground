#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>
#include "containers/choc_SingleReaderSingleWriterFIFO.h"
#include "concurrentqueue.h"
#include "library/Parameter.h"
#include "core/AudioEngineContext.h"

// TODO: Need to redefine the process function signature to include
// AudioEngine metadata, sample_rate, buffer_size, input channels, output, channels, etc.
// Need to also include MidiMsg queue for midi input. This makes the most sense to have the midi input

class AudioNode
{
public:
    AudioNode();
    ~AudioNode();
    using Ptr = std::shared_ptr<AudioNode>;
    using UPtr = std::unique_ptr<AudioNode>;
    std::string name = "AudioNode";
    std::mutex mtx;

    // Add an input node to this node
    void add_input_node(Ptr node);

    // Remove an input node from this node
    void remove_input_node(Ptr &node);

    // Process 1 frame of audio samples with output buffer of samples. Return samples will be summed. Modified the buffer will override the already processed samples.
    float * process(float *input_buffer, float *output_buffer, unsigned long frameCount, std::atomic<uint64_t> &cycle, AudioEngineContext &context);

    // Add a node to the processing chain
    void add_node_to_chain(Ptr node);

    std::vector<Ptr> get_input_nodes();

    // Remove a node from the processing chain
    void remove_node_from_chain(Ptr node);

    // Clear the processing chain
    void clear_chain();

    // Enable or disable the node
    void set_enabled(bool enabled);

    int get_input_node_count();

    void prepare_graph();
    
    // This is the main method that should be overriden to implement custom functionality. 
    // This is where the audio processing should be implemented. 
    // Generative nodes should implement this method to generate audio samples.
    // Effect nodes should implement this method to process audio samples.
    // Modifying the buffer will override the already processed samples.
    virtual void process_audio(float *input_buffer, float *output_buffer, unsigned long frameCount, AudioEngineContext &context)
    {
        // std::copy(cachedSamples.begin(), cachedSamples.begin() + 2, buffer);
    }

    void attach_parameter(Parameter *parameter)
    {
        _parameters.push_back(parameter);
    }

protected:
    std::vector<Ptr> _chain_nodes;
    bool _is_dirty = false;
    std::vector<Ptr> _input_nodes;
    std::vector<float> cachedSamples;
    std::vector<Parameter *> _parameters;
    bool _enabled = true;
    std::atomic<uint64_t> processCycle;
    uint64_t lastProcessCycle = 0;
    float processing_samples[2] = {0.0, 0.0};
    moodycamel::ConcurrentQueue<Ptr> _removal_queue = moodycamel::ConcurrentQueue<Ptr>(32);
    moodycamel::ConcurrentQueue<Ptr> _addition_queue = moodycamel::ConcurrentQueue<Ptr>(32);
    moodycamel::ConcurrentQueue<Ptr> _chain_removal_queue = moodycamel::ConcurrentQueue<Ptr>(32);
    moodycamel::ConcurrentQueue<Ptr> _chain_addition_queue = moodycamel::ConcurrentQueue<Ptr>(32);



    // Proccesses process audio for this node. This method should be overriden to implement custom functionality.
};