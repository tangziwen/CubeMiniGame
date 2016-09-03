#include "Drawable3D.h"
#include "../Scene/Scene.h"
namespace tzw {

Drawable3D::Drawable3D()
{

}

Drawable3D::~Drawable3D()
{
    if(this->getIsAccpectOCTtree())
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

Drawable3DGroup::Drawable3DGroup(Drawable3D **obj, int count)
{
    for(int i =0;i<count;i++)
    {
        m_list.push_back(obj[i]);
    }
}

Drawable3D *  Drawable3DGroup::hitByRay(const Ray &ray, vec3 &hitPoint)
{
    std::vector<Drawable3D * > resultList;
    for(int i =0;i<m_list.size();i++)
    {
        Drawable3D * obj = m_list[i];
        if(obj->intersectByRay(ray,hitPoint))
        {
            resultList.push_back(obj);
        }
    }
    //sort by Dist
    if (!resultList.empty())
    {
        std::sort(resultList.begin(),resultList.end(),[ray](Drawable3D* p1, Drawable3D* p2)    {
            float dist1 = ray.origin().distance(p1->getAABB().centre());
            float dist2 = ray.origin().distance(p2->getAABB().centre());
            return dist1<dist2;
        });
        return resultList[0];
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



} // namespace tzw

