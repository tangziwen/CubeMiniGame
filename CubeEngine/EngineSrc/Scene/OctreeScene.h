#ifndef TZW_OCTREESCENE_H
#define TZW_OCTREESCENE_H

#include <functional>

#include "../Math/AABB.h"
#include "../Math/Ray.h"
#include <unordered_set>
#include <unordered_map>
namespace tzw {
class Drawable3D;
class Camera;
struct OctreeNode
{
    OctreeNode();
    AABB aabb;
    std::vector<Drawable3D *> m_drawlist;
    OctreeNode * m_child[8];
	int m_index;
	void genId();
};

class OctreeScene
{
public:
    OctreeScene();
    void init(AABB range);
    void addObj(Drawable3D * obj);
    void removeObj(Drawable3D * obj);
    void updateObj(Drawable3D * obj);
    bool hitByRay(const Ray &ray, vec3 &hitPoint);
    void cullingByCamera(Camera * camera, uint32_t renderStageFlag);
	void cullingByCameraExtraFlag(Camera * camera, uint32_t flags, uint32_t renderStageFlag, std::vector<Drawable3D *> & resultList);
    void getRange(std::vector<Drawable3D *> * list, uint32_t flags, AABB aabb);
    int getAmount();
    std::vector<Drawable3D *>& getVisibleList();
	bool isInOctree(Drawable3D * obj);
	OctreeNode * getNodeByIndex(int index);
private:
    int getAmount_R(OctreeNode * node);
	void cullingByCameraFlag_R(OctreeNode * node,Camera * camera, uint32_t flags, uint32_t renderStageFlag, std::vector<Drawable3D *> & resultList);
    void cullingImp_R(OctreeNode * node, uint32_t flags, std::vector<Drawable3D *> * list, const std::function<bool(const AABB&)>& testFunc);
    OctreeNode * m_root;
    bool addObj_R(OctreeNode * node,Drawable3D * obj);
    bool removeObj_R(OctreeNode * node,Drawable3D * obj);
    bool hitByRay_R(OctreeNode * node,const Ray &ray, vec3 & hitPoint);
    void subDivide(OctreeNode * node,int level);
    std::vector<Drawable3D *> m_visibleList;
	std::unordered_set<Drawable3D *> m_objSet;
	std::unordered_map<Drawable3D *, int> m_objMap;
	std::vector<OctreeNode *> m_nodeList;
};

} // namespace tzw

#endif // TZW_OCTREESCENE_H
