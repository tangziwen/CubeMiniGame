#include "Loader.h"
#include "CMC_Vertex.h"
#include "external/TUtility/file/Tfile.h"
namespace tzw {

QMatrix4x4 toQMatrix(aiMatrix4x4 mat)
{
    QMatrix4x4 m(mat.a1,mat.a2,mat.a3,mat.a4,
                 mat.b1,mat.b2,mat.b3,mat.b4,
                 mat.c1,mat.c2,mat.c3,mat.c4,
                 mat.d1,mat.d2,mat.d3,mat.d4
                );
    return m;
}

Loader::Loader()
    :m_pScene(nullptr),m_model(nullptr)
{

}

Loader::~Loader()
{

}
CMC_ModelData *Loader::model() const
{
    return m_model;
}

void Loader::setModel(CMC_ModelData *model)
{
    m_model = model;
}

bool Loader::loadFromModel(const char *fileName)
{
    printf("load %s begin\n",fileName);
    printf("parsing..\n");
    const aiScene* pScene = m_Importer.ReadFile(fileName,aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals |aiProcess_CalcTangentSpace);
    this->m_pScene = (aiScene*)pScene;
    if(!m_pScene)
    {
        printf("parsing failure!\n");
        return false;
    }
    else
    {
        printf("parsing succes!\n");
        printf("extracting data to memory...\n");
        m_model = new CMC_ModelData();

        auto inverseTransform = pScene->mRootNode->mTransformation;
        inverseTransform = inverseTransform.Inverse();
        m_model->setGlobalInverseTransform (toQMatrix(inverseTransform));
        LoadMaterial(pScene,fileName);

        const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
        for(int i =0 ;i< pScene->mNumMeshes ;i++)
        {
            const aiMesh* the_mesh = pScene->mMeshes[i];
            auto mesh = new CMC_MeshData;
            m_model->addMesh(mesh);
            //set material
            mesh->setMaterialIndex (the_mesh->mMaterialIndex);
            for(int j =0; j<the_mesh->mNumVertices;j++)
            {
                const aiVector3D* pPos = &(the_mesh->mVertices[j]);
                const aiVector3D* pNormal = &(the_mesh->mNormals[j]);
                const aiVector3D* pTexCoord = the_mesh->HasTextureCoords(0) ? &(the_mesh->mTextureCoords[0][j]) : &Zero3D;
                const aiVector3D* pTangent = &(the_mesh->mTangents[i]);
                CMC_Vertex  vec;
                vec.setPos (QVector3D(pPos->x,pPos->y,pPos->z));
                vec.setNormal (QVector3D(pNormal->x,pNormal->y,pNormal->z));
                vec.setUV (QVector2D(pTexCoord->x,pTexCoord->y));
            if(pTangent)
            {
                 vec.setTangent (QVector3D(pTangent->x,pTangent->y,pTangent->z));
            }
            mesh->pushVertex (vec);
            }

            for (unsigned int k = 0 ; k < the_mesh->mNumFaces ; k++) {
                const aiFace& Face = the_mesh->mFaces[k];
                assert(Face.mNumIndices == 3);
                mesh->pushIndex (Face.mIndices[0]);
                mesh->pushIndex(Face.mIndices[1]);
                mesh->pushIndex(Face.mIndices[2]);
            }
            //load bones
            loadBoneList(the_mesh,mesh);
            //mesh->finish();
        }
        loadNodeHeirarchy(nullptr,m_pScene->mRootNode);
        loadAnimations ();
        printf("extracting finish..\n");
        return true;
    }
}

void Loader::loadBoneList(const aiMesh *pMesh, CMC_MeshData *mesh)
{
    m_model->m_hasAnimation = m_pScene->HasAnimations();
    if(!m_model->m_hasAnimation)
    {
        m_model->setRootBone (nullptr);
        return;
    }
    for (uint i = 0 ; i < pMesh->mNumBones ; i++) {
        uint BoneIndex = 0;
        std::string BoneName(pMesh->mBones[i]->mName.data);
        if (m_model->m_BoneMetaInfoMapping.find(BoneName) == m_model->m_BoneMetaInfoMapping.end()) {
            // Allocate an index for a new bone
            BoneIndex = m_model->m_numBones;
            m_model->m_numBones++;
            CMC_NodeMetaInfo * metaInfo = new CMC_NodeMetaInfo();
            metaInfo->setName (BoneName);
            auto matrix_assimp = pMesh->mBones[i]->mOffsetMatrix;
            auto matrix_qt = toQMatrix(matrix_assimp);
            metaInfo->setDefaultBoneOffset (matrix_qt);
            m_model->m_boneMetaInfoList.push_back (metaInfo);
            m_model->m_BoneMetaInfoMapping[BoneName] = BoneIndex;
        }
        else {
            BoneIndex = m_model->m_BoneMetaInfoMapping[BoneName];
        }
        for (uint j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) {//load bones weight and index to mesh's vertices..
            uint VertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight  = pMesh->mBones[i]->mWeights[j].mWeight;
            mesh->getVertex (VertexID)->addBoneData (BoneIndex,Weight);
        }
    }
}

void Loader::LoadMaterial(const aiScene *pScene, const char *file_name)
{
    //store material
    for(int i = 0 ;i<pScene->mNumMaterials;i++)
    {
        aiColor3D dif(0.f,0.f,0.f);
        aiColor3D amb(0.f,0.f,0.f);
        aiColor3D spec(0.f,0.f,0.f);
        aiMaterial * the_material = pScene->mMaterials[i];
        CMC_Material * material = new CMC_Material();
        the_material->Get(AI_MATKEY_COLOR_AMBIENT,amb);
        the_material->Get(AI_MATKEY_COLOR_DIFFUSE,dif);
        the_material->Get(AI_MATKEY_COLOR_SPECULAR,spec);

        material->ambientColor = QVector3D(amb.r,amb.g,amb.b);
        material->diffuseColor = QVector3D(dif.r,dif.g,dif.b);
        material->specularColor = QVector3D(spec.r,spec.g,spec.b);
        aiString normalPath;
        the_material->GetTexture (aiTextureType_NORMALS,0,&normalPath);
        //now ,we only use the first diffuse texture, will fix it later
        aiString diffuse_Path;
        the_material->GetTexture(aiTextureType_DIFFUSE,0,&diffuse_Path);
        if(diffuse_Path.length == 0 )
        {
            material->diffuseTexturePath = "INVALID";
        }else
        {
            material->diffuseTexturePath = diffuse_Path.C_Str ();
        }
        m_model->addMaterial (material);
    }
}

void Loader::loadNodeHeirarchy(CMC_Node *parentNode, const aiNode *pNode)
{
    CMC_Node *  node = nullptr;
    auto NodeName = pNode->mName.C_Str ();
    if(!m_model->rootBone ())
    {
        m_model->setRootBone (new CMC_Node());
        node = m_model->rootBone ();
    }
    else
    {
        node = new CMC_Node();
        parentNode->addChild (node);
    }

    //load the index of meshes which attach to the node.
    for(int i =0;i<pNode->mNumMeshes;i++)
    {
        node->m_childrenMeshes.push_back (pNode->mMeshes[i]);
    }

    //check if it is a bone
    if (m_model->m_BoneMetaInfoMapping.find(NodeName) != m_model->m_BoneMetaInfoMapping.end ()) {
        uint BoneIndex = m_model->m_BoneMetaInfoMapping[NodeName];
        auto  boneinfo = m_model->m_boneMetaInfoList[BoneIndex];
        node->setInfo (boneinfo);
    }else //not a bone ,just a normal node ,we use the identy matrix.
    {

        auto nodeInfo = new CMC_NodeMetaInfo();
        nodeInfo->setName (NodeName);
        QMatrix4x4 mat;
        mat.setToIdentity ();
        nodeInfo->setDefaultBoneOffset (mat);
        node->setInfo (nodeInfo);
    }
    node->m_localTransform = toQMatrix (pNode->mTransformation);//set the node to it's parent node transformation.
    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        loadNodeHeirarchy(node,pNode->mChildren[i]);
    }
}

