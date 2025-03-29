#pragma once
#include "yaml-cpp/emittermanip.h"
#ifndef PARAMETER_H
#define PARAMETER_H

#include "Observer.h"
#include "core/EventRegistry.h"
#include "utility.h"
#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include "yaml-cpp/yaml.h"


class Parameter : public ISubject, public IObserver {
public:
  struct Events {
    inline static const std::string VALUE = "VALUE";
  };
  Parameter() {}
  Parameter(std::string name, unsigned int sample_rate = 44100) {
    this->_sample_rate = sample_rate;
    this->name = name;
  }
  virtual ~Parameter() {  };
  virtual void process(){};
  virtual void encode(YAML::Emitter &out){};
  virtual void decode(YAML::Node &node){};
  void prepare(int sample_rate, int buffer_size){
    this->_sample_rate = sample_rate;
    this->_buffer_size = buffer_size;
  }

  

protected:
  std::string name;
  u_int _sample_rate = 44100;
  int _buffer_size = 64;
};

class StringParameter : public Parameter {
public:
  StringParameter(std::string name, std::string default_value) {
    this->name = name;
    this->_value = default_value;
  };

  ~StringParameter() { std::cout << "Parameter destroyed\n"; };
  std::function<void(std::string)> on_change;

  // Get the value of the parameter
  std::string get_value() { return _value; };

  std::string get_default_value() { return _default_value; };

  // Set the value of the parameter
  void set_value(std::string value) {
    if (this->_value != value) {
      this->_value = value;
      if (on_change) {
        on_change(this->_value);
      }
      notify(Parameter::Events::VALUE, this->_value);
    }
  };

  void set_default_value(std::string default_value) {
    this->_default_value = default_value;
  };

  void reset() { set_value(_default_value); };

  void set_sample_rate(u_int sample_rate) { this->_sample_rate = sample_rate; };

  // Increment the value of the parameter
  void increment() { };

  // Decrement the value of the parameter
  void decrement() { };

  // Update listeners
  void update() {
    if (on_change) {
      on_change(this->_value);
    }
    this->notify(Parameter::Events::VALUE, this->_value);
  };

  void encode(YAML::Emitter &out) {
    out << YAML::Key << name;
    out << YAML::Value << _value;
  }

  void decode(YAML::Node &node) {
    _value = node["value"].as<std::string>();
  }
  protected:
  std::string _value;
  std::string _default_value = "";
};

// Interger Parameter class
class IntParameter : public Parameter {
public:
  IntParameter(std::string name, int default_value, int min, int max, int step,
               unsigned int sample_rate = 44100) {
    this->_sample_rate = sample_rate;
    this->set_time_interval_ms(10);
    this->name = name;
    this->_value = default_value;
    this->_default_value = default_value;
    this->_min = min;
    this->_max = max;
    this->_step = step;
  };

  ~IntParameter() { std::cout << "Parameter destroyed\n"; };
  std::function<void(int)> on_change;

  // Get the value of the parameter
  int get_value() { return _value; };

  int get_min() { return _min; };

  int get_max() { return _max; };

  int get_step() { return _step; };

  int get_default_value() { return _default_value; };

  // Set the range of the parameter
  void set_range(int min, int max) {
    this->_min = min;
    this->_max = max;
  };

  // Set the value of the parameter
  void set_value(int value) {
    if (this->_value != value) {
      this->_value = constrain(value, _min, _max);
      if (on_change) {
        on_change(this->_value);
      }
      notify(Parameter::Events::VALUE, this->_value.load());
    }
  };

  void set_step(int step) { this->_step = step; };

  void set_default_value(int default_value) {
    this->_default_value = default_value;
  };

  void reset() { set_value(_default_value); };

  void set_sample_rate(u_int sample_rate) { this->_sample_rate = sample_rate; };

  void set_time_interval_ms(int time_ms) {
    this->_frames_interval = time_ms / 1000 * _sample_rate;
  };

