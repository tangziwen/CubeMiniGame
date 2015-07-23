#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>
#include <vector>
#include <string>
#include <functional>
#include "base/tvector3d.h"

#include "base/removable.h"

#define NODE_TYPE_NODE 0
#define NODE_TYPE_ENTITY 1
#define NODE_TYPE_CAMERA 2
#define NODE_TYPE_SPRITE 4
#define NODE_TYPE_CUSTOM 5
#define NODE_TYPE_BONE 6

enum class NodeGroup
{
    G_Default = 1,
    G_1 = 1 << 1,
    G_2 = 1 << 2,
    G_3 = 1 << 3,
    G_4 = 1 << 4,
    G_5 = 1 << 5,
    G_6 = 1 << 6,
    G_7 = 1 << 7,
    G_8 = 1 << 8,
};

class Camera;
class Scene;
class Node :public Removable
{
public:
    Node();
    void addChild(Node * child);
    void removeFromParent();
    void removechild(Node * child);
    void removeAllChildren();
    Node *parent() const;
    void setParent(Node *parent);
    std::vector<Node *> children() const;
    void setChildren(const std::vector<Node *> &children);
    void visit(Scene *scene);
    int nodeType() const;
    void setNodeType(int nodeType);

    void setTransform(QMatrix4x4 matrix);
    void translate(float x, float y, float z);
    void rotate(float x,float y, float z);
    void scale(float x, float y ,float z);

    virtual QMatrix4x4 getModelTrans();
    QMatrix4x4 getParentModelTrans();
    QMatrix4x4 getRotaionMatrix();
    QMatrix4x4 getScallingMatrix();
    QMatrix4x4 getTranslationMatrix();
    const char *name() ;
    void setName(const char *name);

    void setYaw(float yaw);
    float getYaw();

    void setPitch(float pitch);
    float getPitch();

    void setRoll(float roll);
    float getRoll();

    void yawBy(float value);
    void pitchBy(float value);
    void rollBy(float value);
    void moveBy(float the_x, float the_y, float the_z,bool isRelative = true);
    void move(float the_x, float the_y, float the_z);


    TVector3D pos() const;
    void setPos(const QVector3D &pos);

    TVector3D rotation() const;
    void setRotation(const QVector3D &rotation);

    TVector3D scalling() const;
    void setScalling(const QVector3D &scalling);

    void setGroupMask(NodeGroup mask);

    bool isGroupMask(NodeGroup mask);

    QVector3D getForwardVector();

    Node * getChild(int index);

    int getChildrenAmount();
public:
    std::function<void (Node *)> onUpdate;
    bool enable() const;
    void setEnable(bool enable);

    virtual Camera *camera() const;
    virtual void setCamera(Camera *camera);

    unsigned int getZOrder() const;
    void setZOrder(unsigned int zOrder);

protected:
    void setAsCustomNode();
protected:
    unsigned int m_zOrder;
    Camera * m_camera;
    TVector3D m_pos;
    TVector3D m_rotation;
    TVector3D m_scalling;
    int m_nodeType;
    Node * m_parent;
    std::vector<Node *>m_children;
    std::string m_name;
    unsigned int m_groupMask;
    bool m_enable;
};

#endif // NODE_H
