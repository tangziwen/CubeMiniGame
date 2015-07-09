#ifndef TZW_CMC_MODEL_H
#define TZW_CMC_MODEL_H

#include "CMC_Mesh.h"
#include <vector>
#include <QMatrix4x4>
#include "CMC_Material.h"
#include <map>
#include "CMC_Bone.h"
#include "CMC_Animate.h"
namespace tzw {
class CMC_Model
{
public:
    CMC_Model();
    ~CMC_Model();
    void addMesh(CMC_Mesh * mesh);
    QMatrix4x4 globalInverseTransform() const;
    void setGlobalInverseTransform(const QMatrix4x4 &globalInverseTransform);
    void writeToFile(const char * fielName);
    void addMaterial(CMC_Material * material);
    void loadFromTZW(const char * fileName);
    bool m_hasAnimation;
    CMC_Bone *rootBone() const;
    void setRootBone(CMC_Bone *rootBone);
    std::map<std::string,int> m_BoneMetaInfoMapping;
    std::vector<CMC_BoneMetaInfo * > m_boneMetaInfoList;
    int m_numBones;
    CMC_Animate m_animate;
    std::vector<CMC_Material *> m_materialList;
    std::vector<CMC_Mesh * > m_meshList;
private:
    CMC_Bone * m_rootBone;

    QMatrix4x4 m_globalInverseTransform;
};

} // namespace tzw

#endif // TZW_CMC_MODEL_H
