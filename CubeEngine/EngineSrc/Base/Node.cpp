#include "Node.h"

#include "../Interface/Drawable3D.h"
#include "../Scene/SceneMgr.h"
#include "../Engine/Engine.h"
#include "../Scene/Scene.h"
#include <iostream>
#include <algorithm>
#include "../Event/EventMgr.h"
#include "../Rendering/Renderer.h"
namespace tzw {
/**
 * @brief Node::Node 构造函数
 */
Node::Node()
    :m_isVisible(true),
      m_isValid(true),
      m_scene(nullptr),
      m_scale(vec3(1,1,1)),
      m_rotateQ(Quaternion(0.0f,0.0f,0.0f,1.0f)),
      m_pos(vec3()),
      m_needToUpdate(true),
      m_localPiority(0),
      m_globalPiority(0),
      m_isAccpectOCTtree(true),
      m_name("default"),
      m_parent(nullptr)
{
    m_name = "default";
}

Node::~Node()
{

}

/**
 * @brief Node::create 静态工厂方法
 * @return
 */
Node *Node::create()
{
    Node * node = new Node();
    return node;
}

/**
 * @brief Node::getTransform 获取节点的变换矩阵
 * @note 该变换是级联的，包括其父节点(如果有的话)的变换
 * @return 变换矩阵
 */
Matrix44 Node::getTransform()
{
    if(m_needToUpdate)
    {
        cacheTransform();
    }
    return m_worldTransformCache;
}
/**
 * @brief Node::getLocalTransform 获取节点的局部变换矩阵
 * @note 该方法返回的矩阵不回受层次关系影响，仅为本节点的局部变换
 * @return 局部变换矩阵
 */
Matrix44 Node::getLocalTransform()
{
    return getTranslationMatrix() * getRotationMatrix() * getScalingMatrix();
}

/**
 * @brief Node::getPos 获取节点的坐标
 * @return 节点的坐标
 */
vec3 Node::getPos() const
{
    return m_pos;
}

/**
 * @brief Node::setPos 设置节点的坐标
 * @param pos 节点的新坐标
 */
void Node::setPos(const vec3 &pos)
{
    m_pos = pos;
	m_needToUpdate = true;
}

void Node::setPos(float x, float y, float z)
{
	setPos(vec3(x, y, z));
}

void Node::submitDrawCmd()
{

}

void Node::logicUpdate(float dt)
{

}

/**
 * @brief Node::getRotate 获取节点的旋转欧拉角
 * @note 旋转量的x,y,z分量分别表示绕x轴,y轴,z轴旋转的角度(以角度记)，变换的顺序为roll(z)->picth(x)->yaw(y);
 * @return
 */
vec3 Node::getRotateE()
{
    float x = m_rotateE.x, y = m_rotateE.y, z = m_rotateE.z;
    m_rotateQ.toEulserAngel(&x, &y, &z);
    //m_rotateE = vec3(x,y,z);
    return m_rotateE;
    //return vec3 (TbaseMath::Radius2Ang(theEulerAngle.x),TbaseMath::Radius2Ang(theEulerAngle.y),TbaseMath::Radius2Ang(theEulerAngle.z));
}

/**
 * @brief Node::setRotate 设置节点的节点的旋转欧拉角
 * @note 旋转量的x,y,z分量分别表示绕x轴,y轴,z轴旋转的角度(以角度记)，变换的顺序为roll(z)->picth(x)->yaw(y);
 * @param rotate
 */
void Node::setRotateE(const vec3 &rotate)
{
    m_rotateE = rotate;
    //theRotate.x = TbaseMath::clampf(rotate.x,-60.f,60.f);
    //theRotate.y = TbaseMath::clampf(rotate.y,0.f,360.f);
    m_rotateQ.fromEulerAngle(m_rotateE);
    m_needToUpdate = true;
}
/**
 * @brief Node::setRotate设置节点的节点的旋转欧拉角
 * @note 旋转量的x,y,z分量分别表示绕x轴,y轴,z轴旋转的角度(以角度记)，变换的顺序为roll(z)->picth(x)->yaw(y);
 * @param x
 * @param y
 * @param z
 */
void Node::setRotateE(float x, float y, float z)
{
    setRotateE(vec3(x,y,z));
}

/**
 * @brief Node::getScale 获取缩放向量
 * @return
 */
vec3 Node::getScale() const
{
    return m_scale;
}

/**
 * @brief Node::setScale 设置缩放向量
 * @param scale
 */
void Node::setScale(const vec3 &scale)
{
    m_scale = scale;
    m_needToUpdate = true;
}
/**
 * @brief Node::setScale 设置缩放向量
 * @param x
 * @param y
 * @param z
 */
void Node::setScale(float x, float y, float z)
{
    setScale(vec3(x,y,z));
}

/**
 * @brief Node::getTranslationMatrix 获取平移矩阵
 * @return
 */
Matrix44 Node::getTranslationMatrix()
{
    Matrix44 mat;
    mat.translate(m_pos);
    return mat;
}

#define PI_OVER_180 (3.14159 / 180)
/**
 * @brief Node::getRotationMatrix 获取旋转矩阵
 * @return
 */
Matrix44 Node::getRotationMatrix()
{
    Matrix44 rotateMatrix;
    rotateMatrix.setToIdentity();
    m_rotateQ.normalize();
    rotateMatrix.rotate(m_rotateQ);
    return rotateMatrix;
}

/**
 * @brief Node::getScalingMatrix 获取缩放矩阵
 * @return
 */
Matrix44 Node::getScalingMatrix()
{
    Matrix44 s;
    s.scale(m_scale);
    return s;
}
///
/// \brief 递归遍历节点(逻辑更新用)
/// \note 该次递归函数在整个逻辑-渲染流程的最开始，对于使用场景管理的3D物体仅用于逻辑更新用，其递归时不会提交任何的render Command
/// 对于其他类型的Node，会在其执行完逻辑update后，立即
///
void Node::visit()
{
	auto scene = g_GetCurrScene()->getOctreeScene();
    logicUpdate(Engine::shared()->deltaTime());
	if(!m_actionList.empty())
		updateAction(this,Engine::shared()->deltaTime());
	//数据如果有变动的话，重新缓存数据
	if(getNeedToUpdate())
	{
		this->reCache();
		setNeedToUpdate(false); 
	}
	if(m_isVisible && m_isValid)
	{
		if(!getIsAccpectOCTtree() || this->getNodeType() != NodeType::Drawable3D )
		{
			submitDrawCmd();
		}
		if(getIsAccpectOCTtree() && getNodeType()==NodeType::Drawable3D && (getNeedToUpdate() || !scene->isInOctree((Drawable3D *)this)))
		{
			scene->updateObj((Drawable3D *)this);
		}
		////遍历子节点
		for(auto child : m_children)
		{
			child->visit();
		}
	}


	//判断是否需要删除
	if(!m_isValid)
	{
		getParent()->detachChild(this);
		delete this;
	}
}

/**
 * @brief Node::addChild 向该Node添加子Node
 * @note 子Node不能重复添加,每次添加子Node的时候会重新排序子节点的顺序
 * @param node
 */
void Node::addChild(Node *node)
{
    if(node->m_parent)
    {
        Tlog()<<"can not multiple adding";
    }else
    {
        node->setScene(m_scene);
        
        //refresh the new child's transform cache
        node->setNeedToUpdate(true);
        node->m_parent = this;
		//for performance issuse, the Zorder < -99 no need to sort, just insert away.
		if (node->getLocalPiority() < -99)
		{
			m_children.push_front(node);
		}
		else
		{
			m_children.push_back(node);
			//sort the children
			sortChildren();
		}
    }
	EventMgr::shared()->notifyListenerChange();
	Renderer::shared()->notifySortGui();
}

/**
 * @brief Node::getParent 获取该Node的父节点
 * @note 初始时值为nullptr_t
 * @return
 */
Node *Node::getParent() const
{
    return m_parent;
}

/**
 * @brief Node::getName 获取该Node的名称
 * @note 名称并无实际意义，只是方便调试与打印
 * @return
 */
std::string Node::getName() const
{
    return m_name;
}

Node *Node::getChildByName(const std::string &name)
{
    for(auto node :m_children)
    {
        if (node->m_name == name) return node;
    }
    return nullptr;
}

Node *Node::getChildByTag(unsigned int tag)
{
    for(auto node :m_children)
    {
        if (node->getTag() == tag) return node;
    }
    return nullptr;
}

Node *Node::getChildByIndex(size_t index)
{
    return m_children[index];
}

/**
 * @brief Node::setName 设置该Node的名称
 * @note 名称并无实际意义，只是方便调试与打印
 * @param name
 */
void Node::setName(const std::string &name)
{
    m_name = name;
}

void Node::setPos2D(float x, float y)
{
    setPos(vec3(x,y,0));
}

void Node::setPos2D(vec2 pos)
{
    setPos2D(pos.getX(),pos.getY());
}

vec2 Node::getPos2D()
{
    return vec2(m_pos.x,m_pos.y);
}

vec3 Node::getWorldPos()
{
    auto mat = getTransform().data();
    return vec3(mat[12],mat[13],mat[14]);
}

vec2 Node::getWorldPos2D()
{
    auto p = getWorldPos();
    return vec2(p.x,p.y);
}
///
/// \brief 判断该节点的几何信息是否被更新，只有当该函数返回true的时候，一些相关的响应动作：如重新计算矩阵、在八叉树上更新位置才会被触发
/// \return
///
bool Node::getNeedToUpdate() const
{
    bool parentNeedToUpdate = false;
    if(m_parent)
    {
        parentNeedToUpdate = m_parent->getNeedToUpdate();
    }
    return m_needToUpdate || parentNeedToUpdate;
}
///
/// \brief 设置该节点的几何信息是否需要被更新，只有当该函数返回true的时候，一些相关的响应动作：如重新计算矩阵、在八叉树上更新位置才会被触发，
/// 当调用一些涉及到几何信息的函数，如平移、缩放、旋转时，该函数将会被隐式的调用
/// \param needToUpdate
///
void Node::setNeedToUpdate(bool needToUpdate)
{
    m_needToUpdate = needToUpdate;
}

Node::NodeType Node::getNodeType()
{
    return NodeType::NormalNode;
}
bool Node::getIsVisible() const
{
    return m_isVisible;
}

void Node::setIsVisible(bool isVisible)
{
    m_isVisible = isVisible;
}

void Node::purgeAllChildren()
{
    if(!m_children.empty())
    {
        for( Node * node :m_children)
        {
            node->removeAllChildren_r();
        }
        m_children.clear();
    }
}

void Node::removeAllChildren_r()
{
    if(!m_children.empty())
    {
        for( Node * node :m_children)
        {
            node->removeAllChildren_r();
        }
        m_children.clear();
    }
    delete this;
}

///
/// \brief 获得全局渲染优先级
/// @note 全局渲染优先级和层级无关，并直接作用于renderComand中的zorder
/// \return 全局渲染优先级
///
unsigned int Node::getGlobalPiority() const
{
    return m_globalPiority;
}

///
/// \brief 设置全局渲染优先级
/// @note 全局渲染优先级和层级无关，并直接作用于renderComand中的zorder,
/// \param globalPiority 全局渲染优先级
///
void Node::setGlobalPiority(unsigned int globalPiority)
{
    m_globalPiority = globalPiority;
	EventMgr::shared()->notifyListenerChange();
	Renderer::shared()->notifySortGui();
}

size_t Node::getChildrenAmount()
{
    return m_children.size();
}

unsigned int Node::getTag() const
{
    return m_tag;
}

void Node::setTag(unsigned int tag)
{
    m_tag = tag;
}

unsigned int Node::getTypeID()
{
	return 0;
}

///
/// \brief 获取该节点是否能够被场景的八叉树管理器的影响，八叉树管理器会影响到该节点的可绘制性。
/// \note 八叉树管理器使用惰性添加的方式处理新的节点，最快能够在其加入到场景树后能够直接被绘制的第二帧开始受到八叉树管理器的管理
/// \return
///
bool Node::getIsAccpectOCTtree() const
{
    return m_isAccpectOCTtree;
}

///
/// \brief 设置该节点是否能够被场景的八叉树管理器的影响，八叉树管理器会影响到该节点的可绘制性。
/// \note 八叉树管理器使用惰性添加的方式处理新的节点，最快能够在其加入到场景树后能够直接被绘制的第二帧开始受到八叉树管理器的管理，
/// 设置是否被八叉树管理只影响到其可绘制性，用户仍然可以强制设置可绘制性的方式来进行渲染
/// \return
///
void Node::setIsAccpectOCTtree(bool isAccpectOCTtree)
{
    m_isAccpectOCTtree = isAccpectOCTtree;
}

void Node::cacheTransform()
{
    if(getNeedToUpdate())
    {
        Matrix44 parentTransform;
        parentTransform.setToIdentity();
        if(m_parent)
        {
            parentTransform = m_parent->getTransform();
        }
        Matrix44 localTransform = getLocalTransform();
        m_worldTransformCache = parentTransform * localTransform;
    }
}

///
/// \brief 重新缓存各种几何信息，世界矩阵，世界坐标下的AABB等
///
void Node::reCache()
{
    cacheTransform();
	for (auto child : m_children)
	{
		child->reCache();
	}
	//将重置缓存标记

}

bool Node::getIsValid() const
{
    return m_isValid;
}

void Node::setIsValid(bool isValid)
{
    m_isValid = isValid;
}

///
/// \brief 将一个指定的子节点分离出去
/// \param node
///
void Node::detachChild(Node *node)
{
    auto result = std::find(m_children.begin(),m_children.end(),node);
    if (result != m_children.end())
    {
        m_children.erase(result);
    }
}

///
/// \brief 从父节点处删除
///
void Node::removeFromParent()
{
    setIsValid(false);
}

void Node::setParent(Node *parent)
{
    m_parent = parent;
}

vec3 Node::getForward()
{
    auto m = getTransform().data();
    return vec3(-m[8],-m[9],-m[10]);
}

///
/// \brief 设置局部渲染优先级
/// @note 局部渲染优先级，只与本层级有关，关系到该节点在本层次被渲染时遍历的先后顺序(即与兄弟之间的次序)，但是其对应rendering Command 的Zorder仍然保持不变，故
/// 不能影响到全局渲染优先级
/// \param zOrder 局部渲染优先级
///
int Node::getLocalPiority() const
{
    return m_localPiority;
}

///
/// \brief 设置局部渲染优先级
/// @note 局部渲染优先级，只与本层级有关，关系到该节点在本层次被渲染时遍历的先后顺序(即与兄弟之间的次序)，但是其对应rendering Command 的Zorder仍然保持不变，故
/// 不能影响到全局渲染优先级
/// \param zOrder 局部渲染优先级
///
void Node::setLocalPiority(int zOrder)
{
    m_localPiority = zOrder;
    if (m_parent)
    {
        m_parent->sortChildren();
    }
	EventMgr::shared()->notifyListenerChange();
	Renderer::shared()->notifySortGui();
}

static bool NodeSort(const Node *a,const Node *b)
{
    return a->getLocalPiority() < b->getLocalPiority();
}

void Node::sortChildren()
{
    std::stable_sort(m_children.begin(),m_children.end(),NodeSort);
}

Scene *Node::getScene() const
{
    return m_scene;
}

void Node::setScene(Scene *scene)
{
    m_scene = scene;
}

Quaternion Node::getRotateQ() const
{
    return m_rotateQ;
}

void Node::setRotateQ(const Quaternion &rotateQ)
{
    m_rotateQ = rotateQ;
    float x,y,z;
    m_rotateQ.toEulserAngel(&x,&y,&z);
    m_rotateE = vec3(x, y, z);
    m_needToUpdate = true;
}


} // namespace tzw

