#include "BufferNode.h"
#include <algorithm>

static inline float sample_interpolation(float read_pointer, u_int channel, u_int start, u_int end, choc::buffer::BufferView<float, choc::buffer::SeparateChannelLayout> &audio_buffer)
{
    // Linear interpolation code
    int index_below = floorf(read_pointer);
    int index_above = index_below + 1;
    float fraction_above = read_pointer - index_below;
    float fraction_below = 1 - fraction_above;

    // Contraints to prevent assetion for out of range
    if (index_above >= audio_buffer.getNumFrames())
    {
        index_above = 0;
    }
    if (index_below >= audio_buffer.getNumFrames()) {
      index_below = 0;
    }
    if(index_above < 0){
        index_above = 0;
    }
    if(index_below < 0){
        index_below = 0;
    }
    
    float sample_below_left = audio_buffer.getSample(channel, index_below);
    float sample_above_left = audio_buffer.getSample(channel, index_above);

    return sample_below_left * fraction_below + sample_above_left * fraction_above;
}
static inline float sample_interpolation(float read_pointer, u_int channel, u_int start, u_int end, choc::buffer::ChannelArrayBuffer<float> &audio_buffer)
{
    // Linear interpolation code
    int index_below = floorf(read_pointer);
    int index_above = index_below + 1;
    float fraction_above = read_pointer - index_below;
    float fraction_below = 1 - fraction_above;
    if (index_above >= end)
    {
        index_above = start;
    }

    float sample_below_left = audio_buffer.getSample(channel, index_below);
    float sample_above_left = audio_buffer.getSample(channel, index_above);

    return sample_below_left * fraction_below + sample_above_left * fraction_above;
}
BufferNode::BufferNode(std::string file_path, u_int sample_rate)
    {
        notify_state_change(BufferNode::States::OFFLINE);
        set_is_source_node(true);
        this->_sample_rate = sample_rate;
        load_sample(file_path);
        _peaks.reserve(4096);
        _averages.reserve(4096);
    };
