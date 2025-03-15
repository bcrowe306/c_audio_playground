#include "Application.h"
#include "Daw.h"
#include <memory>
#include <string>

Transport::Transport(shared_ptr<Daw> daw) : daw(daw)
{ std::cout << "Transport created\n"; }
void Transport::play() {
    daw->playhead->play();
}
void Transport::toggle_play() {
    daw->playhead->toggle_play();
}
bool Transport::is_playing() {
    return daw->playhead->is_playing();
}
void Transport::stop() {
    daw->playhead->stop();
}
void Transport::record() {
    daw->playhead->record();
}
bool Transport::is_recording() {
    return daw->playhead->is_recording();
}
void Transport::toggle_record() {
    daw->playhead->toggle_record();
}
void Transport::pause() {
    // Not implemented
    // daw->playhead->pause();
}
void Transport::set_loop(bool enabled) {
    // Not implemented
    // daw->playhead->set_loop(enabled);
}
bool Transport::get_loop() {
    // Not implemented
    // return daw->playhead->get_loop();
}
void Transport::set_loop_start() {
    // Not implemented
    // daw->playhead->set_loop_start();
}
int Transport::get_loop_start() {
    // Not implemented
    // return daw->playhead->get_loop_start();
}
void Transport::set_loop_end() {
    // Not implemented
    // daw->playhead->set_loop_end();
}
int Transport::get_loop_end() {
    // Not implemented
    // return daw->playhead->get_loop_end();
} 
void Transport::set_tempo(float tempo) {
    daw->playhead->bpm.set_value(tempo);
}

float Transport::get_tempo() {
    return daw->playhead->bpm.get_value();
}

void Transport::set_time_signature_numerator(int numerator) {
    daw->playhead->time_signature_numerator.set_value(numerator);
}

int Transport::get_time_signature_numerator() {
    return daw->playhead->time_signature_numerator.get_value();
}   

void Transport::set_time_signature_denominator(int denominator) {
    daw->playhead->time_signature_denominator.set_value(denominator);
}

int Transport::get_time_signature_denominator() {
    return daw->playhead->time_signature_denominator.get_value();
}

void Transport::set_quantization(int quantization) {
    // Not implemented
    // daw->playhead->set_quantization(quantization);
}

int Transport::get_quantization() {
    // Not implemented
    // return daw->playhead->get_quantization();
}

void Transport::set_metronome_enabled(bool enabled) {
    daw->metronome->enabled.set_value(enabled);
}

bool Transport::get_metronome_enabled() {
    return daw->metronome->enabled.get_value();
}

void Transport::toggle_metronome() {
    daw->metronome->toggle_enabled();
}

void Transport::set_metronome_half_beat(bool half_beat) {
    daw->metronome->half_beat_enabled.set_value(half_beat);
}

bool Transport::get_metronome_half_beat() {
    return daw->metronome->half_beat_enabled.get_value();
}

void Transport::set_metronome_volume(float volume) {
    daw->metronome->global_gain.set_value(volume);
}

float Transport::get_metronome_volume() {
    return daw->metronome->global_gain.get_value();
}

void Transport::set_metronome_down_beat_volume(float volume) {
    daw->metronome->bar_gain.set_value(volume);
}

float Transport::get_metronome_down_beat_volume() {
    return daw->metronome->bar_gain.get_value();
}

void Transport::set_metronome_beat_volume(float volume) {
    daw->metronome->beat_gain.set_value(volume);
}

float Transport::get_metronome_beat_volume() {
    return daw->metronome->beat_gain.get_value();
}

void Transport::set_metronome_half_beat_volume(float volume) {
    daw->metronome->half_beat_gain.set_value(volume);
}

float Transport::get_metronome_half_beat_volume() {
    return daw->metronome->half_beat_gain.get_value();
}

void Transport::set_launch_quantization(int quantization) {
    // Not implemented
    daw->playhead->launch_quantization.set_value(quantization);
}

int Transport::get_launch_quantization() {
    // Not implemented
    return daw->playhead->launch_quantization.get_value();
}

string Transport::get_launch_quantization_string() {
    return daw->playhead->launch_quantization.get_option();
}

string Transport::get_song_position_string() {
    return daw->playhead->get_song_position_string();
}

int Transport::get_metronome_type() {
    return daw->metronome->get_metronome_type();
}

Application::Application(shared_ptr<Daw> daw) : 
    daw(daw),
    Transport(daw)
{
    std::cout << "Application created\n";
    new_project();
}


Application::~Application()
{
    std::cout << "Application destroyed\n";
}

void Application::new_project() { 
    project = make_shared<Project>(daw); 
}