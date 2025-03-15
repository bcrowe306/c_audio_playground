#pragma once
#include "MidiMsg.h"
#include "concurrentqueue.h"
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::shared_ptr;

class MidiNode{
public:
    using MPtr = std::shared_ptr<MidiNode>;
    string name = "";
    bool _is_source = false;
    bool _is_destination = false;

    // Constructor
    // @param name: string. Only necessary if the device is a source or destination.
    // @param is_source: bool. True if the device is a source.
    // @param is_destination: bool. True if the device is a destination.
    MidiNode(string name = "", bool is_source = false, bool is_destination = false);

    // Destructor
    ~MidiNode();

    // Called on incoming midi events. 
    // Can be overridden by subclasses to introduce custom midi processing.
    // By default, this method takes the incoming midi message and processes it if the device and channel match the filters.
    // It then passes the message to all child nodes for processing.
    // @param msg: MidiMsg. The incoming midi message.
    virtual void on_midi_in_event(MidiMsg &msg);

    // Adds a MidiNode as a child to this node. This means the child node will receive midi events from this node.
    // @param node: std::shared_ptr<MidiNode>. The node to add as a child.
    void add_child_node(MPtr node);

    // Removes a MidiNode as a child to this node. This means the child node will no longer receive midi events from this node.
    // @param node: std::shared_ptr<MidiNode>. The node to remove as a child.
    void remove_child_node(MPtr node);

    // Adds a MidiNode as an output to this node. This means the output node will receive midi events generated from this node.
    // @param node: std::shared_ptr<MidiNode>. The node to add as an output.
    void add_output_node(MPtr node);

    // Removes a MidiNode as an output to this node. This means the output node will no longer receive midi events generated from this node.
    // @param node: std::shared_ptr<MidiNode>. The node to remove as an output.
    void remove_output_node(MPtr node);

    // Sets the input device filter. If the device name of the incoming midi message does not match the filter, the message will not be processed.
    // To disable the filter, set the filter to "all" or "". This is the default.
    // @param device_name: string. The device name to filter for.
    void set_input_device_filter(string device_name);

    // Sets the input channel filter. If the channel of the incoming midi message does not match the filter, the message will not be processed.
    // To disable the filter, set the filter to -1. This is the default.
    // @param channel: int. The channel to filter for.
    void set_input_channel_filter(int channel);

protected:
    string _device_name;
    string _input_device_filter = "";
    int _input_channel_filter = -1;
    vector<MPtr> _children;
    vector<MPtr> _outputs;
    moodycamel::ConcurrentQueue<MPtr> _child_removal_queue = moodycamel::ConcurrentQueue<MPtr>(32);
    moodycamel::ConcurrentQueue<MPtr> _child_addition_queue = moodycamel::ConcurrentQueue<MPtr>(32);
    moodycamel::ConcurrentQueue<MPtr> _out_removal_queue = moodycamel::ConcurrentQueue<MPtr>(32);
    moodycamel::ConcurrentQueue<MPtr> _out_addition_queue = moodycamel::ConcurrentQueue<MPtr>(32);
    bool _is_processing = false;

    virtual void _process_midi(MidiMsg msg);
    virtual void _generate_midi(MidiMsg msg);
    bool _is_device_match(string device_name);
    bool _is_channel_match(int channel);
    void _process_child_addition_queue();
    void _process_child_removal_queue();
    void _process_out_addition_queue();
    void _process_out_removal_queue();
    void _process_queues();

};