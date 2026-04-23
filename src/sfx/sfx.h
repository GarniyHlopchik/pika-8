#include <string>
#include "miniaudio/miniaudio.h"
#include "config.h"
#include "globals.h"
#include "data_types/priority_queue.h"
#include <atomic>
#include <mutex>
#include <thread>
#include "lua_system/lua_system.h"
struct SoundRes{
    unsigned int id;
    bool is_playing = false;
    ma_sound sound;
    Resource data;
    ma_decoder decoder;
};
enum class AudioCommandType {
    PLAY,
    STOP,
    SET_VOLUME,
    SET_PITCH,
    SET_PAN,
    SET_LOOPING,
    LOAD
};

struct AudioCommand {
    AudioCommandType type;
    unsigned int sound_id;
    float volume;
    float pitch;
    float pan;
    bool loop;
    std::string path;     
    int registry_ref;      
};
//this is passed around in queues that comunicate with loader worker thread
struct SFXLoadRequest{
    int registry_ref;
    std::string path;
};
struct SFXLoadResult{
    int registry_ref;
    int value;
};

class SFX{
public:
    SFX(LuaSystem* lua);
    ~SFX();

    unsigned int load(const std::string& path);
    void play(unsigned int sound_id, float volume = 1.0f, float pitch = 1.0f, bool loop = false, float pan = 0.0f);
    void stop(unsigned int sound_id);
    void set_volume(unsigned int sound_id, float volume);
    void set_pitch(unsigned int sound_id, float pitch);
    void set_pan(unsigned int sound_id, float pan);
    void set_looping(unsigned int sound_id, bool loop);
    void poll_loaded_sounds();
    void schedule_load(const std::string& path, int registry_ref);
    void schedule_command(const AudioCommand& cmd);
    void worker_loop();
private:
    LuaSystem* lua;
    unsigned int get_id();
    ma_engine engine;
    std::list<SoundRes> sounds;
    std::atomic<bool> worker_should_run;
    PriorityQueue<SFXLoadRequest> to_worker_queue;
    PriorityQueue<SFXLoadResult> to_main_queue;
    std::mutex to_worker_mutex;
    std::mutex to_main_mutex;
    PriorityQueue<AudioCommand> command_queue;
    std::mutex command_mutex;
    std::condition_variable cv;
    std::jthread worker_thread;

    void process_command(const AudioCommand& cmd);
    void internal_play(unsigned int sound_id, float volume, float pitch, bool loop, float pan);
    void internal_stop(unsigned int sound_id);
    void internal_set_volume(unsigned int sound_id, float volume);
    void internal_set_pitch(unsigned int sound_id, float pitch);
    void internal_set_pan(unsigned int sound_id, float pan);
    void internal_set_looping(unsigned int sound_id, bool loop);
    SoundRes* find_sound(unsigned int sound_id);
};