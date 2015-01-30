#include "node.h"
#include "scene/scene.h"
Node::Node()
{
    this->m_pos=QVector3D(0,0,0);
    this->m_rotation=QVector3D(0,0,0);
    this->m_scalling=QVector3D(1,1,1);
    setNodeType (NODE_TYPE_NODE);
    this->m_parent = NULL;
    this->m_name = "node";
    this->m_groupMask = (unsigned int)NodeGroup::G_Default;
}

void Node::addChild(Node *child)
{
    m_children.push_back (child);
    child->setParent (this);
}
Node *Node::parent() const
{
    return m_parent;
}

void Node::setParent(Node *parent)
{
    m_parent = parent;
}

std::vector<Node *> Node::children() const
{
    return m_children;
}

void Node::setChildren(const std::vector<Node *> &children)
{
    m_children = children;
}

void Node::visit(Scene *scene)
{
    for(int i =0;i<m_children.size ();i++)
    {
        m_children[i]->visit(scene);
    }
    switch(nodeType())
    {
    case NODE_TYPE_ENTITY:
    {
        Entity *entity = (Entity * )this;
        if(scene)
        {
            scene->pushEntityToRenderQueue (entity);
        }
    }
        break;
    case NODE_TYPE_SPRITE:
    {
        auto sprite = (Sprite *)this;
        if(scene)
        {
            scene->pushSpriteToRenderQueue (sprite);
        }
    }
        break;
    }
}
int Node::nodeType() const
{
    return m_nodeType;
}

void Node::setNodeType(int nodeType)
{
    m_nodeType = nodeType;
}

void Node::translate(float x, float y, float z)
{
    m_pos=QVector3D(x,y,z);
}

void Node::rotate(float x, float y, float z)
{
    m_rotation=QVector3D(x,y,z);
}

void Node::scale(float x, float y, float z)
{
    m_scalling = QVector3D(x,y,z);
}

QMatrix4x4 Node::getModelTrans()
{
    return getParentModelTrans()*getTranslationMatrix() *getRotaionMatrix() *getScallingMatrix();
}

QMatrix4x4 Node::getParentModelTrans()
{
    if(m_parent)
    {
        return m_parent->getModelTrans ();
    }else{
        QMatrix4x4 mat;
        mat.setToIdentity ();
        return mat;
    }
}

QMatrix4x4 Node::getRotaionMatrix()
{
    QMatrix4x4 rotateMatrix;
    rotateMatrix.setToIdentity();
    rotateMatrix.rotate(this->m_rotation.y(),QVector3D(0,1,0));//yaw - pan
    rotateMatrix.rotate(this->m_rotation.x(),QVector3D(1,0,0));//pitch - tilt
    rotateMatrix.rotate(this->m_rotation.z(),QVector3D(0,0,1));//roll
    return rotateMatrix;
}

QMatrix4x4 Node::getScallingMatrix()
{
    QMatrix4x4 scaleMatrix;
    scaleMatrix.setToIdentity();
    scaleMatrix.scale(TVector3D(this->m_scalling.x(),this->m_scalling.y(),this->m_scalling.z()));
    return scaleMatrix;
}

QMatrix4x4 Node::getTranslationMatrix()
{
    QMatrix4x4 translateMatrix;
    translateMatrix.setToIdentity();
    translateMatrix.translate(m_pos.x(),m_pos.y(),m_pos.z());
    return translateMatrix;
}
const char * Node::name()
{
    return m_name.c_str ();
}

void Node::setName(const char * name)
{
    m_name = name;
}

void Node::setYaw(float yaw)
{
    this->m_rotation.setY (yaw);
}

float Node::getYaw()
{
    return this->m_rotation.y ();
}

void Node::setPitch(float pitch)
{
    this->m_rotation.setX (pitch);
}

float Node::getPitch()
{
    return this->m_rotation.x ();
}

void Node::setRoll(float roll)
{
    this->m_rotation.setZ (roll);
}

float Node::getRoll()
{
    return this->m_rotation.z ();
}

void Node::yawBy(float value)
{
    m_rotation.setY(m_rotation.y ()+value);
}

void Node::pitchBy(float value)
{
    m_rotation.setX(m_rotation.x ()+value);
}

void Node::rollBy(float value)
{
    m_rotation.setZ(m_rotation.z ()+value);
}

void Node::moveBy(float the_x, float the_y, float the_z)
{
    QVector4D v(the_x,the_y,the_z,1);
    v = this->getRotaionMatrix ()*v;
    this->m_pos += TVector3D(v.x(),v.y(),v.z());
}

void Node::move(float the_x, float the_y, float the_z)
{
    this->m_pos = TVector3D(the_x,the_y,the_z);
}
TVector3D Node::pos() const
{
    return m_pos;
}

void Node::setPos(const TVector3D &pos)
{
    m_pos = pos;
}
TVector3D Node::rotation() const
{
    return m_rotation;
}

void Node::setRotation(const TVector3D &rotation)
{
    m_rotation = rotation;
}
TVector3D Node::scalling() const
{
    return m_scalling;
}

void Node::setGroupMask(NodeGroup mask)
{
    m_groupMask =  (unsigned int )mask;
}

bool Node::isGroupMask(NodeGroup mask)
{
    return m_groupMask & (unsigned int )mask;
}


void Node::setScalling(const TVector3D &scalling)
{
    m_scalling = scalling;
}









