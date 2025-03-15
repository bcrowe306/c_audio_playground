#pragma once

#ifndef MIDI_ROUTER_H
#define MIDI_ROUTER_H

#include "core/MidiHandler.h"
#include "core/MidiNode.h"
#include "core/MidiMsg.h"
#include "core/MidiEngine.h"
#include <vector>
#include <string>
#include <memory>

using std::vector;
using std::string;

using MPtr = std::shared_ptr<MidiNode>;

class MidiRouter : public MidiHandler {

public:
  MidiRouter(shared_ptr<MidiEngine> engine);
  ~MidiRouter();
  void add_node(MPtr node);
  void remove_node(MPtr node);
  void process(MidiMsg &event) override;
  vector<MPtr> * get_sources();
  vector<MPtr> * get_destinations();
  vector<MPtr> * get_output_ports();
  vector<string> * get_input_ports();
  void update_ports();

protected:
  shared_ptr<MidiEngine> _engine;
  void _add_child_node(MPtr node);
  void _remove_child_node(MPtr node);
  void _process_child_addition_queue();
  void _process_child_removal_queue();
  bool _is_processing = false;
  vector<MPtr> _children;
  vector<MPtr> _sources;
  vector<MPtr> _destinations;
  vector<MPtr> _output_ports;
  vector<string> _input_ports;
  moodycamel::ConcurrentQueue<MPtr> _child_removal_queue = moodycamel::ConcurrentQueue<MPtr>(32);
  moodycamel::ConcurrentQueue<MPtr> _child_addition_queue = moodycamel::ConcurrentQueue<MPtr>(32);
};

#endif // !MIDI_ROUTER_H