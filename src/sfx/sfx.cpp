
#include "sfx.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"
#include "file_resolve/file_system.h"
#include "logger/proxy.h"

SFX::SFX(LuaSystem* l_lua): worker_should_run(true), worker_thread(&SFX::worker_loop, this), lua(l_lua){
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        LOG(LogLevel::EROR, "Sound engine couldn't start");
    }

}
SFX::~SFX(){
    for (SoundRes& s : sounds) {
        ma_sound_uninit(&s.sound);
        ma_decoder_uninit(&s.decoder);
    }
    ma_engine_uninit(&engine);
    worker_should_run = false;
}
unsigned int SFX::load(const std::string& path){
    sounds.emplace_back();
    SoundRes& s = sounds.back();

    memset(&s.sound, 0, sizeof(ma_sound));

    Resource res = FileSystem::get_resource(path);

    if (!res.is_valid()) {
        LOG(LogLevel::EROR, "Sound file could not be loaded: ", path);
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
        LOG(LogLevel::EROR, "Failed to decode sound: ", path);
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
        LOG(LogLevel::EROR, "Failed to create sound: ", path);
        sounds.pop_back();
        return 0;
    }

    s.data = std::move(res); // keep memory alive
    s.id = get_id();

	LOG(LogLevel::DEBUG, "Loaded sound from '", path, "'. ID: '", s.id, "'");
    return s.id;
}


void SFX::play(unsigned int sound_id, float volume, float pitch, bool loop, float pan) {
    AudioCommand cmd;
    cmd.type = AudioCommandType::PLAY;
    cmd.sound_id = sound_id;
    cmd.volume = volume;
    cmd.pitch = pitch;
    cmd.loop = loop;
    cmd.pan = pan;
	// LOG(LogLevel::DEBUG, "Scheduling play command for sound ID: ", sound_id);
    schedule_command(cmd);
}

void SFX::stop(unsigned int sound_id) {
    AudioCommand cmd;
    cmd.type = AudioCommandType::STOP;
    cmd.sound_id = sound_id;
	// LOG(LogLevel::DEBUG, "Scheduling stop command for sound ID: ", sound_id);
    schedule_command(cmd);
}

void SFX::set_volume(unsigned int sound_id, float volume) {
    AudioCommand cmd;
    cmd.type = AudioCommandType::SET_VOLUME;
    cmd.sound_id = sound_id;
    cmd.volume = volume;
	// LOG(LogLevel::DEBUG, "Scheduling set volume command for sound ID: " , sound_id, " with volume: ", volume);
    schedule_command(cmd);
}

void SFX::set_pitch(unsigned int sound_id, float pitch) {
    AudioCommand cmd;
    cmd.type = AudioCommandType::SET_PITCH;
    cmd.sound_id = sound_id;
    cmd.pitch = pitch;
	// LOG(LogLevel::DEBUG, "Scheduling set pitch command for sound ID: " , sound_id, " with pitch: ", pitch);
    schedule_command(cmd);
}

void SFX::set_pan(unsigned int sound_id, float pan) {
    AudioCommand cmd;
    cmd.type = AudioCommandType::SET_PAN;
    cmd.sound_id = sound_id;
    cmd.pan = pan;
	// LOG(LogLevel::DEBUG, "Scheduling set pan command for sound ID: " , sound_id, " with pan: ", pan);
    schedule_command(cmd);
}

void SFX::set_looping(unsigned int sound_id, bool loop) {
    AudioCommand cmd;
    cmd.type = AudioCommandType::SET_LOOPING;
    cmd.sound_id = sound_id;
    cmd.loop = loop;
	// LOG(LogLevel::DEBUG, "Scheduling looping for sound ID: " , sound_id, " to " , loop);
    schedule_command(cmd);
}

void SFX::schedule_command(const AudioCommand& cmd) {
    std::lock_guard<std::mutex> lock(command_mutex);
    command_queue.enqueue(cmd, 1); 
    cv.notify_one();
}

SoundRes* SFX::find_sound(unsigned int sound_id) {
    for (SoundRes& s : sounds) {
        if (s.id == sound_id) {
			// LOG(LogLevel::DEBUG, "Found sound with ID: " , sound_id);
            return &s;
        }
    }
	LOG(LogLevel::EROR, "Sound not found: " , sound_id);
    return nullptr;
}

void SFX::internal_play(unsigned int sound_id, float volume, float pitch, bool loop, float pan) {
    SoundRes* s = find_sound(sound_id);
    if (!s) {
        return;
    }

    if (s->is_playing) {
        return;
    }

    ma_sound_set_volume(&s->sound, volume);
    ma_sound_set_pitch(&s->sound, pitch);
    ma_sound_set_pan(&s->sound, pan);
    ma_sound_set_looping(&s->sound, loop ? MA_TRUE : MA_FALSE);
    ma_sound_start(&s->sound);
    s->is_playing = true;

    LOG(LogLevel::DEBUG, "Sound started: " , sound_id);
}