void Loader::loadAnimation(const aiNode *pNode,int index,CMC_AnimateData * animate)
{
    std::string NodeName(pNode->mName.data);
    if(!m_pScene->HasAnimations ())
    {
        m_model->m_hasAnimation = false;
        return ;
    }
    const aiAnimation* pAnimation = m_pScene->mAnimations[index];
    const aiNodeAnim * pNodeAnim = findNodeAnim(pAnimation, NodeName);
    if(pNodeAnim)//that node is a animation bone.
    {
        auto animateBone = new CMC_AnimateBone();
        animateBone->m_boneName = NodeName;
        //load position key.
        for(int i =0;i<pNodeAnim->mNumPositionKeys;i++)
        {
            auto v = pNodeAnim->mPositionKeys[i];
            CMC_TranslateKey key;
            key.time = v.mTime;
            key.trans = QVector3D (v.mValue.x,v.mValue.y,v.mValue.z);
            animateBone->addTranslate (key);
        }
        //load scale key
        for(int i =0;i<pNodeAnim->mNumScalingKeys;i++)
        {
            auto v = pNodeAnim->mScalingKeys[i];
            CMC_ScaleKey key;
            key.time = v.mTime;
            key.scale = QVector3D (v.mValue.x,v.mValue.y,v.mValue.z);
            animateBone->addScale (key);
        }
        //load rotation key
        for(int i =0;i<pNodeAnim->mNumPositionKeys;i++)
        {
            auto v = pNodeAnim->mRotationKeys[i];
            CMC_RotateKey key;
            key.time = v.mTime;
            key.rotate = QQuaternion(v.mValue.w,v.mValue.x,v.mValue.y,v.mValue.z);
            animateBone->addRotate (key);
        }

        animate->addAnimateBone (animateBone);
        animate->m_ticksPerSecond = pAnimation->mTicksPerSecond;
        animate->m_duration = pAnimation->mDuration;
    }
    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        loadAnimation( pNode->mChildren[i],index,animate);
    }
}

void Loader::loadAnimations()
{
    if(m_pScene->HasAnimations ())
    {
        for(int i =0;i<m_pScene->mNumAnimations;i++)
        {
            CMC_AnimateData * animate = new CMC_AnimateData();
            animate->m_name = m_pScene->mAnimations[i]->mName.C_Str ();
            loadAnimation (m_pScene->mRootNode,i,animate);
            m_model->m_animates.push_back (animate);
        }
    }
}

const aiNodeAnim *Loader::findNodeAnim(const aiAnimation *pAnimation, const std::string NodeName)
{
    for (uint i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }
    return NULL;
}

int Loader::findBoneIndex(const std::string NodeName)
{
    auto result = m_model->m_BoneMetaInfoMapping.find (NodeName);
    if(result!=m_model->m_BoneMetaInfoMapping.end ())
    {
        return m_model->m_BoneMetaInfoMapping[NodeName];
    }else
    {
        return -1;
    }
}


} // namespace tzw

