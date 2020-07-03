#include "OctreeScene.h"
#include "../Interface/Drawable3D.h"
#include <algorithm>
#include "../base/Camera.h"
#include <cassert>
#include "3D/Primitive/CubePrimitive.h"
#define MAX_DEEP 5
namespace tzw {
static int g_nodeIndex = 0;
void OctreeNode::genId()
{
	m_index = g_nodeIndex;
	g_nodeIndex += 1;
}
OctreeNode::OctreeNode()
{
    for(int i =0;i<8;i++)
    {
        m_child[i] = nullptr;
    }
	genId();
}

OctreeScene::OctreeScene(): m_root(nullptr)
{
}

void OctreeScene::init(AABB range)
{
    m_root = new OctreeNode();
    m_root->aabb = range;
	m_nodeList.push_back(m_root);
    subDivide(m_root,1);
}

void OctreeScene::subDivide(OctreeNode *node, int level)
{
    if(level> MAX_DEEP) {return;}
    auto subAABBList = node->aabb.split8();
    for(int i =0;i<8;i++)
    {
		auto child = new OctreeNode();
		m_nodeList.push_back(child);
        child->aabb = subAABBList[i];
		node->m_child[i] = child;
        subDivide(child,level + 1 );
    }
}

std::vector<Drawable3D *> &OctreeScene::getVisibleList()
{
    return m_visibleList;
}

bool OctreeScene::isInOctree(Drawable3D * obj)
{
	return m_objSet.find(obj) != m_objSet.end();
}

OctreeNode* OctreeScene::getNodeByIndex(int index)
{
	return m_nodeList[index];
}

void OctreeScene::addObj(Drawable3D *obj)
{
    auto result = addObj_R(m_root,obj);
    if(result)
    {
		m_objSet.insert(obj);
	}else
	{
		//printf("can't add Object\n");
		//assert(0);
	}
}

bool OctreeScene::addObj_R(OctreeNode *node, Drawable3D *obj)
{
    if(node->aabb.isCanCotain(obj->getAABB()))
    {
        if(!node->m_child[0])//terminal node
        {
            node->m_drawlist.push_back(obj);
			obj->setOctNodeIndex(node->m_index);
            return true;
        }else {
            for(int i =0;i<8;i++)
            {
                if(addObj_R(node->m_child[i],obj))
                {
                    return true;
                }
            }
			// the children can containt that ,so we only can put it in parent.
			node->m_drawlist.push_back(obj);
			return true;
        }
    }
    return false;
}

void OctreeScene::removeObj(Drawable3D *obj)
{
    removeObj_R(m_root,obj);

	{	
		auto iter = m_objMap.find(obj);
		if (iter != m_objMap.end())
		{
			m_objMap.erase(iter);
		}
	}

	{	
		auto iter = m_objSet.find(obj);
		if (iter != m_objSet.end())
		{
			m_objSet.erase(iter);
		}
	}

}

bool OctreeScene::removeObj_R(OctreeNode *node, Drawable3D *obj)
{
    auto result = std::find(node->m_drawlist.begin(),node->m_drawlist.end(),obj);
    if(result != node->m_drawlist.end())
    {
        node->m_drawlist.erase(result);
        return true;
    }else
    {
        if(!node->m_child[0]) return false;//terminal node.

        for(int i =0;i<8;i++)
        {
            if(removeObj_R(node->m_child[i],obj))
            {
                return true;
            }
        }
        return false;
    }
}

bool OctreeScene::hitByRay_R(OctreeNode *node, const Ray &ray, vec3 &hitPoint)
{
    vec3 result;
    if(ray.intersectAABB(node->aabb,nullptr,result))
    {
        //check self
        for(int i =0;i<node->m_drawlist.size();i++)
        {
            Drawable3D * obj = node->m_drawlist[i];
            if(ray.intersectAABB(obj->getAABB(),nullptr,hitPoint))
            {
                return true;
            }
        }
        //check sub
        if(!node->m_child[0]) return false; // is terminal
        for(int i = 0;i<8;i++)
        {
            if(hitByRay_R(node->m_child[i],ray,hitPoint))
            {
                return true;
            }
        }
        return false;
    }else
    {
        return false;
    }
}

void OctreeScene::updateObj(Drawable3D *obj)
{
	if(obj->getOctNodeIndex() >= 0)
	{
		auto node = getNodeByIndex(obj->getOctNodeIndex());
		if(node->aabb.isCanCotain(obj->getAABB()))// no need to update;
		{
			return;
		}
	}
	
    removeObj(obj);
    addObj(obj);
}

bool OctreeScene::hitByRay(const Ray &ray, vec3 &hitPoint)
{
    return hitByRay_R(m_root,ray,hitPoint);
}

void OctreeScene::cullingByCamera(Camera *camera)
{
    //clear visible List;
    m_visibleList.clear();
	cullingByCameraExtraFlag(camera, static_cast<uint32_t>(DrawableFlag::Drawable), m_visibleList);
}

void OctreeScene::cullingByCameraExtraFlag(Camera* camera, uint32_t flags, std::vector<Drawable3D*>& resultList)
{
	cullingByCameraFlag_R(m_root,camera, flags, resultList);
    //auto test = [camera](const AABB& targetAABB){return !camera->isOutOfFrustum(targetAABB);};
	//cullingImp_R(m_root,flags, &resultList,test);
}

void OctreeScene::getRange(std::vector<Drawable3D *> *list, uint32_t flags, AABB aabb)
{
    auto test = [&aabb](const AABB& targetAABB){vec3 noNeedVar; return aabb.isIntersect(targetAABB, noNeedVar);};
    cullingImp_R(m_root,flags, list,test);
}

int OctreeScene::getAmount()
{
    return getAmount_R(m_root);
}

int OctreeScene::getAmount_R(OctreeNode *node)
{
    int the_size = node->m_drawlist.size();
    if(!node->m_child[0]) return the_size;
    for(int i =0;i<8;i++)
    {
        the_size += getAmount_R(node->m_child[i]);
    }
    return the_size;
}
static Camera * currentCamera = nullptr;
static int compare(const void * a, const void * b)
{
	auto nodeA = (OctreeNode *)a;
	auto nodeB = (OctreeNode *)b;
    auto vA = nodeA->aabb.centre() - currentCamera->getPos();
    auto vB = nodeB->aabb.centre() - currentCamera->getPos();
    auto distA  = fabs(vA.x) + fabs(vA.y) + fabs(vA.z);
    auto distB  = fabs(vB.x) + fabs(vB.y) + fabs(vB.z);
    if(distA > distB)
    {
        return 1;
    }
    else if(fabs(distA  - distB) < 0.00001)
    {
        return 0;
    }else
    {
        return -1;
    }
}

void OctreeScene::cullingByCameraFlag_R(OctreeNode* node, Camera* camera, uint32_t flags, std::vector<Drawable3D*>& resultList)
{
	if(!camera->isOutOfFrustum(node->aabb))
    {
        for(int i=0;i<node->m_drawlist.size();i++)
        {
            Drawable3D * obj = node->m_drawlist[i];
            if(!camera->isOutOfFrustum(obj->getAABB()))
            {
            	if(obj->getDrawableFlag() & flags)
            	{
            		resultList.push_back (obj);
            	}
            }
        }
        if(!node->m_child[0]) return;//terminal node return directly
        for(int i=0;i<8;i++)
        {
            cullingByCameraFlag_R(node->m_child[i],camera, flags, resultList);
        }
    }
}

void OctreeScene::cullingImp_R(OctreeNode *node, uint32_t flags,  std::vector<Drawable3D *> *list, const std::function<bool(const AABB&)>& testFunc)
{
	if(testFunc(node->aabb))
    {
        //put self
        for(auto drawObj : node->m_drawlist)
        {
            if(drawObj->getDrawableFlag() & flags)
            {
        		if(testFunc(drawObj->getAABB()))
	            {
	                list->push_back(drawObj);
	            }
            }
        }
        //check sub
        if(!node->m_child[0]) return;
        for(int i =0;i<8;i++)
        {
            cullingImp_R(node->m_child[i], flags, list, testFunc);
        }
    }
}




} // namespace tzw