  // Increment the value of the parameter
  void increment() {
    if (get_value() + _step <= _max) {
      set_value(get_value() + _step);
    } else {
      set_value(_max);
    }
  };

  // Decrement the value of the parameter
  void decrement() {
    if (get_value() - _step >= _min) {
      set_value(get_value() - _step);
    } else {
      set_value(_min);
    }
  };

  // Update listeners
  void update() {
    if (on_change) {
      on_change(this->_value);
    }
    this->notify(Parameter::Events::VALUE, this->_value.load());
  };

  void set_value_interval(int target_value, float time_ms = 10) {
    if (target_value == _value) {
      return;
    }
    _target_value = target_value;
    _frames_interval = time_ms / 1000 * (float)_sample_rate;
    _step = (float)(target_value - _value) / _frames_interval;
    _current_frame = 0;
    _processing = true;
  }
  void process() {
    if (_processing) {
      _value += _step;
      // std::cout << "Processing frame:" <<  _current_frame << ", Value: "<<
      // _value << "Frame_interval: " << _frames_interval << std::endl;
      if (_current_frame < _frames_interval) {
        _current_frame++;
      } else {
        _processing = false;
        set_value(_target_value);
        _current_frame = 0;
      }
    }
  };
  void encode(YAML::Emitter &out) {
    out << YAML::Key << name;
    out << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "value" << YAML::Value << _value;
    out << YAML::Key << "min" << YAML::Value << _min;
    out << YAML::Key << "max" << YAML::Value << _max;
    out << YAML::Key << "step" << YAML::Value << _step;
    out << YAML::Key << "default_value" << YAML::Value << _default_value;
    out << YAML::EndMap;
  }

  void decode(YAML::Node &node) {
    _value = node["value"].as<int>();
    _min = node["min"].as<int>();
    _max = node["max"].as<int>();
    _step = node["step"].as<int>();
    _default_value = node["default_value"].as<int>();
  }

protected:
  EventRegistry &registry = EventRegistry::getInstance();

  int _target_value;
  bool _processing = false;
  std::atomic<int> _value;
  int _min = 0;
  int _max = 1;
  int _default_value = 0;
  int _step = 1;
  u_int _current_frame = 0;
  u_int _frames_interval = 0;
  float _interval_step;
};

// Float Parameter class
class FloatParameter : public Parameter {
public:
  FloatParameter(std::string name, float default_value, float min, float max,
                 float step, float sample_rate = 44100) {
    this->_sample_rate = sample_rate;
    this->set_time_interval_ms(10);
    this->name = name;
    this->_value = default_value;
    this->_default_value = default_value;
    this->_min = min;
    this->_max = max;
    this->_step = step;
  };

  ~FloatParameter() { std::cout << "Parameter destroyed\n"; };
  std::function<void(float)> on_change;

  // Get the value of the parameter
  float get_value() { return _value; };

  float get_min() { return _min; };

  float get_max() { return _max; };

  float get_step() { return _step; };

  float get_default_value() { return _default_value; };

  // Set the range of the parameter
  void set_range(float min, float max) {
    this->_min = min;
    this->_max = max;
  };

  // Set the value of the parameter
  void set_value(float value) {
    if (this->_value != value) {
      this->_value = constrain(value, _min, _max);
      if (on_change) {
        on_change(this->_value);
      }
      notify(Parameter::Events::VALUE, this->_value.load());
    }
  };

  void reset() { set_value(_default_value); };

  void set_step(float step) { this->_step = step; };

  void set_default_value(float default_value) {
    this->_default_value = default_value;
  };

  void set_sample_rate(u_int sample_rate) { this->_sample_rate = sample_rate; };

  void set_time_interval_ms(float time_ms) {
    this->_frames_interval = time_ms / 1000 * _sample_rate;
  };
  // Increment the value of the parameter
  void increment() {
    if (get_value() + _step <= _max) {
      set_value(get_value() + _step);
    } else {
      set_value(_max);
    }
  };

