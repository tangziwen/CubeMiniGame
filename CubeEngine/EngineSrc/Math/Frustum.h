#ifndef TZW_FRUSTUM_H
#define TZW_FRUSTUM_H

#include "AABB.h"
#include "Plane.h"
namespace tzw {
class Camera;
class Frustum
{
    friend class Camera;
public:
    /**
     * Constructor & Destructor.
     */
    Frustum(): _initialized(false), _clipZ(true){}
    ~Frustum(){}

    /**
     * init frustum from camera.
     */
    bool initFrustumFromCamera(Camera* camera);

    /**
     * init frustum from camera.
     */
    bool initFrustumFromProjectMatrix(QMatrix4x4 matrix);
    /**
     * is aabb out of frustum.
     */
    bool isOutOfFrustum(const AABB& aabb) const;

    /**
     * get & set z clip. if bclipZ == true use near and far plane
     */
    void setClipZ(bool clipZ) { _clipZ = clipZ; }
    bool isClipZ() { return _clipZ; }

protected:
    /**
     * create clip plane
     */
    void createPlane( Camera* camera);

    /**
     * create clip plane
     */
    void createPlane(QMatrix4x4 matrix);

    Plane _plane[6];             // clip plane, left, right, top, bottom, near, far
    bool _clipZ;                // use near and far clip plane
    bool _initialized;
};

} // namespace tzw

#endif // TZW_FRUSTUM_H
