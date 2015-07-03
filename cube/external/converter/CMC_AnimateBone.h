#ifndef TZW_CMC_ANIMATEBONE_H
#define TZW_CMC_ANIMATEBONE_H

#include <vector>
#include <QVector3D>
#include <QQuaternion>

namespace tzw {
struct CMC_TranslateKey
{
    QVector3D trans;
    float time;
};

struct CMC_ScaleKey
{
    QVector3D scale;
    float time;
};

struct CMC_RotateKey
{
    QQuaternion rotate;
    float time;
};

class CMC_AnimateBone
{
public:
    CMC_AnimateBone();
    std::vector<CMC_TranslateKey>m_transKeys;
    std::vector<CMC_ScaleKey>m_scaleKeys;
    std::vector<CMC_RotateKey>m_rotateKeys;
    int m_boneIndex;
    void addTranslate(CMC_TranslateKey v);
    void addScale(CMC_ScaleKey v);
    void addRotate(CMC_RotateKey q);
};

} // namespace tzw

#endif // TZW_CMC_ANIMATEBONE_H