  // Decrement the value of the parameter
  void decrement() {
    if (get_value() - _step >= _min) {
      set_value(get_value() - _step);
    } else {
      set_value(_min);
    }
  };

  // Update listeners
  void update() {
    if (on_change) {
      on_change(this->_value);
    }
    this->notify(Parameter::Events::VALUE, this->_value.load());
  };

  void set_value_interval(float target_value, float time_ms = 10) {
    if (target_value == _value) {
      return;
    }
    _target_value = target_value;
    _frames_interval = time_ms / 1000 * (float)_sample_rate;
    _step = (float)(target_value - _value) / _frames_interval;
    _current_frame = 0;
    _processing = true;
  }
  void process() override {
    if (_processing) {
      _value.store(_value + _step);
      // std::cout << "Processing frame:" <<  _current_frame << ", Value: "<<
      // _value << "Frame_interval: " << _frames_interval << std::endl;
      if (_current_frame < _frames_interval) {
        _current_frame++;
      } else {
        _processing = false;
        set_value(_target_value);
        _current_frame = 0;
      }
    }
  };
  void encode(YAML::Emitter &out) override {
    out << YAML::Key << name;
    out << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "value" << YAML::Value << _value;
    out << YAML::Key << "min" << YAML::Value << _min;
    out << YAML::Key << "max" << YAML::Value << _max;
    out << YAML::Key << "step" << YAML::Value << _step;
    out << YAML::Key << "default_value" << YAML::Value << _default_value;
    out << YAML::EndMap;
  }

  void decode(YAML::Node &node) override {
    _value = node["value"].as<float>();
    _min = node["min"].as<float>();
    _max = node["max"].as<float>();
    _step = node["step"].as<float>();
    _default_value = node["default_value"].as<float>();
  }

protected:
  float _target_value;
  bool _processing = false;
  std::atomic<float> _value;
  float _min = 0;
  float _max = 1;
  float _default_value = 0;
  float _step = 1;
  u_int _current_frame = 0;
  u_int _frames_interval = 0;
  float _interval_step;
};

// Bool Parameter class
class BoolParameter : public Parameter {
public:
  BoolParameter(std::string name, bool default_value,
                unsigned int sample_rate = 44100) {
    this->_sample_rate = sample_rate;
    this->name = name;
    this->_value = default_value;
  };

  ~BoolParameter(){};

  std::function<void(bool)> on_change;

  // Get the value of the parameter
  bool get_value() { return _value; };

  bool get_default_value() { return _default_value; };

  // Set the value of the parameter
  void set_value(bool value) {
    if (this->_value != value) {
        this->_value = value;
        if (on_change) {
          on_change(this->_value);
        }
        notify(Parameter::Events::VALUE, this->_value);
    }
  };

  void set_default_value(bool default_value) {
    this->_default_value = default_value;
  };

  void reset() { set_value(_default_value); };

  void set_sample_rate(u_int sample_rate) { this->_sample_rate = sample_rate; };

  // Increment the value of the parameter
  void increment() { set_value(!get_value()); };

  // Decrement the value of the parameter
  void decrement() { set_value(!get_value()); };

  // Update listeners
  void update() {
    if (on_change) {
      on_change(this->_value);
    }
    this->notify(Parameter::Events::VALUE, this->_value);
  };
  void encode(YAML::Emitter &out) override {
    out << YAML::Key << name;
    out << YAML::Value << _value;
  }

  void decode(YAML::Node &node) override {
    _value = node["value"].as<bool>();
  }

protected:
  EventRegistry &registry = EventRegistry::getInstance();
  float _value;
  float _default_value = 0;
};

template <typename T>
class OptionParameter : public ISubject, public IObserver {
public:
  struct Events {
    inline static const std::string VALUE = "VALUE";
  };
  OptionParameter(std::string name, std::vector<T> options, T default_value) {
    this->name = name;
    this->options = options;
    this->_value = default_value;
  };

  ~OptionParameter() { std::cout << "OptionParameter destroyed\n"; };
  std::function<void(T)> on_change;

  // Get the value of the parameter
  T get_value() { return _value; };

