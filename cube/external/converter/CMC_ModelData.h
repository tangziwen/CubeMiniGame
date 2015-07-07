#ifndef TZW_CMC_MODEL_H
#define TZW_CMC_MODEL_H

#include "CMC_MeshData.h"
#include <vector>
#include <QMatrix4x4>
#include "CMC_Material.h"
#include <map>
#include "CMC_Node.h"
#include "CMC_Animate.h"
namespace tzw {
class CMC_ModelData
{
public:
    CMC_ModelData();
    ~CMC_ModelData();
    void addMesh(CMC_MeshData * mesh);
    QMatrix4x4 globalInverseTransform() const;
    void setGlobalInverseTransform(const QMatrix4x4 &globalInverseTransform);
    void writeToFile(const char * fielName);
    void addMaterial(CMC_Material * material);
    void loadFromTZW(const char * fileName);
    bool m_hasAnimation;
    CMC_Node *rootBone() const;
    void setRootBone(CMC_Node *rootBone);
    std::map<std::string,int> m_BoneMetaInfoMapping;
    std::vector<CMC_NodeMetaInfo * > m_boneMetaInfoList;
    int m_numBones;
    CMC_Animate m_animate;
    std::vector<CMC_Material *> m_materialList;
    std::vector<CMC_MeshData * > m_meshList;
private:
    CMC_Node * m_rootBone;

    QMatrix4x4 m_globalInverseTransform;
};

} // namespace tzw

#endif // TZW_CMC_MODEL_H
