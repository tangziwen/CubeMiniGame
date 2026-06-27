#include "Drawable3D.h"
#include "../Technique/MaterialInstance.h"
#include "../Scene/Scene.h"
#include <algorithm>
#include "../Scene/OctreeScene.h"
namespace tzw {


Drawable3D::Drawable3D()
{
	setLocalPriority(-999);
	m_octNodeIndex = -1;
	m_drawableFlag = static_cast<uint32_t>(DrawableFlag::Drawable);
	m_renderStageFlag = static_cast<uint32_t>(RenderFlag::RenderStage::Unset);
	m_outlineEnabled = false;
	m_outlineColor = vec4(1.0f, 0.85f, 0.15f, 1.0f);
}

Drawable3D::~Drawable3D()
{
    if(this->Node::getIsAccpectOcTtree())
    {
        m_scene->getOctreeScene()->removeObj(this);
    }
}
AABB Drawable3D::localAABB() const
{
    return m_localAABB;
}

void Drawable3D::setLocalAABB(const AABB &localAABB)
{
    m_localAABB = localAABB;
}

AABB Drawable3D::getAABB()
{

    return m_worldAABBCache;
}

Node::NodeType Drawable3D::getNodeType()
{
    return NodeType::Drawable3D;
}

bool Drawable3D::intersectByAABB(const AABB &other, vec3 &overLap)
{
    return getAABB().isIntersect(other,overLap);
}

Drawable3D *Drawable3D::intersectByRay(const Ray &ray, vec3 &hitPoint)
{
    if(ray.intersectAABB(this->getAABB(),nullptr,hitPoint))
    {
        return this;
    }else
    {
        return nullptr;
    }
}

bool Drawable3D::intersectBySphere(const t_Sphere &sphere, std::vector<vec3> &hitPoint)
{
    return false;
}

void Drawable3D::reCache()
{
    Drawable::reCache();
    reCacheAABB();
}

void Drawable3D::reCacheAABB()
{
    if(getNeedToUpdate())
    {
        m_worldAABBCache = m_localAABB;
        m_worldAABBCache.transForm(getTransform());
    }
}

bool Drawable3D::getIsHitable() const
{
	return m_isHitable;
}

void Drawable3D::setIsHitable(bool val)
{
	m_isHitable = val;
}

void Drawable3D::setUpCommand(RenderCommand & command)
{
	Drawable::setUpCommand(command);
	command.setOutlineColor(m_outlineColor);
}

int Drawable3D::setOctNodeIndex(int index)
{
	m_octNodeIndex = index;
	return index;
}

int Drawable3D::getOctNodeIndex()
{
	return m_octNodeIndex;
}

Mesh* Drawable3D::getMesh(int index)
{
	return nullptr;
}

Mesh* Drawable3D::getMesh()
{
	return nullptr;
}

int Drawable3D::getMeshCount()
{
	return 1;
}

void Drawable3D::setColor(vec4 newColor)
{
}

uint32_t Drawable3D::getDrawableFlag() const
{
	return m_drawableFlag;
}

void Drawable3D::setDrawableFlag(const uint32_t drawableFlag)
{
	m_drawableFlag = drawableFlag;
}

uint32_t Drawable3D::getRenderStage(MaterialInstance* materialHint) const
{
	if(m_renderStageFlag != static_cast<uint32_t>(RenderFlag::RenderStage::Unset))
	{
		return m_renderStageFlag;
	}
	if(materialHint)
	{
		const uint32_t materialStage = static_cast<uint32_t>(materialHint->getRenderStage());
		if(materialStage != static_cast<uint32_t>(RenderFlag::RenderStage::Unset))
		{
			return materialStage;
		}
	}
	return static_cast<uint32_t>(RenderFlag::RenderStage::COMMON);
}

uint32_t Drawable3D::getRenderStageForRequest(MaterialInstance* materialHint, uint32_t requestedStageMask) const
{
	if(requestedStageMask == static_cast<uint32_t>(RenderFlag::RenderStage::Unset)
		|| requestedStageMask == static_cast<uint32_t>(RenderFlag::RenderStage::All))
	{
		return getRenderStage(materialHint);
	}

	const uint32_t explicitStage = m_renderStageFlag;
	if(explicitStage != static_cast<uint32_t>(RenderFlag::RenderStage::Unset))
	{
		const uint32_t explicitMatch = explicitStage & requestedStageMask;
		if(explicitMatch != 0)
		{
			return explicitMatch;
		}

		const bool shadowOnlyRequest = (requestedStageMask & ~static_cast<uint32_t>(RenderFlag::RenderStage::SHADOW)) == 0;
		if(shadowOnlyRequest)
		{
			return static_cast<uint32_t>(RenderFlag::RenderStage::Unset);
		}
	}

	uint32_t materialStage = materialHint ? static_cast<uint32_t>(materialHint->getRenderStage())
		: static_cast<uint32_t>(RenderFlag::RenderStage::COMMON);
	if(materialStage == static_cast<uint32_t>(RenderFlag::RenderStage::Unset))
	{
		materialStage = static_cast<uint32_t>(RenderFlag::RenderStage::COMMON);
	}
	return materialStage & requestedStageMask;
}

void Drawable3D::setOutlineEnabled(bool enabled)
{
	m_outlineEnabled = enabled;
}

bool Drawable3D::isOutlineEnabled() const
{
	return m_outlineEnabled;
}

void Drawable3D::setOutlineColor(vec4 color)
{
	m_outlineColor = color;
}

vec4 Drawable3D::outlineColor() const
{
	return m_outlineColor;
}

void Drawable3D::getInstancedData(std::vector<InstanceRendereData> & instanceDataList)
{
    return;
}

Drawable3DGroup::Drawable3DGroup(Drawable3D **obj, int count)
{
	init(obj, count);
}

Drawable3DGroup::Drawable3DGroup()
{

}

void Drawable3DGroup::init(Drawable3D ** obj, int count)
{
	for (int i = 0; i < count; i++)
	{
		m_list.push_back(obj[i]);
	}
}

struct tmpData
{
    Drawable3D * obj;
    vec3 hitPoint;
};

struct tmpDataList
{
    Drawable3D * obj;
    std::vector<vec3> hitPoint;
};
Drawable3D *  Drawable3DGroup::hitByRay(const Ray &ray, vec3 &hitPoint) const
{
    std::vector<tmpData > resultList;
    for(int i =0;i<m_list.size();i++)
    {
        Drawable3D * obj = m_list[i];
		if (! obj->getIsHitable())
			continue;
        auto hitP = vec3();
        if(obj->intersectByRay(ray, hitP))
        {
            tmpData data;
            data.obj = obj;
            data.hitPoint = hitP;
            resultList.push_back(data);
        }
    }
    //sort by Dist
    if (!resultList.empty())
    {
        vec3 minHitP = resultList[0].hitPoint;
        Drawable3D * minObj = resultList[0].obj;
        for( auto item : resultList)
        {
            if(item.hitPoint.distance(ray.origin()) < minHitP.distance(ray.origin()))
            {
                minObj = item.obj;
                minHitP = item.hitPoint;
            }
        }
        hitPoint = minHitP;
        return minObj;
    }else
    {
        return nullptr;
    }
}

bool Drawable3DGroup::hitByAABB(AABB &aabb,vec3 & minmalOverLap)
{
    bool isHit =false;
    vec3 resultOverLap;
    for(int i =0;i<m_list.size();i++)
    {
        Drawable3D * obj = m_list[i];
        vec3 tmp;
        if(obj->intersectByAABB(aabb,tmp))
        {
            isHit = true;
            if(fabs(tmp.x) >fabs(resultOverLap.x))
            {
                resultOverLap.x = tmp.x;
            }
            if(fabs(tmp.y) >fabs(resultOverLap.y))
            {
                resultOverLap.y = tmp.y;
            }
            if(fabs(tmp.z) >fabs(resultOverLap.z))
            {
                resultOverLap.z = tmp.z;
            }
        }
    }
    if(isHit)
    {
        minmalOverLap = resultOverLap;
    }
    return isHit;
}

bool Drawable3DGroup::hitBySphere(t_Sphere &sphere, std::vector<vec3>& hitPoint)
{
    std::vector<tmpDataList> resultList;
    for(int i =0;i<m_list.size();i++)
    {
        Drawable3D * obj = m_list[i];
        if(obj->intersectBySphere(sphere,hitPoint))
        {
            tmpDataList data;
            data.obj = obj;
            data.hitPoint = hitPoint;
            resultList.push_back(data);
        }
    }
    //sort by Dist
    if (!resultList.empty())
    {
        std::sort(resultList.begin(),resultList.end(),[sphere](const tmpDataList& p1, const tmpDataList& p2)    {
            float dist1 = sphere.centre().distance(p1.obj->getPos());
            float dist2 = sphere.centre().distance(p2.obj->getPos());
            return dist1<dist2;
        });
        hitPoint = resultList[0].hitPoint;
        return true;
    }else
    {
        return false;
    }
}

void Drawable3DGroup::checkCollide(ColliderEllipsoid *package)
{

}



} // namespace tzw

