#ifndef TZW_CMC_ASSIMPLOADER_H
#define TZW_CMC_ASSIMPLOADER_H

#include "external/include/assimp/scene.h"
#include "external/include/assimp/Importer.hpp"
#include "external/include/assimp/postprocess.h"

#include "CMC_ModelData.h"
#include "CMC_Material.h"
namespace tzw {

class Loader
{
public:
    Loader();
    ~Loader();

    CMC_ModelData *model() const;
    void setModel(CMC_ModelData *model);

    bool loadFromModel(const char * fileName);
private:
    void loadBoneList(const aiMesh* pMesh, CMC_MeshData * mesh);
    void LoadMaterial(const aiScene* pScene, const char *file_name);
    void loadNodeHeirarchy(CMC_Node  *paretnBone, const aiNode * pNode);
    void loadAnimation(const aiNode * pNode, int index, CMC_AnimateData *animate);
    void loadAnimations();
    const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
    int findBoneIndex(const std::string NodeName);
    aiScene* m_pScene;
    Assimp::Importer m_Importer;
    CMC_ModelData *m_model;
};

} // namespace tzw

#endif // TZW_CMC_ASSIMPLOADER_H
