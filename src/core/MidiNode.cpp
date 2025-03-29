#include "MidiNode.h"
#include <string>


MidiNode::MidiNode(string name, bool is_source, bool is_destination){
    this->name = name;
    _is_source = is_source;
    _is_destination = is_destination;
    std::cout << "MidiNode created\n";
}

MidiNode::~MidiNode()
{
    std::cout << "MidiNode destroyed\n";
}

bool MidiNode::_is_device_match(string device_name)
{
    return _device_name == device_name || _input_device_filter == "" || _input_device_filter == "all";
}

bool MidiNode::_is_channel_match(int channel)
{
    return _input_channel_filter == -1 || _input_channel_filter == channel;
}

void MidiNode::on_midi_in_event(MidiMsg &msg)
{
    // Run this nodes midi processing if the device and channel match
    auto device = msg.getDeviceName();
    // get optional from msg.device

    if (_is_device_match(device) && _is_channel_match(msg.getChannel0to15()))
      _process_midi(msg);

    // Process the midi for all children. 
    // Set the is_processing flag to prevent modifying the children vector while it is being processed
    _is_processing = true;
    for(auto &node : _children){
        node->on_midi_in_event(msg);
    }
    _is_processing = false;
    _process_queues();
}

void MidiNode::add_child_node(MPtr node)
{
    if(_is_processing){
        _child_addition_queue.enqueue(node);
    }else{
        _children.push_back(node);
    }
}

void MidiNode::remove_child_node(MPtr node)
{
    if(_is_processing){
        _child_removal_queue.enqueue(node);
    }else{
        _children.erase(std::remove(_children.begin(), _children.end(), node), _children.end());
    }
}

void MidiNode::add_output_node(MPtr node)
{
    if(_is_processing){
        _out_addition_queue.enqueue(node);
    }else{
        _outputs.push_back(node);
    }
}

void MidiNode::remove_output_node(MPtr node)
{
    if(_is_processing){
        _out_removal_queue.enqueue(node);
    }else{
        _outputs.erase(std::remove(_outputs.begin(), _outputs.end(), node), _outputs.end());
    }
}

void MidiNode::set_input_device_filter(string device_name)
{
    _input_device_filter = device_name;
}

void MidiNode::set_input_channel_filter(int channel)
{
    _input_channel_filter = channel;
}

void MidiNode::_process_child_addition_queue()
{
    MPtr node;
    while(_child_addition_queue.try_dequeue(node)){
        _children.push_back(node);
    }
}

void MidiNode::_process_child_removal_queue()
{
    MPtr node;
    while(_child_removal_queue.try_dequeue(node)){
        _children.erase(std::remove(_children.begin(), _children.end(), node), _children.end());
    }
}

void MidiNode::_process_out_addition_queue()
{
    MPtr node;
    while(_out_addition_queue.try_dequeue(node)){
        _outputs.push_back(node);
    }
}

void MidiNode::_process_out_removal_queue()
{
    MPtr node;
    while(_out_removal_queue.try_dequeue(node)){
        _outputs.erase(std::remove(_outputs.begin(), _outputs.end(), node), _outputs.end());
    }
}

void MidiNode::_process_queues()
{
    _process_child_addition_queue();
    _process_child_removal_queue();
    _process_out_addition_queue();
    _process_out_removal_queue();
}

void MidiNode::_process_midi(MidiMsg msg)
{
    std::cout << "MidiNode processing message\n";
}

void MidiNode::_generate_midi(MidiMsg msg)
{
    _is_processing = true;
    for(auto &node : _outputs){
        node->on_midi_in_event(msg);
    }

    for(auto &node : _children){
        node->on_midi_in_event(msg);
    }
    _is_processing = false;
    _process_queues();
}

void MidiNode::set_is_source(bool is_source)
{
    _is_source = is_source;
}

void MidiNode::set_is_destination(bool is_destination)
{
    _is_destination = is_destination;
}