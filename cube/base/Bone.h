#ifndef BONE_H
#define BONE_H

#include "ogldev_math_3d.h"
#include <string>
#include <vector>
class Bone
{
public:
    Bone();
    ~Bone();
    Matrix4f boneDefaultOffset() const;
    void setBoneDefaultOffset(const Matrix4f &boneDefaultOffset);

    std::string name() const;
    void setName(const std::string &name);

    Matrix4f finalTransformation() const;
    void setFinalTransformation(const Matrix4f &finalTransformation);

    void addChild(Bone * bone);
private:
    std::vector<Bone * >m_children;
    std::string m_name;
    Matrix4f m_boneDefaultOffset;
    Matrix4f m_finalTransformation;
};

#endif // BONE_H