void SFX::internal_stop(unsigned int sound_id) {
    SoundRes* s = find_sound(sound_id);
    if (!s || !s->is_playing) return;

    ma_sound_stop(&s->sound);
    s->is_playing = false;
    LOG(LogLevel::DEBUG, "Sound stopped: " , sound_id);
}

void SFX::internal_set_volume(unsigned int sound_id, float volume) {
    SoundRes* s = find_sound(sound_id);
    if (s) {
        ma_sound_set_volume(&s->sound, volume);
    }
}

void SFX::internal_set_pitch(unsigned int sound_id, float pitch) {
    SoundRes* s = find_sound(sound_id);
    if (s) {
        ma_sound_set_pitch(&s->sound, pitch);
    }
}

void SFX::internal_set_pan(unsigned int sound_id, float pan) {
    SoundRes* s = find_sound(sound_id);
    if (s) {
        ma_sound_set_pan(&s->sound, pan);
    }
}

void SFX::internal_set_looping(unsigned int sound_id, bool loop) {
    SoundRes* s = find_sound(sound_id);
    if (s) {
        ma_sound_set_looping(&s->sound, loop ? MA_TRUE : MA_FALSE);
    }
}

void SFX::process_command(const AudioCommand& cmd) {
    switch (cmd.type) {
    case AudioCommandType::PLAY:
        internal_play(cmd.sound_id, cmd.volume, cmd.pitch, cmd.loop, cmd.pan);
        break;
    case AudioCommandType::STOP:
        internal_stop(cmd.sound_id);
        break;
    case AudioCommandType::SET_VOLUME:
		LOG(LogLevel::DEBUG, "Volume changed to ", cmd.volume, " for sound ID: ", cmd.sound_id);
        internal_set_volume(cmd.sound_id, cmd.volume);
        break;
    case AudioCommandType::SET_PITCH:
		LOG(LogLevel::DEBUG, "Pitch changed to ", cmd.pitch, " for sound ID: ", cmd.sound_id);
        internal_set_pitch(cmd.sound_id, cmd.pitch);
        break;
    case AudioCommandType::SET_PAN:
		LOG(LogLevel::DEBUG, "Pan changed to ", cmd.pan, " for sound ID: ", cmd.sound_id);
        internal_set_pan(cmd.sound_id, cmd.pan);
        break;
    case AudioCommandType::SET_LOOPING:
		LOG(LogLevel::DEBUG, "Looping changed to ", cmd.loop, " for sound ID: ", cmd.sound_id);
        internal_set_looping(cmd.sound_id, cmd.loop);
        break;
    default:
        break;
    }
}

void SFX::worker_loop() {
    while (worker_should_run) {

        {
            std::unique_lock<std::mutex> lock(command_mutex);
            if (!command_queue.empty()) {
                AudioCommand cmd = command_queue.dequeue(QueueProtocol::OLDEST).data;
                lock.unlock();
                process_command(cmd);
                lock.lock();
                continue;
            }
        }

        SFXLoadRequest task;
        bool has_task = false;
        {
            std::unique_lock<std::mutex> lock(to_worker_mutex);
            if (!to_worker_queue.empty()) {
                task = to_worker_queue.dequeue(QueueProtocol::OLDEST).data;
                has_task = true;
            }
        }

        if (has_task) {
            unsigned int sound_handle = load(task.path.c_str());
            {
                std::lock_guard<std::mutex> lock(to_main_mutex);
                SFXLoadResult data = { task.registry_ref, sound_handle };
                to_main_queue.enqueue(data, 1);
            }
            continue;
        }

        std::unique_lock<std::mutex> lock(command_mutex);
        cv.wait_for(lock, std::chrono::milliseconds(10), [this] {
            return !command_queue.empty() || !to_worker_queue.empty() || !worker_should_run;
            });
    }
}

unsigned int SFX::get_id() {
    static unsigned int id = 0;
    return id++;
}
void SFX::poll_loaded_sounds(){
    std::lock_guard<std::mutex> lock(to_main_mutex);
    while(!to_main_queue.empty()){
        SFXLoadResult finished_task = to_main_queue.dequeue(QueueProtocol::OLDEST).data;
        lua->resolve_promise(finished_task.value, finished_task.registry_ref);
    }
}
void SFX::schedule_load(const std::string& path, int registry_ref){
    std::lock_guard<std::mutex> lock(to_worker_mutex);
    SFXLoadRequest request = {registry_ref, path};
    to_worker_queue.enqueue(request,1);
}
