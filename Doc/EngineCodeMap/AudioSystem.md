# CubeEngine/EngineSrc/AudioSystem

## Role

Audio playback wrapper, default one-shot sounds, listener parameters, and active sound events.

## Important Objects

- `AudioSystem`: singleton audio system pointer, sound creation, default sound pool, update, one-shot playback, and listener transform.
- `Audio`: loaded sound wrapper; supports looping, 2D/3D play, and fire-and-forget play.
- `AudioEvent`: active channel/event handle with pause, volume, stop, and 3D position.
