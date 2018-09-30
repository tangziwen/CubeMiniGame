#ifndef TZW_DRAWABLE3D_H
#define TZW_DRAWABLE3D_H

#include "../Math/AABB.h"
#include "Drawable.h"
#include "DepthPolicy.h"
#include "../Math/t_Sphere.h"
#include "../Collision/ColliderEllipsoid.h"
namespace tzw {
class Ray;

class Drawable3D : public Drawable
{
public:
    Drawable3D();
    virtual ~Drawable3D();
    virtual AABB localAABB() const;
    virtual void setLocalAABB(const AABB &localAABB);
    virtual AABB getAABB();
    virtual NodeType getNodeType();
    virtual bool intersectByAABB(const AABB & other, vec3 &overLap);
    virtual Drawable3D * intersectByRay(const Ray & ray,vec3 &hitPoint);
    virtual bool intersectBySphere(const t_Sphere & sphere, std::vector<vec3> &hitPoint);
    virtual void reCache();
    void reCacheAABB();
    DepthPolicy& getDepthPolicy();
    void setDepthPolicy(const DepthPolicy &depthPolicy);
    virtual void checkCollide(ColliderEllipsoid * package);
	bool getIsHitable() const;
	void setIsHitable(bool val);
	virtual bool getIsNeedTransparent() const;
	virtual void setIsNeedTransparent(bool val);
	virtual void setUpCommand(RenderCommand & command);
	int setOctNodeIndex(int index);
	int getOctNodeIndex();
protected:
    AABB m_localAABB;
    AABB m_worldAABBCache;
    DepthPolicy m_depthPolicy;
	bool m_isHitable;
	bool m_isNeedTransparent;
	int m_octNodeIndex;
};

class Drawable3DGroup
{
public:
    Drawable3DGroup(Drawable3D ** obj,int count);
	Drawable3DGroup();
	void init(Drawable3D ** obj, int count);
    Drawable3D *hitByRay(const Ray& ray,vec3 & hitPoint) const;
    bool hitByAABB(AABB & aabb, vec3 &minmalOverLap);
    bool hitBySphere(t_Sphere & sphere, std::vector<vec3> &hitPoint);
    void checkCollide(ColliderEllipsoid * package);
private:
    std::vector<Drawable3D * >m_list;
};

} // namespace tzw

#endif // TZW_DRAWABLE3D_H
