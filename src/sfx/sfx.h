#include <string>
#include "miniaudio/miniaudio.h"
#include "config.h"
#include "globals.h"
#include "data_types/priority_queue.h"
#include <atomic>
#include <mutex>
struct SoundRes{
    unsigned int id;
    bool is_playing = false;
    ma_sound sound;
    Resource data;
    ma_decoder decoder;
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
    SFX();
    ~SFX();

    unsigned int load(const std::string& path);
    void play(unsigned int sound_id, float volume = 1.0f, float pitch = 1.0f, bool loop = false, float pan = 0.0f);
    void stop(unsigned int sound_id);
    void poll_loaded_sounds();
    void schedule_load(const std::string& path, int registry_ref);
    void worker_loop();
private:
    unsigned int get_id();
    ma_engine engine;
    std::list<SoundRes> sounds;
    std::atomic<bool> worker_should_run;
    PriorityQueue<SFXLoadRequest> to_worker_queue;
    PriorityQueue<SFXLoadResult> to_main_queue;
    std::mutex to_worker_mutex;
    std::mutex to_main_mutex;
    std::condition_variable cv;
};