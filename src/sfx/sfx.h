#include "miniaudio/miniaudio.h"
#include "config.h"
struct SoundRes{
    unsigned int id;
    bool is_playing;
    ma_sound sound;
};

class SFX{
public:
    SFX();
    ~SFX();

    unsigned int load(const std::string& path);
    void play(unsigned int sound_id, float volume = 1.0f, float pitch = 1.0f, bool loop = false, float pan = 0.0f);
    void stop(unsigned int sound_id);

private:
    unsigned int get_id();
    ma_engine engine;
    std::list<SoundRes> sounds;
};