#include "Bone.h"

Bone::Bone()
{

}

Bone::~Bone()
{

}
Matrix4f Bone::boneDefaultOffset() const
{
    return m_boneDefaultOffset;
}

void Bone::setBoneDefaultOffset(const Matrix4f &boneDefaultOffset)
{
    m_boneDefaultOffset = boneDefaultOffset;
}
std::string Bone::name() const
{
    return m_name;
}

void Bone::setName(const std::string &name)
{
    m_name = name;
}
Matrix4f Bone::finalTransformation() const
{
    return m_finalTransformation;
}

void Bone::setFinalTransformation(const Matrix4f &finalTransformation)
{
    m_finalTransformation = finalTransformation;
}

void Bone::addChild(Bone *bone)
{
    m_children.push_back (bone);
}




