# CubeEngine/EngineSrc/AudioSystem

## Role

Audio playback wrapper, default one-shot sounds, listener parameters, and active sound events.

## Important Objects

- `AudioSystem`: singleton audio system pointer, sound creation, default sound pool, update, one-shot playback, and listener transform.
- `Audio`: loaded sound wrapper; supports looping, 2D/3D play, and fire-and-forget play.
- `AudioEvent`: active channel/event handle with pause, volume, stop, and 3D position.

## Contracts

- `AudioSystem::init()` is called from `Engine::onStart()`.
- `AudioSystem::update()` is called once per frame from `Engine::update()`.
- Use `AudioEvent` when playback needs later control; use `playWithOutCare()` for disposable one-shots.

## Boundaries

- Backend audio implementation is hidden behind `void*` handles; avoid leaking concrete audio library types outward.

## Known Traps

- Default sound enum maps to a private pool; verify initialization before assuming a default clip exists.
