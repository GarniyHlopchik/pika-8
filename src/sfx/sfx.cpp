
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
void SFX::worker_loop(){
    while(worker_should_run){
        SFXLoadRequest task;
        {
            std::unique_lock<std::mutex> lock(to_worker_mutex);
            
            // Wait until there's a task OR the engine is shutting down
            cv.wait(lock, [this]{ 
                return to_worker_queue.peek(QueueProtocol::OLDEST).priority || !worker_should_run; 
            });

            // If we woke up because we're shutting down, exit the loop
            if (!worker_should_run && !to_worker_queue.peek(QueueProtocol::OLDEST).priority) break;

            task = to_worker_queue.dequeue(QueueProtocol::OLDEST).data;
        }

        // Process the task
        unsigned int sound_handle = load(task.path.c_str());
        //enqueue to main with lock
        {
            std::lock_guard<std::mutex> lock(to_main_mutex);
            SFXLoadResult data = {task.registry_ref, sound_handle};
            to_main_queue.enqueue(data, 1);
        }
        
    }
}
void SFX::poll_loaded_sounds(){
    std::lock_guard<std::mutex> lock(to_main_mutex);
    while(to_main_queue.peek(QueueProtocol::OLDEST).priority){
        SFXLoadResult finished_task = to_main_queue.dequeue(QueueProtocol::OLDEST).data;
        //TODO: set ready and value of respective table reference
    }
}
void SFX::schedule_load(const std::string& path, int registry_ref){
    std::lock_guard<std::mutex> lock(to_worker_mutex);
    SFXLoadRequest request = {registry_ref, path};
    to_worker_queue.enqueue(request,1);
}