BufferNode::BufferNode(u_int sample_rate)
    {
        this->_sample_rate = sample_rate;
    };
    BufferNode::~BufferNode(){

    };
    void BufferNode::load_sample(std::string file_path)
    {
        _loading = true;
        notify_state_change(BufferNode::States::LOADING);
        _file_path = file_path;
        _file_name = std::filesystem::path(file_path).filename().string();

        // check file extension to insure it's a valid wav file
        if (path(file_path).extension() != ".wav") {
          std::cerr << "Invalid file: " << file_path << std::endl;
          _loading = false;
          _offline = true;
          notify_state_change(BufferNode::States::OFFLINE);
          return;
        }

        try
        {
            _reader = _wav.createReader(file_path);
            _audio_buffer = _reader->loadFileContent(this->_sample_rate);
            _buffer_view = _audio_buffer.frames.getView();
            _end = _audio_buffer.frames.getNumFrames();
            _length = _end;

        } catch (const std::exception &) {
            std::cerr << "Error loading file: " << file_path << std::endl;
            _loading = false;
            _offline = true;
            notify_state_change(BufferNode::States::OFFLINE);
          return;
        }
        _loading = false;
        _offline = false;
        waveform_generated = false;
        notify_state_change(BufferNode::States::READY);
    }
    void BufferNode::set_loop(bool value)
    {
        _loop = value;
    };
    void BufferNode::set_start(u_int value)
    {
        _start = value;
    };
    void BufferNode::set_end(u_int value)
    {
        _end = value;
    };
    void BufferNode::set_sample_rate(u_int value)
    {
        _sample_rate = value;
    };
    void BufferNode::set_interpolation(bool value)
    {
        _interpolation = value;
    };
    void BufferNode::set_reverse(bool value)
    {
        _reverse = value;
    };
    void BufferNode::set_offline(bool value)
    {
        _offline = value;
    };
    void BufferNode::set_counter_rate(float value)
    {
        _counter_rate = value;
    };
    void BufferNode::set_counter(float value)
    {
        _counter = value;
    };

    void BufferNode::trigger()
    {
        if (_reverse)
        {
            _counter = _end;
        }
        else
        {
            _counter = _start;
        }
        _is_playing = true;
    };

    void BufferNode::stop()
    {
        _is_playing = false;
    };

    void BufferNode::reset()
    {
        _counter = 0;
    };

    float *BufferNode::get_samples_internal_counter(float *buffer)
    {
        // Linear interpolation code
        int index_below = floorf(_counter);
        int index_above = index_below + 1;
        float fraction_above = _counter - index_below;
        float fraction_below = 1 - fraction_above;
        if (index_above >= _end)
        {
            index_above = _start;
        }
        // Stereo or mono
        if (_audio_buffer.frames.getNumChannels() == 2)
        {
            float sample_below_left = _audio_buffer.frames.getSample(0, index_below);
            float sample_below_right = _audio_buffer.frames.getSample(1, index_below);
            float sample_above_left = _audio_buffer.frames.getSample(0, index_above);
            float sample_above_right = _audio_buffer.frames.getSample(1, index_above);
            buffer[0] = sample_below_left * fraction_below + sample_above_left * fraction_above;
            buffer[1] = sample_below_right * fraction_below + sample_above_right * fraction_above;
        }
        else if (_audio_buffer.frames.getNumChannels() == 1)
        {
            float sample_below = _audio_buffer.frames.getSample(0, index_below);
            float sample_above = _audio_buffer.frames.getSample(0, index_above);
            buffer[0] = sample_below * fraction_below + sample_above * fraction_above;
            buffer[1] = buffer[0];
        }
        return buffer;
    };

    void BufferNode::generate_audio(float *buffer_to_fill, unsigned long frameCount, AudioEngineContext &context)
    {
        buffer_to_fill[0] = 0;
        buffer_to_fill[1] = 0;

        // Check if sampler is offline and return 0s if true
        if (_offline)
        {
            return;
        }

        // counter logic
        if (_counter >= _end)
        {
            if (_reverse)
            {
                _counter = _end;
            }
            else
            {
                _counter = _start;
            }
            if (!_loop)
            {
                _is_playing = false;
            }
        }

        // If the buffer is playing, get the samples
        if (_is_playing)
        {
          get_samples_internal_counter(buffer_to_fill);
        }

        // Increment the counter by rate. This enables speeding up or slowing down the playback
        _counter += _counter_rate;

        // Reverse the playback if reverse is true... 
        // TODO: Needs work
        if (_reverse)
        {
            _counter -= _counter_rate;
        }
        return;
    };

    float & BufferNode::get_samples(u_int index, u_int channel)
    {
        return _audio_buffer.frames.getSample(channel, index);
    };

    vector<AudioAverage> * BufferNode::get_audio_averages(int startSample, int endSample, int pixel_width){
      if (pixel_width == 0 || startSample >= endSample)
        return &_averages;

      if (_loading || _offline) {
        return nullptr;
      }
      bool bounds_changed = startSample != waveform_start || endSample != waveform_end;
      waveform_start = startSample;
      waveform_end = endSample;

      pixel_width = pixel_width > 4096 ? 4096 : pixel_width;
      if (_averages.size() == pixel_width && waveform_generated &&
          !bounds_changed) {
        return &_averages;
      }
      _averages.clear();
      size_t totalSamples = endSample - startSample;
      float samplesPerPixel = static_cast<float>(totalSamples) / pixel_width;

      for (size_t x = 0; x < pixel_width; ++x) {
        size_t start =
            startSample + static_cast<size_t>(std::floor(x * samplesPerPixel));
        size_t end = startSample +
                     static_cast<size_t>(std::floor((x + 1) * samplesPerPixel));
        end = std::min<int>(end, endSample); // Prevent out-of-bounds

        float sumAbs = 0.0f;
        float sumSquares = 0.0f;
        size_t count = end - start;

        for (size_t i = start; i < end; ++i) {
          float sample = _buffer_view.getSample(0, i);
          sumAbs += std::abs(sample);
          sumSquares += sample * sample;
        }

        float meanAbs = count > 0 ? sumAbs / count : 0.0f;
        float rms = count > 0 ? std::sqrt(sumSquares / count) : 0.0f;

        _averages.push_back({meanAbs, rms});
      }

      return &_averages;
    }
    
    std::vector<AudioPeak> * BufferNode::get_audio_peaks(int startSample, int endSample, int pixel_width) {
      if (_loading || _offline) {
        return nullptr;
      }
      bool bounds_changed = startSample != waveform_start || endSample != waveform_end;
      waveform_start = startSample;
      waveform_end = endSample;

      pixel_width = pixel_width > 4096 ? 4096 : pixel_width;
      if (_peaks.size() == pixel_width && waveform_generated && !bounds_changed) {
        return &_peaks;
      }
      _peaks.clear();

      // Generate peaks and return if not already generated
      size_t totalSamples = endSample - startSample;
      float samplesPerPixel = static_cast<float>(totalSamples) / pixel_width;

      for (size_t x = 0; x < pixel_width; ++x) {
        size_t start =
            startSample + static_cast<size_t>(std::floor(x * samplesPerPixel));
        size_t end = startSample +
                     static_cast<size_t>(std::floor((x + 1) * samplesPerPixel));
        end = std::min<int>(end, endSample); // Ensure we don't go out of bounds

        auto min_sample = _buffer_view.getSample(0, start);
        auto max_sample = _buffer_view.getSample(0, end);
        _peaks.push_back({min_sample, max_sample});
      }

      return &_peaks;
    }


    float *BufferNode::get_waveform(int &start, int &end,
                                      int &pixel_width) {
      if (_loading || _offline) {
        return nullptr;
      }
      bool bounds_changed = start != waveform_start || end != waveform_end;
        waveform_start = start;
        waveform_end = end;

      pixel_width = pixel_width > 4096 ? 4096 : pixel_width;
      if (_waveform.size() == pixel_width && waveform_generated && !bounds_changed) {
        return _waveform.data();
      }
      _waveform.clear();
      auto samples_per_pixel = (float)(end - start) / (float)pixel_width;
      auto read_head = (float)start;
      for (int i = start; i < pixel_width; i++) {
        if (_loading || _offline) {
          return nullptr;
        }
        _waveform.push_back(
            sample_interpolation(read_head, 0, start, end, _buffer_view));
        read_head += samples_per_pixel;
      }
      waveform_generated = true;
      return _waveform.data();
    }

    bool BufferNode::is_offline()
    {
        return _offline;
    };

    bool BufferNode::is_loading()
    {
        return _loading;
    };

    bool BufferNode::get_loop()
    {
        return _loop;
    };

    u_int BufferNode::get_start()
    {
        return _start;
    };

    u_int BufferNode::get_end()
    {
        return _end;
    };

    u_int BufferNode::get_sample_rate()
    {
        return _sample_rate;
    };

    bool BufferNode::get_interpolation()
    {
        return _interpolation;
    };

    std::string BufferNode::get_file_name()
    {
        return _file_name;
    };

    float * BufferNode::get_interpolated_samples(float read_pointer, float *buffer)
    {
        if(buffer == nullptr){
            float *new_buffer = new float[2];
            new_buffer[0] = sample_interpolation(read_pointer, 0, this->_start, this->_end, _audio_buffer.frames);
            
            // If stereo
            if(_audio_buffer.frames.getNumChannels() > 1)
            {
                new_buffer[1] = sample_interpolation(read_pointer, 1, this->_start,
                                             this->_end, _audio_buffer.frames);
            }
            else{
                new_buffer[1] = new_buffer[0];
            }
           
            return new_buffer;
        }
        else{
            buffer[0] = sample_interpolation(read_pointer, 0, this->_start, this->_end, _audio_buffer.frames);

            // If stereo
            if(_audio_buffer.frames.getNumChannels() > 1)
            {
                buffer[1] = sample_interpolation(read_pointer, 1, this->_start,
                                             this->_end, _audio_buffer.frames);
            }
            else{
                buffer[1] = buffer[0];
            }
            return buffer;
        }
    };

    float * BufferNode::get_stereo_interleaved_samples(u_int read_pointer, float *buffer)
    {
        if(buffer == nullptr){
            float *new_buffer = new float[2];
            new_buffer[0] = _audio_buffer.frames.getSample(0, read_pointer);
            new_buffer[1] = _audio_buffer.frames.getSample(1, read_pointer);
            return new_buffer;
        }
        else{
            buffer[0] = _audio_buffer.frames.getSample(0, read_pointer);
            buffer[1] = _audio_buffer.frames.getSample(1, read_pointer);
            return buffer;
        }
    };
