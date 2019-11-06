#include "Node.h"

#include "../Interface/Drawable3D.h"
#include "../Scene/SceneMgr.h"
#include "../Engine/Engine.h"
#include "../Scene/Scene.h"
#include <algorithm>
#include "../Event/EventMgr.h"
#include "../Rendering/Renderer.h"
#include "Utility/log/Log.h"

namespace tzw {

Node::Node()
	: m_isVisible(true),
	  m_isValid(true),
	  m_scene(nullptr),
	  m_scale(vec3(1, 1, 1)),
	  m_rotateQ(Quaternion(0.0f, 0.0f, 0.0f, 1.0f)),
	  m_pos(vec3()),
	  m_needToUpdate(true),
	  m_localPiority(0),
	  m_globalPiority(0),
	  m_isAccpectOCTtree(true),
	  m_name("default"),
	  m_parent(nullptr), m_tag(0),
		onSubmitDrawCommand(nullptr)
{
	m_name = "default";
}

Node::~Node()
{
	for(auto child : m_children)
	{
		delete child;
	}
}

Node *Node::create()
{
    Node * node = new Node();
    return node;
}


Matrix44 Node::getTransform()
{
    if(m_needToUpdate)
    {
        cacheTransform();
    }
    return m_worldTransformCache;
}

Matrix44 Node::getLocalTransform()
{
    return getTranslationMatrix() * getRotationMatrix() * getScalingMatrix();
}

vec3 Node::getPos() const
{
    return m_pos;
}

void Node::setPos(const vec3 &pos)
{
    m_pos = pos;
	setNeedToUpdate(true);
}

void Node::setPos(float x, float y, float z)
{
	setPos(vec3(x, y, z));
}

void Node::submitDrawCmd(RenderCommand::RenderType passType)
{

}

void Node::logicUpdate(float dt)
{

}

vec3 Node::getRotateE()
{
    float x = m_rotateE.x, y = m_rotateE.y, z = m_rotateE.z;
    m_rotateQ.toEulserAngel(&x, &y, &z);
    //m_rotateE = vec3(x,y,z);
    return m_rotateE;
    //return vec3 (TbaseMath::Radius2Ang(theEulerAngle.x),TbaseMath::Radius2Ang(theEulerAngle.y),TbaseMath::Radius2Ang(theEulerAngle.z));
}

void Node::setRotateE(const vec3 &rotate)
{
    m_rotateE = rotate;
    //theRotate.x = TbaseMath::clampf(rotate.x,-60.f,60.f);
    //theRotate.y = TbaseMath::clampf(rotate.y,0.f,360.f);
    m_rotateQ.fromEulerAngle(m_rotateE);
    m_needToUpdate = true;
}

void Node::setRotateE(float x, float y, float z)
{
    setRotateE(vec3(x,y,z));
}

vec3 Node::getScale() const
{
    return m_scale;
}

void Node::setScale(const vec3 &scale)
{
    m_scale = scale;
    m_needToUpdate = true;
}

void Node::setScale(float x, float y, float z)
{
    setScale(vec3(x,y,z));
}

Matrix44 Node::getTranslationMatrix()
{
    Matrix44 mat;
    mat.translate(m_pos);
    return mat;
}

#define PI_OVER_180 (3.14159 / 180)

Matrix44 Node::getRotationMatrix()
{
    Matrix44 rotateMatrix;
    rotateMatrix.setToIdentity();
    m_rotateQ.normalize();
    rotateMatrix.rotate(m_rotateQ);
    return rotateMatrix;
}

Matrix44 Node::getScalingMatrix()
{
    Matrix44 s;
    s.scale(m_scale);
    return s;
}

void Node::visit(RenderCommand::RenderType passType)
{
	auto scene = g_GetCurrScene()->getOctreeScene();
    logicUpdate(Engine::shared()->deltaTime());
	if(!m_actionList.empty())
		updateAction(this,Engine::shared()->deltaTime());
	std::vector <Node *> removeList;

	if(getNeedToUpdate())
	{
		this->reCache();
		//setNeedToUpdate(false); //这里原来直接设了False，但是后面其实还用到了判断，太他妈蠢了，留个注释
	}
	if(m_isVisible && m_isValid)
	{
		if(!getIsAccpectOcTtree() || this->getNodeType() != NodeType::Drawable3D )
		{
			submitDrawCmd(passType);
			if(onSubmitDrawCommand)
			{
				onSubmitDrawCommand(RenderCommand::RenderType::Common);
			}
		}
		if(getNodeType()==NodeType::Drawable3D  && getIsAccpectOcTtree() && (getNeedToUpdate() || !scene->isInOctree(static_cast<Drawable3D *>(this))))
		{
			scene->updateObj(static_cast<Drawable3D *>(this));
			//and update threre child

		}
		////traversal the children
		for(auto child : m_children)
		{
			child->visit(passType);
			if(!child->m_isValid)
			{
				removeList.push_back(child);
			}
		}
	}
	if(getNeedToUpdate())
	{
		//this->reCache();
		setNeedToUpdate(false);
	}
	for( auto child : removeList)
	{
		detachChild(child);
		//delete child;
	}
}

void Node::addChild(Node *node, bool isNeedSort)
{
    if(node->m_parent)
    {
        tlogError("can not multiple adding");
    	exit(1);
    }else
    {
    	node->setIsValid(true);
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
	if (isNeedSort)
	{
		Renderer::shared()->notifySortGui();
	}
}

Node *Node::getParent() const
{
    return m_parent;
}

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

bool Node::getNeedToUpdate() const
{
    bool parentNeedToUpdate = false;
    if(m_parent)
    {
        parentNeedToUpdate = m_parent->getNeedToUpdate();
    }
    return m_needToUpdate || parentNeedToUpdate;
}

void Node::setNeedToUpdate(bool needToUpdate)
{
    m_needToUpdate = needToUpdate;
	for(auto child :m_children)
	{
		child->setNeedToUpdate(needToUpdate);
	}
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
            node->removeAllChildrenR();
        }
        m_children.clear();
    }
}

