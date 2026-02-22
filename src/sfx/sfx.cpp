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

    s.id = get_id();
    return s.id;
}
void SFX::play(unsigned int sound_id){
    std::cout << "Playing sound id: "<< sound_id << std::endl;
    for(SoundRes& s : sounds){
        if(s.id == sound_id){
            ma_sound_start(&s.sound);
            return;
        }
    }
    std::cout << "Could not find a sound file with that id!" << std::endl;
}
unsigned int SFX::get_id(){
    static unsigned int id = 0;
    return id++;
}