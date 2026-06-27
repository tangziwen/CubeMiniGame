# CubeEngine/EngineSrc/Action

## Role

Small action system for time-based mutation of `Node`s.

## Important Objects

- `Action`: base step/update/final/isDone interface.
- `ActionMgr`: list of active actions; inherited by `Node`.
- `ActionInterval`: duration/current-time base for finite actions.
- `ActionSequence`: ordered list of actions.
- `RepeatForever`: wraps an interval action and restarts it.
- `MoveTo`, `MoveBy`, `RotateTo`, `RotateBy`, `TintTo`: common transform/color actions.
- `ActionCalFunc`, `ActionRemoveMySelf`: callback/removal actions.