  // Set the value of the parameter
  void set_value(T value) {
    if (this->_value != value) {
      this->_value = value;
      if (on_change) {
        on_change(this->_value);
      }
      notify(OptionParameter::Events::VALUE, this->_value);
    }
  };

  // Set the value of the parameter by string
  void set_option(T option) {
    // Find the index of the option
    auto it = std::find(options.begin(), options.end(), option);
    if (it != options.end()) {
      set_value(*it);
    }
  };

  // Get the current option name of the parameter
  T get_option() { return options[_value]; };

  // Increment the value of the parameter
  void increment() {
    // Cycle positively through the options vector, loop to beginning when end
    // is reached get index of current option
    auto it = std::find(options.begin(), options.end(), get_option());
    // increase index by one
    it++;
    // if index is at the end of the vector, loop to beginning
    if (it == options.end()) {
      it = options.begin();
    }
    // set the value to the option at the new index
    set_value(*it);
  };

  // Decrement the value of the parameter
  void decrement() {
    // Cycle negatively through the options vector, loop to end when beginning
    // is reached get index of current option
    auto it = std::find(options.begin(), options.end(), get_option());
    // decrease index by one
    it--;
    // if index is at the beginning of the vector, loop to end
    if (it == options.begin() - 1) {
      it = options.end() - 1;
    }
    // set the value to the option at the new index
    set_value(*it);
  };

  void set_default_value(T default_value) { this->_default_value = default_value; };

  void reset() { set_value(_default_value); };

  // Update listeners
  void update() {
    if (on_change) {
      on_change(this->_value);
    }
    this->notify(this->_value);
  };

  void encode(YAML::Emitter &out) {
    out << YAML::Key << name;
    out << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "value" << YAML::Value << _value;
    out << YAML::Key << "default_value" << YAML::Value << _default_value;
    out << YAML::Key << "options" << YAML::Value << YAML::BeginSeq;
    for (auto &option : options) {
      out << option;
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;
  }

  void decode(YAML::Node &node) {
    _value = node["value"].as<T>();
    _default_value = node["default_value"].as<T>();
    options.clear();
    for (auto option : node["options"]) {
      options.push_back(option.as<T>());
    }
  }

  void prepare(int sample_rate, int buffer_size) {
    this->_sample_rate = sample_rate;
    this->_buffer_size = buffer_size;
  }
  void set_sample_rate(u_int sample_rate) { this->_sample_rate = sample_rate; };
  void set_buffer_size(int buffer_size) { this->_buffer_size = buffer_size; };

protected:
  int _sample_rate = 44100;
  int _buffer_size = 64;
  std::string name = name;
  std::vector<T> options = options;
  T _value;
  T _default_value;
};

class EnumParameter : public ISubject, public IObserver {
public:
  struct Events {
    inline static const std::string VALUE = "VALUE";
  };
  EnumParameter(std::string name, std::vector<std::string> options,
                int default_value);
  ~EnumParameter();
  std::function<void(int)> on_change;

  // Set the value of the parameter
  void set_value(int value);

  // Set the value of the parameter by string
  void set_option(std::string option);

  // Get the value of the parameter
  int get_value();

  // Get the current option name of the parameter
  std::string get_option();

  // Increment the value of the parameter
  void increment();

  // Decrement the value of the parameter
  void decrement();

  // Update listeners
  void update();

  void encode(YAML::Emitter &out);

  void decode(YAML::Node &node);

  void prepare(int sample_rate, int buffer_size) {
    this->_sample_rate = sample_rate;
    this->_buffer_size = buffer_size;
  }
  void set_sample_rate(u_int sample_rate) { this->_sample_rate = sample_rate; };
  void set_buffer_size(int buffer_size) { this->_buffer_size = buffer_size; };

protected:
  std::string name;
  std::vector<std::string> options;
  int value;
  int default_value = 0;
  int _sample_rate = 44100;
  int _buffer_size = 64;
};

#endif //  PARAMETER_H

