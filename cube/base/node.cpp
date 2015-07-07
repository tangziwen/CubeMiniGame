#include "node.h"
#include "scene/scene.h"
#include "utility.h"
#include <algorithm>
Node::Node()
    :m_enable(true),m_camera(nullptr)
{
    this->m_pos=QVector3D(0,0,0);
    this->m_rotation=QVector3D(0,0,0);
    this->m_scalling=QVector3D(1,1,1);
    setNodeType (NODE_TYPE_NODE);
    this->m_parent = NULL;
    this->m_name = "node";
    this->m_groupMask = (unsigned int)NodeGroup::G_Default;
    onUpdate = nullptr;
}

void Node::addChild(Node *child)
{
    if(child->parent ()){
        T_LOG<<"your child has already add in a node";
    }else{
        m_children.push_back (child);
        child->setParent (this);
    }
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
    if(!m_enable) return;
    if(onUpdate) onUpdate(this);
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
    case NODE_TYPE_CUSTOM:
    {
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

static bool decomposeMatrix(QMatrix4x4 src,QVector3D * pos,QQuaternion * rotation,QVector3D * scale)
{
    auto m = src.data ();
    if (pos)
        {
            // Extract the translation.
        pos->setX (m[12]);
        pos->setY (m[13]);
        pos->setZ (m[14]);
        }

        // Nothing left to do.
        if (scale == nullptr && rotation == nullptr)
            return true;

        // Extract the scale.
        // This is simply the length of each axis (row/column) in the matrix.
        QVector3D xaxis(m[0], m[1], m[2]);
        float scaleX = xaxis.length ();

        QVector3D yaxis(m[4], m[5], m[6]);
        float scaleY = yaxis.length();

        QVector3D zaxis(m[8], m[9], m[10]);
        float scaleZ = zaxis.length();

        // Determine if we have a negative scale (true if determinant is less than zero).
        // In this case, we simply negate a single axis of the scale.
        float det = src.determinant ();
        if (det < 0)
            scaleZ = -scaleZ;

        if (scale)
        {
            scale->setX (scaleX);
            scale->setY (scaleY);
            scale->setZ (scaleZ);
        }

        // Nothing left to do.
        if (rotation == nullptr)
            return true;

        // Scale too close to zero, can't decompose rotation.
        if (scaleX < 0.00001 || scaleY < 00001 || fabs(scaleZ) < 00001)
            return false;

        float rn;

        // Factor the scale out of the matrix axes.
        rn = 1.0f / scaleX;
        xaxis *=rn;

        rn = 1.0f / scaleY;
        yaxis *= rn;


        rn = 1.0f / scaleZ;
        zaxis *= rn;


        // Now calculate the rotation from the resulting matrix (axes).
        float trace = xaxis.x() + yaxis.y() + zaxis.z() + 1.0f;

        if (trace > 0.000001f)
        {
            float s = 0.5f / sqrt(trace);
            rotation->setScalar (0.25f / s);
            rotation->setX ((yaxis.z() - zaxis.y()) * s);
            rotation->setY ((zaxis.x() - xaxis.z()) * s);
            rotation->setZ ((xaxis.y() - yaxis.x()) * s);
        }
        else
        {
            // Note: since xaxis, yaxis, and zaxis are normalized,
            // we will never divide by zero in the code below.
            if (xaxis.x() > yaxis.y() && xaxis.x() > zaxis.z())
            {
                float s = 0.5f / sqrt(1.0f + xaxis.x() - yaxis.y() - zaxis.z());
                rotation->setScalar ((yaxis.z() - zaxis.y()) * s);
                rotation->setX (0.25f / s);
                rotation->setY ((yaxis.x() + xaxis.y()) * s);
                rotation->setZ ((zaxis.x() + xaxis.z()) * s);
            }
            else if (yaxis.y() > zaxis.z())
            {
                float s = 0.5f / sqrt(1.0f + yaxis.y() - xaxis.x ()- zaxis.z());
                rotation->setScalar ((zaxis.x() - xaxis.z()) * s);
                rotation->setX ((yaxis.x() + xaxis.y()) * s);
                rotation->setY (0.25f / s);
                rotation->setZ ((zaxis.y() + yaxis.z()) * s);
            }
            else
            {
                float s = 0.5f / sqrt(1.0f + zaxis.z() - xaxis.x ()- yaxis.y() );
                rotation->setScalar ((xaxis.y() - yaxis.x() ) * s);
                rotation->setX ((zaxis.x() + xaxis.z() ) * s);
                rotation->setY ((zaxis.y() + yaxis.z() ) * s);
                rotation->setZ (0.25f / s);
            }
        }
        return true;
}

QVector3D quaternionToEuler(QQuaternion q)
{
    float rotate_x,rotate_y,rotate_z;
    //convert quaternion to Euler angle
    float x = q.x (), y = q.y (), z = q.z(), w = q.scalar ();
    rotate_x = atan2f(2.f * (w * x + y * z), 1.f - 2.f * (x * x + y * y));
    rotate_y = asinf(2.f * (w * y - z * x));
    rotate_z = atan2f(2.f * (w * z + x * y), 1.f - 2.f * (y * y + z * z));

    rotate_x = utility::Radius2Ang (rotate_x);
    rotate_y = utility::Radius2Ang (rotate_y);
    rotate_z = utility::Radius2Ang (rotate_z);
    return QVector3D(rotate_x,rotate_y,rotate_z);
}

void Node::setTransform(QMatrix4x4 matrix)
{
    QVector3D pos,scale;
    QQuaternion rotate;
    decomposeMatrix(matrix,&pos,&rotate,&scale);
    m_pos = pos;
    m_scalling = scale;
    m_rotation = quaternionToEuler(rotate);
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

void Node::moveBy(float the_x, float the_y, float the_z, bool isRelative)
{
    if(isRelative)
    {
        QVector4D v(the_x,the_y,the_z,1);
        v = this->getRotaionMatrix ()*v;
        this->m_pos += TVector3D(v.x(),v.y(),v.z());
    }else
    {
        this->m_pos += TVector3D(the_x,the_y,the_z);
    }

}

void Node::move(float the_x, float the_y, float the_z)
{
    this->m_pos = TVector3D(the_x,the_y,the_z);
}

void Node::removeFromParent()
{
    if(T_INVALID(m_parent)) return;

    m_parent->removechild (this);
}

void Node::removechild(Node *child)
{
    if(T_INVALID(child)) return;

    auto iter = std::find(m_children.begin (),m_children.end (),child);
    if(iter !=m_children.end ())
    {
        Node * node = (*iter);
        node->setIsRemoved (true);
        m_children.erase (iter);
    }
}

void Node::removeAllChildren()
{
    for(Node * node : m_children)
    {
        delete node;
    }
    m_children.clear ();
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

QVector3D Node::getForwardVector()
{
    QVector4D v(0,0,-1,1);
    v = this->getRotaionMatrix ()*v;
    return v.toVector3D ();
}

Node *Node::getChild(int index)
{
    return m_children[index];
}

int Node::getChildrenAmount()
{
    return m_children.size ();
}

void Node::setAsCustomNode()
{
    m_nodeType = NODE_TYPE_CUSTOM;
}
Camera *Node::camera() const
{
    return m_camera;
}

void Node::setCamera(Camera *camera)
{
    m_camera = camera;
}

bool Node::enable() const
{
    return m_enable;
}

void Node::setEnable(bool enable)
{
    m_enable = enable;
}



void Node::setScalling(const TVector3D &scalling)
{
    m_scalling = scalling;
}
