#pragma once
#ifndef METRONOME_H
#define METRONOME_H

#include <cmath>
#include <memory>
#include <any>
#include <string>
#include "nodes/BufferNode.h"
#include "library/Observer.h"
#include "library/Parameter.h"
#include "core/Playhead.h"
#include "nodes/GainNode.h"


class Metronome : public ISubject, public IObserver
{
public:
    std::shared_ptr<BufferNode> metronome_sound;
    std::shared_ptr<GainNode> global_gain_node;
    std::shared_ptr<GainNode> beat_gain_node;
    BoolParameter enabled = BoolParameter("Enabled", true);
    BoolParameter half_beat_enabled = BoolParameter("Half Beat", false);
    FloatParameter bar_gain = FloatParameter("Bar Gain", 0.0, -60.0, 6.0, .1 );
    FloatParameter beat_gain = FloatParameter("Bar Gain", -6.0, -60.0, 6.0, .1 );
    FloatParameter half_beat_gain = FloatParameter("Bar Gain", -24.0, -60.0, 6.0, .1 );
    FloatParameter global_gain = FloatParameter("Global Gain", -3.0, -60.0, 6.0, .1 );


    Metronome(){
        this->metronome_sound = std::make_shared<BufferNode>("/Users/bcrowe/Documents/Audio Samples/BVKER - The Astro Hip Hop Drum Kit/Drum Hits/Percs/BVKER - The Astro Perc 10.wav");
        this->global_gain_node = make_shared<GainNode>();
        this->beat_gain_node = make_shared<GainNode>();
        this->metronome_sound->add_node_to_chain(beat_gain_node);
        metronome_sound->add_node_to_chain(global_gain_node);

        global_gain.on_change = [&](float value){
            this->global_gain_node->set_gain_db(value);
        };
        global_gain.update();

        enabled.on_change = [&](bool value){
            this->set_enabled(value);
        };
        enabled.update();
        this->bind_function(Playhead::Events::STATE_CHANGE, std::bind(&Metronome::on_playhead_state_changed, this, std::placeholders::_1, std::placeholders::_2));

        half_beat_enabled.update();
    };

    ~Metronome(){
        std::cout << "Metronome destroyed\n";
    };

    // Enable or disable the metronome
    void on_playhead_state_changed(std::string event, std::any data){
            int playhead_event = std::any_cast<int>(data);
            if(playhead_event == (int)PlayheadState::STOPPED){
                _metronome_type = -1;
            }
            std::cout << "Metronome received playhead event: " << playhead_event << std::endl;
    };
    void set_enabled(bool enabled){
        if(enabled){
        }
        this->_enabled = enabled;
    };

    // Toggle the enabled state of the metronome
    void toggle_enabled(){
        this->enabled.set_value(!this->enabled.get_value());
    };
    
    // Returns an int representing the state of the metronome beat. 
    // -1 = stopped, 0 = half beat, 1 = beat, 2 = bar
    // @return int
    int get_metronome_type(){
        return _metronome_type;
    };

    void on_playhead_event(PlayheadEvents event){
        if(event == PlayheadEvents::BAR){
            _metronome_type = 2;
            beat_gain_node->set_gain_db(bar_gain.get_value());
            trigger_metronome(2);
        }
        if(event == PlayheadEvents::BEAT){
            _metronome_type = 1;
            beat_gain_node->set_gain_db(beat_gain.get_value());
            trigger_metronome(1);
        }
        if(event == PlayheadEvents::HALF_BEAT){
            _metronome_type = 0;
            if(half_beat_enabled.get_value()){
                beat_gain_node->set_gain_db(half_beat_gain.get_value());
                trigger_metronome(0);
            }
        }
    };


protected:
    bool _enabled = false;
    int _metronome_type = -1;
    void trigger_metronome(int type){
        if(_enabled){
            switch(type){
                case 0:
                    metronome_sound->set_counter_rate(.75);
                    break;
                case 1:
                    metronome_sound->set_counter_rate(1);
                    break;
                case 2:
                    metronome_sound->set_counter_rate(1.5);
                    break;
            }
            metronome_sound->trigger();
        }
    };
    
};


#endif // !METRONOME_H