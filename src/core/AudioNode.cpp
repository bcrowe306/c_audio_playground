#include <iostream>
#include "AudioNode.h"
#include <algorithm>


AudioNode::AudioNode() : cachedSamples(2, 0.0f)
{
    // std::cout << "AudioNode created\n";
}

AudioNode::~AudioNode()
{
    // std::cout << "AudioNode destroyed\n";
}

void AudioNode::add_input_node(Ptr node)
{
    _is_dirty = true;
    _addition_queue.enqueue(node);
}

void AudioNode::remove_input_node(Ptr node)
{
    _is_dirty = true;
    _removal_queue.enqueue(node);
    
}
void AudioNode::prepare(int sample_rate, int buffer_size)
{
    // Prepare the node for processing
    _sample_rate = sample_rate;
    _buffer_size = buffer_size;
    for (auto &node : _input_nodes)
    {
        if(node == nullptr){
            continue;
        }
        node->prepare(sample_rate, buffer_size);
    }
    for (auto &node : _chain_nodes)
    {
        if(node == nullptr){
            continue;
        }
        node->prepare(sample_rate, buffer_size);
    }
    for (auto &param : _parameters)
    {
        if(param == nullptr){
            continue;
        }
        param->prepare(sample_rate, buffer_size);
    }
}
void AudioNode::process(float *input_buffer, float * output_buffer, unsigned long frameCount, std::atomic<uint64_t> &cycle, AudioEngineContext &context)
{
    // Run process functions on attached parameters. This allows the the parameters to have timing based functionality like smoothing and ramping.
    for (auto &param : _parameters)
    {
        if(param == nullptr){
            continue;
        }
        param->process();
    }

    // Check if this node has already been processed this cycle
    if (lastProcessCycle > 0 && lastProcessCycle == cycle.load())
    {
        std::copy(cachedSamples.begin(), cachedSamples.begin() + 2, output_buffer);

    }

    // Update the last process cycle
    lastProcessCycle = cycle.load();

    // reset processing samples
    processing_samples[0] = 0.0;
    processing_samples[1] = 0.0;

    if(_is_source_node)
    {
        // Call the function to generate the audio samples into the output buffer
        generate_audio(processing_samples, frameCount, context);
        
    }
    else 
    {
        // Process input nodes, summing their output to the processing samples
        
        for (auto &node : _input_nodes)
        {
            if(node == nullptr){
                continue;
            }

            node->process(input_buffer, child_nodes_output_buffer, frameCount, processCycle, context);
            processing_samples[0] += child_nodes_output_buffer[0];
            processing_samples[1] += child_nodes_output_buffer[1];

            // Reset child nodes buffer to zeros
            child_nodes_output_buffer[0] = 0.0;
            child_nodes_output_buffer[1] = 0.0;
        }

    }

    processing_samples[0] += input_buffer[0];
    processing_samples[1] += input_buffer[1];

    
    process_audio(processing_samples, output_buffer, frameCount, context);

    // Process audio chain using the output of this node
    for (auto &node : _chain_nodes) {
        if(node == nullptr){
            continue;
        }
        // Process the node with the output buffer as input

        node->process(output_buffer, output_buffer, frameCount,
            processCycle, context);
        
    }

    // copy output buffer to cached samples
    std::copy(output_buffer, output_buffer + 2, cachedSamples.begin());

}

void AudioNode::set_is_source_node(bool is_source_node)
{
    _is_source_node = is_source_node;
}

bool AudioNode::is_source_node()
{
    return _is_source_node;
}

bool AudioNode::is_enabled()
{
    return _enabled;
}

void AudioNode::add_node_to_chain(Ptr node)
{
    _is_dirty = true;
    _chain_addition_queue.enqueue(node);
}

void AudioNode::remove_node_from_chain(Ptr node)
{   
    _is_dirty = true;
    _chain_removal_queue.enqueue(node);
}

void AudioNode::clear_chain()
{
    _chain_nodes.clear();
}

void AudioNode::set_enabled(bool enabled)
{
    _enabled = enabled;
}

int AudioNode::get_input_node_count()
{
    std::lock_guard<std::mutex> lock(mtx);
    return _input_nodes.size();
}
// Proccesses process audio for this node. This method should be overriden to implement custom functionality.

std::vector<AudioNode::Ptr> AudioNode::get_input_nodes()
{
    return _input_nodes;
}
void AudioNode::prepare_graph()
{   
    
    std::lock_guard<std::mutex> lock(mtx);

    // Go through all input nodes and clean their graphs
    for(auto &node : _input_nodes)
    {
        node->prepare_graph();
    }

    if (!_is_dirty) {
      return;
    }
    std::cout << "Preparing Graph\n";
    // Remove nodes from the input nodes that are in the removal queue
    Ptr _removal_audio_node_ptr;
    while (_removal_queue.try_dequeue(_removal_audio_node_ptr))
    {
        std::cout << "Removing Input Node\n";
        _input_nodes.erase(std::remove(_input_nodes.begin(), _input_nodes.end(), _removal_audio_node_ptr), _input_nodes.end());
    }

    // Add nodes to the input nodes that are in the addition queue
    Ptr _addition_audio_node_ptr;
    std::cout << name.c_str() <<  ": Queue Size: " << _addition_queue.size_approx() << std::endl;
    while (_addition_queue.try_dequeue(_addition_audio_node_ptr))
    {
        _input_nodes.emplace_back(_addition_audio_node_ptr);
        _addition_audio_node_ptr->prepare_graph();
    }


    // Remove nodes from the chain nodes that are in the removal queue
    Ptr _chain_removal_audio_node_ptr;
    while (_chain_removal_queue.try_dequeue(_chain_removal_audio_node_ptr))
    {
        _chain_nodes.erase(std::remove(_chain_nodes.begin(), _chain_nodes.end(), _chain_removal_audio_node_ptr), _chain_nodes.end());
    }

    // Add nodes to the chain nodes that are in the addition queue
    Ptr _chain_addition_audio_node_ptr;
    while (_chain_addition_queue.try_dequeue(_chain_addition_audio_node_ptr))
    {
        std::cout << "Adding Chain Node\n";
        _chain_nodes.emplace_back(_chain_addition_audio_node_ptr);
        _chain_addition_audio_node_ptr->prepare_graph();
    }
    _is_dirty = false;
}
