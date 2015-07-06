#include "CMC_AnimateBone.h"

namespace tzw {

CMC_AnimateBone::CMC_AnimateBone()
{

}

void CMC_AnimateBone::addTranslate(CMC_TranslateKey v)
{
    m_transKeys.push_back (v);
}

void CMC_AnimateBone::addScale(CMC_ScaleKey v)
{
    m_scaleKeys.push_back (v);
}

void CMC_AnimateBone::addRotate(CMC_RotateKey q)
{
    m_rotateKeys.push_back (q);
}

void CMC_AnimateBone::calcInterpolatedScaling(QVector3D &out, float AnimationTime)const
{
    if (m_scaleKeys.size ()== 1) {
        out = m_scaleKeys[0].scale;
        return;
    }

    uint ScalingIndex = findBoneInterpoScaling(AnimationTime);
    uint NextScalingIndex = (ScalingIndex + 1);
    float DeltaTime = (float)(m_scaleKeys[NextScalingIndex].time - m_scaleKeys[ScalingIndex].time);
    float Factor = (AnimationTime - (float)m_scaleKeys[ScalingIndex].time) / DeltaTime;
    const QVector3D & Start = m_scaleKeys[ScalingIndex].scale;
    const QVector3D& End   = m_scaleKeys[NextScalingIndex].scale;
    QVector3D Delta = End - Start;
    out = Start + Factor * Delta;
}

void CMC_AnimateBone::calcInterpolatedRotation(QQuaternion &out, float AnimationTime) const
{
    // we need at least two values to interpolate...
    if (m_rotateKeys.size ()== 1) {
        out = m_rotateKeys[0].rotate;
        return;
    }

    uint RotationIndex = findBoneInterpoRotation(AnimationTime);
    uint NextRotationIndex = (RotationIndex + 1);
    float DeltaTime = (float)(m_rotateKeys[NextRotationIndex].time - m_rotateKeys[RotationIndex].time);
    float Factor = (AnimationTime - (float)m_rotateKeys[RotationIndex].time) / DeltaTime;

    const QQuaternion& StartRotationQ = m_rotateKeys[RotationIndex].rotate;
    const QQuaternion& EndRotationQ   = m_rotateKeys[NextRotationIndex].rotate;
    out = QQuaternion::slerp (StartRotationQ,EndRotationQ,Factor);
    out.normalize ();
}

void CMC_AnimateBone::calcInterpolatedPosition(QVector3D &out, float AnimationTime) const
{
    if (m_transKeys.size ()== 1) {
        out = m_transKeys[0].trans;
        return;
    }

    uint PositionIndex = findBoneInterpoTranslation(AnimationTime);
    uint NextPositionIndex = (PositionIndex + 1);
    float DeltaTime = (float)(m_transKeys[NextPositionIndex].time - m_transKeys[PositionIndex].time);
    float Factor = (AnimationTime - (float)m_transKeys[PositionIndex].time) / DeltaTime;
    const QVector3D& Start = m_transKeys[PositionIndex].trans;
    const QVector3D& End = m_transKeys[NextPositionIndex].trans;
    QVector3D Delta = End - Start;
    out = Start + Factor * Delta;
}

int CMC_AnimateBone::findBoneInterpoScaling(float AnimationTime) const
{
    for (uint i = 0 ; i < m_scaleKeys.size ()- 1 ; i++) {
        if (AnimationTime < (float)m_scaleKeys[i+1].time) {
            return i;
        }
    }
}

int CMC_AnimateBone::findBoneInterpoRotation(float AnimationTime) const
{
    for (uint i = 0 ; i < m_rotateKeys.size ()- 1 ; i++) {
        if (AnimationTime < (float)m_rotateKeys[i + 1].time) {
            return i;
        }
    }
}

int CMC_AnimateBone::findBoneInterpoTranslation(float AnimationTime) const
{
    for (uint i = 0 ; i < m_transKeys.size ()- 1 ; i++) {
        if (AnimationTime < (float)m_transKeys[i + 1].time) {
            return i;
        }
    }
}

} // namespace tzw

