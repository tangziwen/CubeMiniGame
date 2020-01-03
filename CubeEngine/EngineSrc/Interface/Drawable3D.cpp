#include "Drawable3D.h"
#include "../Scene/Scene.h"
#include <algorithm>
namespace tzw {

Drawable3D::Drawable3D()
{
	setLocalPiority(-999);
	m_isNeedTransparent = false;
	m_octNodeIndex = -1;
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

DepthPolicy &Drawable3D::getDepthPolicy()
{
    return m_depthPolicy;
}

void Drawable3D::setDepthPolicy(const DepthPolicy &depthPolicy)
{
    m_depthPolicy = depthPolicy;
}

void Drawable3D::checkCollide(ColliderEllipsoid *package)
{

}

bool Drawable3D::getIsHitable() const
{
	return m_isHitable;
}

void Drawable3D::setIsHitable(bool val)
{
	m_isHitable = val;
}

bool Drawable3D::getIsNeedTransparent() const
{
	return m_isNeedTransparent;
}

void Drawable3D::setIsNeedTransparent(bool val)
{
	m_isNeedTransparent = val;
}

void Drawable3D::setUpCommand(RenderCommand & command)
{
	Drawable::setUpCommand(command);
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
        std::sort(resultList.begin(),resultList.end(),[sphere](tmpDataList p1, tmpDataList p2)    {
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
    for(int i =0;i<m_list.size();i++)
    {
        Drawable3D * obj = m_list[i];
        obj->checkCollide(package);
    }
}



} // namespace tzw

