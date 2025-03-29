#include "MidiRouter.h"
#include <memory>

MidiRouter::MidiRouter(shared_ptr<MidiEngine> engine) {
  _engine = engine;
  update_ports();
}

MidiRouter::~MidiRouter() {}

void MidiRouter::add_node(MidiNode::MPtr node) {
    _add_child_node(node);
  if (node->_is_source) {
    _sources.emplace_back(node);
  }
  if (node->_is_destination) {
    _destinations.push_back(node);
  }
}

void MidiRouter::process(MidiMsg &event) {
  _is_processing = true;
  for (auto &child : _children) {
    child->on_midi_in_event(event);
  }
  _is_processing = false;
  _process_child_addition_queue();
  _process_child_removal_queue();
}

// TODO: May need to add locks and mutexes to protect against vector access while modifying.
void MidiRouter::remove_node(MidiNode::MPtr node) {
  _remove_child_node(node);
  _sources.erase(std::remove(_sources.begin(), _sources.end(), node), _sources.end());
  _destinations.erase(std::remove(_destinations.begin(), _destinations.end(), node), _destinations.end());
}

void MidiRouter::_add_child_node(MPtr node) {
  if (_is_processing) {
    _child_addition_queue.enqueue(node);
  } else {
    _children.push_back(node);
  }
}

void MidiRouter::_remove_child_node(MPtr node) {
  if (_is_processing) {
    _child_removal_queue.enqueue(node);
  } else {
    std:;cout << "Removing node: " << node->name.c_str() << std::endl;
    _children.erase(std::remove(_children.begin(), _children.end(), node), _children.end());
  }
}

void MidiRouter::_process_child_addition_queue() {
  MPtr node;
  while (_child_addition_queue.try_dequeue(node)) {
    _children.push_back(node);
  }
}

void MidiRouter::_process_child_removal_queue() {
  MPtr node;
  while (_child_removal_queue.try_dequeue(node)) {
    _children.erase(std::remove(_children.begin(), _children.end(), node), _children.end());
  }
}
void MidiRouter::update_ports() {
  // Update the input and output ports from the engine.
  // Build a list of input ports from the engine map.
  for(auto const& [key, val] : _engine->inputDevices) {
    _input_ports.push_back(key);
  }

  // Build a list of output ports from the engine map.
  for(auto const& [key, val] : _engine->outputDevices) {
    _output_ports.push_back(val);
  }
}

vector<MidiNode::MPtr> *MidiRouter::get_sources() {
  return &_sources;
}

vector<MidiNode::MPtr> *MidiRouter::get_destinations() {
  return &_destinations;
}

vector<MidiNode::MPtr> *MidiRouter::get_output_ports() {
  return &_output_ports;
  
}

vector<string> *MidiRouter::get_input_ports() {
  return &_input_ports;
}