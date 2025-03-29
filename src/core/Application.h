#pragma once
#include "audio/choc_SampleBuffers.h"
#include <cstddef>
#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/Daw.h"
#include "core/Project.h"
#include "yaml-cpp/yaml.h"
#include "core/TrackNode.h"
#include <memory>
#include <iostream>
#include <string>

using std::shared_ptr;
using std::make_shared;
using std::string;

struct TracksStruct {
  TracksStruct();
  TracksStruct(shared_ptr<Daw> daw, shared_ptr<Project> project);

  void add_track();
  void remove_track(size_t index);
  vector<shared_ptr<TrackNode>> * get_tracks();
  shared_ptr<TrackNode> get_track(size_t index);
  void select_track(size_t index);
  size_t get_track_index(shared_ptr<TrackNode> track);
  shared_ptr<TrackNode> get_selected_track();
  int get_selected_track_index();
  shared_ptr<TrackNode> get_bus_tracks();
  shared_ptr<TrackNode> get_selected_bus_track();
  shared_ptr<TrackNode> get_master_track();
  void move_track(unsigned int from, unsigned int to);
  void solo_track(size_t index);
  void mute_track(size_t index);
  void arm_track(size_t index);
  shared_ptr<Daw> daw;
  shared_ptr<Project> project;
};

struct Transport {
  Transport(shared_ptr<Daw> daw);

  void play();
  void toggle_play();
  bool is_playing();
  void stop();
  void record();
  bool is_recording();
  void toggle_record();
  void pause();
  void set_loop(bool enabled);
  bool get_loop();
  void set_loop_start();
  int get_loop_start();
  void set_loop_end();
  int get_loop_end();
  void set_tempo(float tempo);
  float get_tempo();
  void set_time_signature_numerator(int numerator);
  int get_time_signature_numerator();
  void set_time_signature_denominator(int denominator);
  int get_time_signature_denominator();
  void set_quantization(int quantization);
  int get_quantization();
  void set_metronome_enabled(bool enabled);
  bool get_metronome_enabled();
  void toggle_metronome();
  void set_metronome_half_beat(bool half_beat);
  bool get_metronome_half_beat();
  void set_metronome_volume(float volume);
  float get_metronome_volume();
  void set_metronome_down_beat_volume(float volume);
  float get_metronome_down_beat_volume();
  void set_metronome_beat_volume(float volume);
  float get_metronome_beat_volume();
  void set_metronome_half_beat_volume(float volume);
  float get_metronome_half_beat_volume();
  void set_launch_quantization(int quantization);
  int get_launch_quantization();
  int get_metronome_type();
  string get_launch_quantization_string();

  string get_song_position_string();
  shared_ptr<Daw> daw;
};

class Application {
public:
  Application(shared_ptr<Daw> daw);
  ~Application();
  void new_project();
  void save_project();
  void load_project();
  void run();
  void quit();
  shared_ptr<Daw> daw;
  shared_ptr<Project> project;
  Transport Transport;
  TracksStruct Tracks;
};

#endif //  APPLICATION_H