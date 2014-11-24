#ifndef BONEDATA_H
#define BONEDATA_H
#define MAX_BONES 4
#include <QOpenGLFunctions>
struct BoneData
{
    int IDs[MAX_BONES];
    float weights[MAX_BONES];
    BoneData();
    void addBoneData(int id, float weight);
};

#endif // BONEDATA_H
