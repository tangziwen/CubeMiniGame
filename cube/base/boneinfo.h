#ifndef BONEINFO_H
#define BONEINFO_H

#include "ogldev_math_3d.h"
class BoneInfo
{
public:
    Matrix4f BoneOffset;
    Matrix4f FinalTransformation;
    BoneInfo();
};
#endif // BONEINFO_H
