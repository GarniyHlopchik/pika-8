#include "sfx.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"


SFX::SFX(){
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        std::cout << "Sound engine couldn't start" << std::endl;
    }

}
SFX::~SFX(){
    for (SoundRes& s : sounds) {
        ma_sound_uninit(&s.sound);
    }
    ma_engine_uninit(&engine);
}
unsigned int SFX::load(const std::string& path){
    // 1. Create the entry directly in the list
    sounds.emplace_back(); 
    SoundRes& s = sounds.back(); 
    
    // 2. Clear memory to be safe
    memset(&s.sound, 0, sizeof(ma_sound));

    // 3. Initialize directly into the list's memory
    ma_result result = ma_sound_init_from_file(&engine, path.c_str(), 0, NULL, NULL, &s.sound);
    
    if (result != MA_SUCCESS) {
        std::cout << "Sound file could not be loaded: " << path << std::endl;
        sounds.pop_back(); // Remove failed entry
        return 0;
    }
    //auto gen = get_id();
    //auto it = gen.begin();
    //if (it != gen.end()) {
    //    s.id = *it;
    //}
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
//std::generator<int> SFX::get_id() {
//    int id = 0;
//    while (true) {
//        co_yield id;
//        id++;
//    }
//}