#include "OctreeScene.h"
#include "../Interface/Drawable3D.h"
#include <algorithm>
#include "../base/Camera.h"
#define MAX_DEEP 4
namespace tzw {

OctreeScene::OctreeScene()
{

}

void OctreeScene::init(AABB range)
{
    m_root = new OctreeNode();
    m_root->aabb = range;
    subDivide(m_root,1);
}

void OctreeScene::subDivide(OctreeNode *node, int level)
{
    if(level> MAX_DEEP) return;
    auto subAABBList = node->aabb.split8();
    for(int i =0;i<8;i++)
    {
        node->m_child[i] = new OctreeNode();
        node->m_child[i]->aabb = subAABBList[i];
        subDivide(node->m_child[i],level + 1 );
    }
}

std::vector<Drawable3D *> &OctreeScene::getVisibleList()
{
    return m_visibleList;
}

void OctreeScene::addObj(Drawable3D *obj)
{
    auto result = addObj_R(m_root,obj);
    if(!result)
    {
        return;
    }
}

bool OctreeScene::addObj_R(OctreeNode *node, Drawable3D *obj)
{
    if(node->aabb.isCanCotain(obj->getAABB()))
    {
        if(!node->m_child[0])//terminal node
        {
            node->m_drawlist.push_back(obj);
            return true;
        }else {
            for(int i =0;i<8;i++)
            {
                if(addObj_R(node->m_child[i],obj))
                {
                    return true;
                }
                // the children can containt that ,so we only can put it in parent.
                node->m_drawlist.push_back(obj);
                return true;
            }
        }
    }
    return false;
}

void OctreeScene::removeObj(Drawable3D *obj)
{
    removeObj_R(m_root,obj);
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
    removeObj(obj);
    addObj(obj);
}

bool OctreeScene::hitByRay(const Ray &ray, vec3 &hitPoint)
{
    hitByRay_R(m_root,ray,hitPoint);
}

void OctreeScene::cullingByCamera(Camera *camera)
{
    //clear visible List;
    m_visibleList.clear ();
    //reset the state
    setDrawable_R(m_root);
    //camera culling
    cullingByCamera_R(m_root,camera);
}

void OctreeScene::getRange(std::vector<Drawable3D *> *list, AABB aabb)
{
    getRange_R(m_root,list,aabb);
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
    OctreeNode * nodeA = (OctreeNode *)a;
    OctreeNode * nodeB = (OctreeNode *)b;
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

void OctreeScene::cullingByCamera_R(OctreeNode *node, Camera *camera)
{
    if(camera->isOutOfFrustum(node->aabb))
    {
        //set In drawable
        setIndrawable_R(node);
    }else
    {
        for(int i=0;i<node->m_drawlist.size();i++)
        {
            Drawable3D * obj = node->m_drawlist[i];
            if(!camera->isOutOfFrustum(obj->getAABB()))
            {
                obj->setIsDrawable(true);
                m_visibleList.push_back (obj);
            }else
            {
                obj->setIsDrawable(false);
            }
        }

        if(!node->m_child[0]) return;//terminal node return directly
        currentCamera = camera;
        qsort(node->m_child,8,sizeof(OctreeNode *),compare);
        for(int i=0;i<8;i++)
        {
            cullingByCamera_R(node->m_child[i],camera);
        }
    }
}

void OctreeScene::setIndrawable_R(OctreeNode *node)
{
    for(int i=0;i<node->m_drawlist.size();i++)
    {
        Drawable3D * obj = node->m_drawlist[i];
        obj->setIsDrawable(false);
    }
    if(!node->m_child[0]) return;//terminal node return directly
    for(int i=0;i<8;i++)
    {
        setIndrawable_R(node->m_child[i]);
    }
}

void OctreeScene::setDrawable_R(OctreeNode *node)
{
    for(int i=0;i<node->m_drawlist.size();i++)
    {
        Drawable3D * obj = node->m_drawlist[i];
        obj->setIsDrawable(true);
    }
    if(!node->m_child[0]) return;//terminal node return directly
    for(int i=0;i<8;i++)
    {
        setDrawable_R(node->m_child[i]);
    }
}

void OctreeScene::getRange_R(OctreeNode *node, std::vector<Drawable3D *> *list, AABB &aabb)
{
    vec3 noNeedVar;
    if(node->aabb.isIntersect(aabb,noNeedVar))
    {
        //put self
        for(int i =0;i<node->m_drawlist.size();i++)
        {
            Drawable3D* obj = node->m_drawlist[i];
            if(obj->getAABB().isIntersect(aabb,noNeedVar))
            {
                list->push_back(node->m_drawlist[i]);
            }
        }
        //check sub
        if(!node->m_child[0]) return;
        for(int i =0;i<8;i++)
        {
            getRange_R(node->m_child[i],list,aabb);
        }
    }
}


OctreeNode::OctreeNode()
{
    for(int i =0;i<8;i++)
    {
        m_child[i] = nullptr;
    }
}

} // namespace tzw

