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
