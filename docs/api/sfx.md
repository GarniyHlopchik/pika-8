### SFX
Low-level API for playing sounds.

## SFX.load(path: String) -> int

Takes in path to a sound file, loads it and returns an integer that represents its id

## SFX.play(sound_id: int, volume: float, pitch: float, loop: Boolean, pan: float)

Takes in sound id obtained from load and plays said sound.
All parametrs, apart from sound_id, are optional. `voulume` represents how loud the sound will be.
To play same sound multiple times at the same moment you *must* create saperate objects with different IDs

## SFX.stop(sound_id: int)

Stops the sound from playing