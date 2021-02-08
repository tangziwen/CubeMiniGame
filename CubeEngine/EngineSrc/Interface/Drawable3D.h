#pragma once

#include "../Math/AABB.h"
#include "Drawable.h"
#include "DepthPolicy.h"
#include "../Math/t_Sphere.h"
#include "../Collision/ColliderEllipsoid.h"
#include "Utility/log/Log.h"
namespace tzw {
class Ray;
enum class DrawableFlag
{
	Drawable = 1 << 1,
    PointLight = 1 << 2,
	Instancing = 1 << 3,

	All = -1,//all bits are 1
};
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
	bool getIsHitable() const;
	void setIsHitable(bool val);
	virtual void setUpCommand(RenderCommand & command);
	int setOctNodeIndex(int index);
	int getOctNodeIndex();
	virtual Mesh * getMesh(int index);
	virtual Mesh * getMesh();
	virtual int getMeshCount();
	void setColor(vec4 newColor) override;
	uint32_t getDrawableFlag() const;
	void setDrawableFlag(const uint32_t drawableFlag);
	virtual void getInstancedData(std::vector<InstanceRendereData> & dataList);
protected:
    AABB m_localAABB;
    AABB m_worldAABBCache;
	bool m_isHitable;
	int m_octNodeIndex;
	uint32_t m_drawableFlag;
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