void Node::removeAllChildrenR()
{
    if(!m_children.empty())
    {
        for( Node * node :m_children)
        {
            node->removeAllChildrenR();
        }
        m_children.clear();
    }
    delete this;
}

unsigned int Node::getGlobalPiority() const
{
    return m_globalPiority;
}

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

unsigned int Node::getTypeId()
{
	return 0;
}

bool Node::getIsAccpectOcTtree() const
{
    return m_isAccpectOCTtree;
}

void Node::setIsAccpectOcTtree(bool isAccpectOCTtree)
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

void Node::reCache()
{
    cacheTransform();
	for (auto child : m_children)
	{
		child->reCache();
	}

}

bool Node::getIsValid() const
{
    return m_isValid;
}

void Node::setIsValid(bool isValid)
{
    m_isValid = isValid;
}

void Node::detachChild(Node *node)
{
    auto result = std::find(m_children.begin(),m_children.end(),node);
    if (result != m_children.end())
    {
        m_children.erase(result);
    }
}

void Node::removeFromParent()
{
	auto scene = g_GetCurrScene()->getOctreeScene();
    setIsValid(false);
	m_parent->detachChild(this);
	m_parent = nullptr;
	if(getNodeType()==NodeType::Drawable3D  && getIsAccpectOcTtree())
	{
		scene->removeObj(static_cast<Drawable3D *>(this));
	}
	//recursive remove children from octree
	for(auto child : m_children)
	{	
		if(child->getNodeType()==NodeType::Drawable3D  && child->getIsAccpectOcTtree())
		{
			scene->removeObj(static_cast<Drawable3D *>(child));
		}
	}
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

vec3 Node::getUp()
{
    auto m = getTransform().data();
    return vec3( m[4], m[5], m[6]);
}

int Node::getLocalPiority() const
{
    return m_localPiority;
}

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
	for(auto child: m_children)
	{
		child->setScene(scene);
	}
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

