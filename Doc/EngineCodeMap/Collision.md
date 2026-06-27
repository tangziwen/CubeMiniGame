# CubeEngine/EngineSrc/Collision

## Role

Bullet physics integration, rigid bodies, shapes, constraints, ray casts, and legacy ellipsoid collision.

## Important Objects

- `PhysicsMgr`: Bullet world owner. Creates dynamics world, shapes/bodies/constraints, steps simulation, syncs physics, and performs closest ray cast.
- `PhysicsRigidBody`: wrapper around `btRigidBody`, attached `Drawable3D`, user index, forces/impulses, CCD, mass/friction, and sync policy.
- `PhysicsShape`, `PhysicsCompoundShape`: collision shape wrappers and inertia helpers.
- `PhysicsConstraint`, `PhysicsHingeConstraint`, `Physics6DOFConstraint`, `Physics6DofSpringConstraint`: Bullet constraint wrappers.
- `PhysicsListener`: callback target for physics-to-object sync.
- `ColliderEllipsoid` / `CollisionUtility`: older collide-and-slide ellipsoid path used by camera code.
- `PhysicsHitResult`: ray-cast result.

## Contracts

- Step is driven by `PhysicsMgr::stepSimulation()` from `Engine::update()`.
- Use `PhysicsRigidBody::SyncPolicy` to decide whether physics updates transform, position only, or nothing.
- Add/remove bodies and constraints through `PhysicsMgr` so Bullet world membership stays consistent.

## Boundaries

- Physics should sync through wrapper/listener APIs, not direct Bullet access from scene objects.

## Known Traps

- `PhysicsMgr` owns Bullet collision shapes in `m_collisionShapes`; avoid external deletion of raw Bullet shapes.
