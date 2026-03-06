#include "sfx.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"
#include "file_resolve/file_system.h"


SFX::SFX(){
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        std::cout << "Sound engine couldn't start" << std::endl;
    }

}
SFX::~SFX(){
    for (SoundRes& s : sounds) {
        ma_sound_uninit(&s.sound);
        ma_decoder_uninit(&s.decoder);
    }
    ma_engine_uninit(&engine);
}
unsigned int SFX::load(const std::string& path){
    sounds.emplace_back();
    SoundRes& s = sounds.back();

    memset(&s.sound, 0, sizeof(ma_sound));

    Resource res = FileSystem::get_resource(path);

    if (!res.is_valid()) {
        std::cout << "Sound file could not be loaded: " << path << std::endl;
        sounds.pop_back();
        return 0;
    }

    ma_result result = ma_decoder_init_memory(
    res.data.get(),
    res.size,
    NULL,
    &s.decoder
    );

    if (result != MA_SUCCESS) {
        std::cout << "Failed to decode sound: " << path << std::endl;
        sounds.pop_back();
        return 0;
    }

    result = ma_sound_init_from_data_source(
        &engine,
        &s.decoder,
        0,
        NULL,
        &s.sound
    );

    if (result != MA_SUCCESS) {
        std::cout << "Failed to create sound: " << path << std::endl;
        sounds.pop_back();
        return 0;
    }

    s.data = std::move(res); // keep memory alive
    s.id = get_id();

    return s.id;
}


void SFX::play(unsigned int sound_id, float volume, float pitch, bool loop, float pan) {
    
    for(SoundRes& s : sounds){
        if(s.is_playing == true && s.id == sound_id){
            // stop(sound_id);
            // std::cout << "Sound was already playing: "<< sound_id << std::endl;
            return;
        } else if(s.is_playing == false && s.id == sound_id){
            // std::cout << "Playing sound id: "<< sound_id << std::endl;
            ma_sound_set_volume(&s.sound, volume);
            ma_sound_set_pitch(&s.sound, pitch);
            ma_sound_set_pan(&s.sound, pan);
            ma_sound_set_looping(&s.sound, loop ? MA_TRUE : MA_FALSE);
            ma_sound_start(&s.sound);
            s.is_playing = true;
            // std::cout << "Sound started: "<< sound_id << std::endl;
            return;
        }
    }
    std::cout << "Could not find a sound file with that id!" << std::endl;
}

void SFX::stop(unsigned int sound_id) {
    for(SoundRes& s : sounds){
        if(s.id == sound_id && s.is_playing == true){
            ma_sound_stop(&s.sound);
            s.is_playing = false;
            // std::cout << "Sound stopped: "<< sound_id << std::endl;
            return;
        }
    }
    // std::cout << "Could not find a sound file with that id!" << std::endl;
}

unsigned int SFX::get_id(){
    static unsigned int id = 0;
    return id++;
}
