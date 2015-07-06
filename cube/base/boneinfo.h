#ifndef BONEINFO_H
#define BONEINFO_H

#include "ogldev_math_3d.h"
#include <qmatrix4x4.h>
class BoneInfo
{
public:
    Matrix4f BoneOffset;
    Matrix4f FinalTransformation;
    QMatrix4x4 finalTrasnformation;
    BoneInfo();
};
#endif // BONEINFO_H
