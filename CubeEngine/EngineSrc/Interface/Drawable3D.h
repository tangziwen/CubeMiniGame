#pragma once

#include "../Math/AABB.h"
#include "Drawable.h"
#include "DepthPolicy.h"
#include "../Math/t_Sphere.h"
#include "../Collision/ColliderEllipsoid.h"
#include "Utility/log/Log.h"
#include "Rendering/RenderViewType.h"
namespace tzw {
class Ray;
class OctreeScene;
class SceneCuller;
enum class DrawableFlag
{
	Drawable = 1 << 1,
    PointLight = 1 << 2,
	Instancing = 1 << 3,

	All = -1,//all bits are 1
};
class Drawable3D : public Drawable
{
	friend class OctreeScene;
	friend class SceneCuller;
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
	uint32_t getRenderStageFlag() const
	{
		return m_renderStageFlag;
	}
	uint32_t getRenderStage(MaterialInstance* materialHint = nullptr) const;
	uint32_t getRenderStageForRequest(MaterialInstance* materialHint, uint32_t requestedStageMask) const;
	void setRenderStageFlag(const uint32_t renderStage)
	{
		m_renderStageFlag = renderStage;
	}
	void setOutlineEnabled(bool enabled);
	bool isOutlineEnabled() const;
	void setOutlineColor(vec4 color);
	vec4 outlineColor() const;
	void setCastShadow(bool enabled);
	bool isCastShadow() const;
	void setReceiveShadow(bool enabled);
	bool isReceiveShadow() const;
	virtual void getInstancedData(std::vector<InstanceRendereData> & dataList);
	bool acceptsRenderView(RenderViewType viewType) const;
protected:
	void setAcceptsRenderView(RenderViewType viewType, bool enabled);
    AABB m_localAABB;
    AABB m_worldAABBCache;
	bool m_isHitable;
	int m_octNodeIndex;
	uint32_t m_drawableFlag;
	uint32_t m_renderStageFlag;
	uint32_t m_renderViewMask;
	bool m_castShadow;
	bool m_receiveShadow;
	bool m_outlineEnabled;
	vec4 m_outlineColor;
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

