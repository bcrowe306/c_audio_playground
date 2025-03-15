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

void AudioNode::remove_input_node(Ptr &node)
{
    _is_dirty = true;
    _removal_queue.enqueue(node);
    
}

float *AudioNode::process(float *input_buffer, float * output_buffer, unsigned long frameCount, std::atomic<uint64_t> &cycle, AudioEngineContext &context)
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

        return output_buffer;
    }

    // Update the last process cycle
    lastProcessCycle = cycle.load();

    // reset processing samples
    processing_samples[0] = 0.0;
    processing_samples[1] = 0.0;

    // Process input nodes
    // Process Inputs
    
    auto i = 0;
    std::lock_guard<std::mutex> lock(mtx);
    for (auto &node : _input_nodes)
    {
        i++;
        if(node == nullptr){
            continue;
        }

        auto temp = node->process(input_buffer, output_buffer, frameCount, processCycle, context);
        processing_samples[0] += temp[0];
        processing_samples[1] += temp[1];
        
    }


    // Process this nodes audio. Sum it with the input nodes
    process_audio(input_buffer, output_buffer, frameCount, context);
    output_buffer[0] += processing_samples[0];
    output_buffer[1] += processing_samples[1];

    // Copy the processed samples to the output buffer
    // std::copy(std::begin(processing_samples), std::end(processing_samples), output_buffer);

    // Process audio chain using the output of this node
    for (auto &node : _chain_nodes) {
      node->process(output_buffer, output_buffer, frameCount,
                    processCycle, context);
    }

    // copy output buffer to cached samples
    std::copy(output_buffer, output_buffer + 2, cachedSamples.begin());

    return output_buffer;
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
{   std::cout << "Preparing Graph\n";
    std::lock_guard<std::mutex> lock(mtx);

    // Go through all input nodes and clean their graphs
    for(auto &node : _input_nodes)
    {
        node->prepare_graph();
    }

    // Remove nodes from the input nodes that are in the removal queue
    Ptr _removal_audio_node_ptr;
    while (_removal_queue.try_dequeue(_removal_audio_node_ptr))
    {
        _input_nodes.erase(std::remove(_input_nodes.begin(), _input_nodes.end(), _removal_audio_node_ptr), _input_nodes.end());
    }

    // Add nodes to the input nodes that are in the addition queue
    Ptr _addition_audio_node_ptr;
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
