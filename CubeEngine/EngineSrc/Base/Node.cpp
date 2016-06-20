#include "Node.h"

#include "../Interface/Drawable3D.h"
#include "../Scene/SceneMgr.h"
#include "../Engine/Engine.h"
#include "../Scene/Scene.h"
namespace tzw {
/**
 * @brief Node::Node 构造函数
 */
Node::Node()
    :m_isDrawable(true),
      m_isValid(true),
      m_scene(nullptr),
      m_scale(vec3(1,1,1)),
      m_rotateQ(QQuaternion(1.0f,0.0f,0.0f,0.0f)),
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
QMatrix4x4 Node::getTransform()
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
QMatrix4x4 Node::getLocalTransform()
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

void Node::draw()
{

}

void Node::update(float dt)
{

}

/**
 * @brief Node::getRotate 获取节点的旋转欧拉角
 * @note 旋转量的x,y,z分量分别表示绕x轴,y轴,z轴旋转的角度(以角度记)，变换的顺序为roll(z)->picth(x)->yaw(y);
 * @return
 */
vec3 Node::getRotate() const
{
    vec3 theEulerAngle;
    m_rotateQ.getEulerAngles(&theEulerAngle.x,&theEulerAngle.y,&theEulerAngle.z);
    return theEulerAngle;
}

/**
 * @brief Node::setRotate 设置节点的节点的旋转欧拉角
 * @note 旋转量的x,y,z分量分别表示绕x轴,y轴,z轴旋转的角度(以角度记)，变换的顺序为roll(z)->picth(x)->yaw(y);
 * @param rotate
 */
void Node::setRotateE(const vec3 &rotate)
{
    m_rotateQ = QQuaternion::fromEulerAngles(QVector3D(rotate.x,rotate.y,rotate.z));
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
QMatrix4x4 Node::getTranslationMatrix()
{
    QMatrix4x4 mat;
    mat.translate(m_pos.x,m_pos.y,m_pos.z);
    return mat;
}

#define PI_OVER_180 (3.14159 / 180)
/**
 * @brief Node::getRotationMatrix 获取旋转矩阵
 * @return
 */
QMatrix4x4 Node::getRotationMatrix()
{
    QMatrix4x4 rotateMatrix;
    rotateMatrix.setToIdentity();
    m_rotateQ.normalize();
    rotateMatrix.rotate(m_rotateQ);
    return rotateMatrix;
}

/**
 * @brief Node::getScalingMatrix 获取缩放矩阵
 * @return
 */
QMatrix4x4 Node::getScalingMatrix()
{
    QMatrix4x4 s;
    s.scale(m_scale.x,m_scale.y,m_scale.z);
    return s;
}
///
/// \brief 递归遍历节点(逻辑更新用)
/// \note 该次递归函数在整个逻辑-渲染流程的最开始，对于使用场景管理的3D物体仅用于逻辑更新用，其递归时不会提交任何的render Command
/// 对于其他类型的Node，会在其执行完逻辑update后，立即
///
void Node::visit()
{
    update(Engine::shared()->deltaTime());
    updateAction(this,Engine::shared()->deltaTime());

    //数据如果有变动的话，重新缓存数据
    if(getNeedToUpdate())
    {
        this->reCache();
    }
    for(int i =0;i<m_children.size();i++)
    {
        Node * child = m_children[i];
        child->visit();
    }
}
///
/// \brief 递归遍历节点(绘制用)
/// \note 该次递归遍历由当前场景的根节点出发，自顶向下进行遍历，如果物体确实可见的情况下将会调用Draw方法进行渲染提交；
/// 同时该函数还会更新物体在八叉树上的位置或往八叉树上添加新节点，八叉树的节点添加是惰性的，只有当一个物体确实可见(自己的可绘制性以及父节点的可绘制性都为true)且尚不在八叉树上的时候，才会在该函数调用时此处被添加至八叉树，
/// 该函数的调用在八叉树的裁剪函数执行完之后调用，因此，在此处新加入的节点的可渲染性将在下一帧开始才会受到八叉树管理的影响
/// \param scene 场景的八叉树管理器
///
void Node::visitPost(OctreeScene *scene)
{
    if(m_isDrawable && m_isValid)
    {
        if(!getIsAccpectOCTtree() || this->getNodeType() != NodeType::Drawable3D )
        {
            draw();
        }
        if(getNeedToUpdate() && getNodeType()==NodeType::Drawable3D && getIsAccpectOCTtree())
        {
            scene->updateObj((Drawable3D *)this);
        }
        //遍历子节点
        for(int i =0;i<m_children.size();i++)
        {
            Node * child = m_children[i];
            child->visitPost(scene);
        }
    }
    //一个节点从逻辑更新至渲染提交的整个流程至此已经彻底结束,将重置缓存标记
    setNeedToUpdate(false);

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
        m_children.push_back(node);
        //refresh the new child's transform cache
        node->setNeedToUpdate(true);
        node->m_parent = this;
        if (node->getGlobalPiority() ==0)
        {
            node->setGlobalPiority(m_globalPiority);
        }
        //sort the children
        sortChildren();
    }
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
bool Node::getIsDrawable() const
{
    return m_isDrawable;
}

void Node::setIsDrawable(bool isDrawable)
{
    m_isDrawable = isDrawable;
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
/// 对于子节点而言，凡是其GlobalPiority不等于0，调用此函数后也会受到级联设置
/// \param globalPiority 全局渲染优先级
///
void Node::setGlobalPiority(unsigned int globalPiority)
{
    m_globalPiority = globalPiority;
    for(auto child :m_children)
    {
        if(child->getGlobalPiority() == 0)
        {
            child->setGlobalPiority(globalPiority);
        }
    }
}

size_t Node::getChildrenAmount()
{
    return m_children.size();
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
        QMatrix4x4 parentTransform;
        parentTransform.setToIdentity();
        if(m_parent)
        {
            parentTransform = m_parent->getTransform();
        }
        QMatrix4x4 localTransform = getLocalTransform();
        m_worldTransformCache = parentTransform * localTransform;
    }
}

///
/// \brief 重新缓存各种几何信息，世界矩阵，世界坐标下的AABB等
///
void Node::reCache()
{
    cacheTransform();
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

QQuaternion Node::getRotateQ() const
{
    return m_rotateQ;
}

void Node::setRotateQ(const QQuaternion &rotateQ)
{
    m_rotateQ = rotateQ;
}


} // namespace tzw

