#include "miniaudio/miniaudio.h"
#include "config.h"
struct SoundRes{
    unsigned int id;
    ma_sound sound;
};
class SFX{
    public:
    SFX();
    ~SFX();
    unsigned int load(const std::string& path);
    void play(unsigned int sound_id);

    private:
    unsigned int get_id();
    ma_engine engine;
    std::list<SoundRes> sounds;
};